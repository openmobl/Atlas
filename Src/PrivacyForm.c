/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "PrivacyForm.h"

#include "Atlas.h"
#include "Atlas_Rsc.h"
#include "Utils.h"


static void PrivacyFormInit(FormType *frmP);
static Boolean PrivacyFormDoCommand(UInt16 command);


static void PrivacyFormInit(FormType *frmP)
{

}

static Boolean PrivacyFormDoCommand(UInt16 command)
{
    Boolean handled = false;
    GlobalsPtr gP = GetGlobals();

    switch (command) {
        // Menus
        // Control Objects
        case kPrivacyClearAllButton:
            if (CtlGetValue(GetObjectPtr(kPrivacyClearCacheCheckBox))) {
                // Clear the cache
            }
            if (CtlGetValue(GetObjectPtr(kPrivacyClearCacheHistoryBox))) {
                // Clear the history
            }
            if (CtlGetValue(GetObjectPtr(kPrivacyClearCacheCookiesBox))) {
                // Clear the cookies
            }
            break;
        case kPrivacyCancelButton:
            // Reset private browsing state
            
            if (CtlGetValue(GetObjectPtr(kPrivacyEnableCheckBox))) {
                // Enable private browsing
            } else {
                // Disable private browsing
            }
            
            FrmReturnToForm(0);
            handled = true;
            break;
        // Default
        default:
            break;
    }

    return handled;
}

Boolean PrivacyFormHandleEvent(EventType *eventP)
{
    Boolean handled = false;
    FormType *frmP;

    switch (eventP->eType) {
        case menuEvent:
            handled = PrivacyFormDoCommand(eventP->data.menu.itemID);
            break;
        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            PrivacyFormInit(frmP);
            handled = true;
            break;
        case frmUpdateEvent:
            break;
        case ctlSelectEvent:
            handled = PrivacyFormDoCommand(eventP->data.ctlSelect.controlID);
            break;
        default:
            break;
    }
    
    return handled;
}
