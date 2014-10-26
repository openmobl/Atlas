/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include <PalmOS.h>
#include <PalmTypes.h>

#include "Utils.h"

#include "macros.h"

/*
 * FUNCTION: GetObjectPtr
 *
 * DESCRIPTION:
 *
 * This routine returns a pointer to an object in the current form.
 *
 * PARAMETERS:
 *
 * formId
 *     id of the form to display
 *
 * RETURNED:
 *     address of object as a void pointer
 */

void *GetObjectPtr(UInt16 objectID)
{
    FormType *frmP;

    frmP = FrmGetActiveForm();
    return FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, objectID));
}

#define memNewChunkFlagAllowLarge			0x1000	// allow >64K allocations

SysAppInfoPtr 	SysGetAppInfo(SysAppInfoPtr *uiAppPP, SysAppInfoPtr *actionCodeAppPP)
							SYS_TRAP(sysTrapSysGetAppInfo);

MemPtr MemPtrNewLarge(UInt32 size)
{
	// Allocate a chunk of memory where the size of the memory might exceed
	// 64k.  There are a few potential problems here:
	//  (a) the memory cannot be saved to a database
	//  (b) if you forget to free the large chunk before
	//      you application quits, you will cause a MEMORY LEAK on
	//      Palm OS 5 and 5.1 .
	if (size > 65500) {
        SysAppInfoType   *unusedAppInfoP;
		UInt16 currentOwnerID = SysGetAppInfo(&unusedAppInfoP, &unusedAppInfoP)->memOwnerID;
		return MemChunkNew(0, size, currentOwnerID | memNewChunkFlagNonMovable | memNewChunkFlagAllowLarge);
	}
    
    return MemPtrNew(size);
}

MemPtr MemPtrRealloc(MemPtr ptr, UInt32 newSize)
{
    UInt32  bufSize = 0;
	MemPtr  tmpBuf = 0;
    
    if (!ptr || !newSize) {
        return (void *)-1;
    }
    
    bufSize = MemPtrSize(ptr);
    
    //Err resizeErr = MemPtrResize(ptr, newSize);
    //if (resizeErr != errNone) {
        tmpBuf = MemPtrNewLarge(newSize);
        
        if (!tmpBuf) {
            return (void *)-1;
        } else {
            MemMove(tmpBuf, ptr, MIN(bufSize, newSize));
            MemPtrFree(ptr);
            
            return tmpBuf;
        }
    //}
    
    //return ptr;
}

Char *StrDup(const Char *str)
{
    if (str) {
        Char *newStr = (Char *)MemPtrNew(StrLen(str) + 1);
        
        if (newStr) {
            StrCopy(newStr, str);
            
            return newStr;
        }
    }
    
    return NULL;
}
