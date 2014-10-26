/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#ifndef Utils_h
#define Utils_h

extern void *GetObjectPtr(UInt16 objectID);
extern MemPtr MemPtrNewLarge(UInt32 size);
extern MemPtr MemPtrRealloc(MemPtr ptr, UInt32 newSize);
extern Char *StrDup(const Char *str);

#endif
