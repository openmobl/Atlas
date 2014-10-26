/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Find and replace: PrefsForm with <Name of Form>Form
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "PrefsForm.h"

#include "Atlas.h"
#include "Atlas_Rsc.h"
#include "TabGadget.h"
#include "Utils.h"

static void PrefsFormInit(FormType *frmP);
static Boolean PrefsFormDoCommand(UInt16 command);

static void PrefsFormInit(FormType *frmP)
{
    TabGadgetInitGadget(frmP, kPrefsTabGadget, 4, kStrListPrefTabTitles);
    TabGadgetSetGadgetMargin(frmP, kPrefsTabGadget, marginLeft, 5);
    TabGadgetSetGadgetMargin(frmP, kPrefsTabGadget, marginRight, 5);
    FrmDrawForm(frmP);
}

static Boolean PrefsFormDoCommand(UInt16 command)
{
    Boolean handled = false;

    switch (command) {
        // Menus
        // Control Objects
        case kPrefsOKButton:
            FrmReturnToForm(0);
            break;
        case kPrefsCancelButton:
            FrmReturnToForm(0);
            break;
        // Default
        default:
            break;
    }

    return handled;
}

Boolean PrefsFormHandleEvent(EventType *eventP)
{
    Boolean handled = false;
    FormType *frmP;

    switch (eventP->eType) {
        case menuEvent:
            return PrefsFormDoCommand(eventP->data.menu.itemID);
        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            PrefsFormInit(frmP);
            handled = true;
            break;
        case frmUpdateEvent:
            break;
        case ctlSelectEvent:
            handled = PrefsFormDoCommand(eventP->data.ctlSelect.controlID);
            break;
        default:
            break;
    }
    
    return handled;
}
