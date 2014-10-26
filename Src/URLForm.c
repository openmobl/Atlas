/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "URLForm.h"

#include "Atlas.h"
#include "Atlas_Rsc.h"
#include "Utils.h"

static void URLFormInit(FormType *frmP);
static FrmUpdateEnum URLFormDetermineRequestType(const Char *navText);
static Boolean URLFormDoCommand(UInt16 command);


static void URLFormInit(FormType *frmP)
{
    GlobalsPtr gP = GetGlobals();
    
    if (gP->navigationOpen) {
        FieldPtr urlFldP = GetObjectPtr(kURLField);
        
        // TODO: Get current page's URL instead!
        FldInsert(urlFldP, gP->navigationOpen, StrLen(gP->navigationOpen));
        FldSetSelection(urlFldP, 0, StrLen(gP->navigationOpen));
    }
    FrmSetFocus(frmP, FrmGetObjectIndex(frmP, kURLField));
}

static FrmUpdateEnum URLFormDetermineRequestType(const Char *navText)
{
    FrmUpdateEnum    commandType = frmUpdateGotoUrl;
   
   // TODO: Maybe something better...
    if (StrStr(navText, ":") || StrStr(navText, "www.") ||
        StrStr(navText, ".com") || StrStr(navText, ".org") ||
        StrStr(navText, ".ed") || StrStr(navText, ".co")) {
        commandType = frmUpdateGotoUrl;
    } else {
        commandType = frmUpdateGotoSearch;
    }
   
    return commandType;
}

static Boolean URLFormDoCommand(UInt16 command)
{
    GlobalsPtr  gP = GetGlobals();
    Boolean     handled = false;

    if (!gP)
        return false;

    switch (command) {
        // Menus
        // Control Objects
        case kURLOpenButton:
            {
                Char    *navigationText = FldGetTextPtr(GetObjectPtr(kURLField));
                Boolean openInNewTab = false;
                
                if (navigationText && StrLen(navigationText)) {
                    if (gP->navigationOpen) {
                        MemPtrFree(gP->navigationOpen);
                    }
                    gP->navigationOpen = StrDup(navigationText);
                    
                    if (CtlGetValue(GetObjectPtr(kURLOpenInNewTabCheckbox))) {
                        openInNewTab = true;
                    }
                    
                    FrmReturnToForm(0);
                    if (openInNewTab) {
                        FrmUpdateForm(kMainForm, frmUpdateNewTab);
                    }
                    FrmUpdateForm(kMainForm, URLFormDetermineRequestType(navigationText));
                }
            }
            break;
        case kURLCancelButton:
            FrmReturnToForm(0);
            break;
        // Handle inserting text using the insert buttons, or list.
        case kWWWPushButton:
        case kCOMPushButton:
        case kORGPushButton:
        case kNETPushButton:
        case kSlashPushButton:
        case kDotPushButton:
        case kHTMLPushButton:
            {
                FieldPtr    urlFldP         = GetObjectPtr(kURLField);
                const Char  *buttonLabelP   = CtlGetLabel(GetObjectPtr(command));
                
                FldInsert(urlFldP, buttonLabelP, StrLen(buttonLabelP));
                CtlSetValue(GetObjectPtr(command), 0);
                
                handled = true;
            }
            break;
        case kMoreTrigger:
            {
                ListPtr     moreListP   = GetObjectPtr(kMoreList);
                FieldPtr    urlFldP     = GetObjectPtr(kURLField);
                Int16       selection   = LstPopupList(moreListP);
                
                if (selection >= 0) {
                    const Char  *selectionP = LstGetSelectionText(moreListP, selection);
                
                    FldInsert(urlFldP, selectionP, StrLen(selectionP));
                }
                handled = true;
            }
            break;
        // Default
        default:
            break;
    }

    return handled;
}

Boolean URLFormHandleEvent(EventType *eventP)
{
    Boolean     handled = false;
    FormType    *frmP = FrmGetActiveForm();

    switch (eventP->eType) {
        case menuEvent:
            handled = URLFormDoCommand(eventP->data.menu.itemID);
            break;
        case frmOpenEvent:
            FrmDrawForm(frmP);
            URLFormInit(frmP);
            handled = true;
            break;
        case frmUpdateEvent:
            break;
        case ctlSelectEvent:
            handled = URLFormDoCommand(eventP->data.ctlSelect.controlID);
            break;
        default:
            break;
    }
    
    return handled;
}
