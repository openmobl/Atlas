/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Find and replace: MainForm with <Name of Form>Form
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "MainForm.h"

#include "Atlas.h"
#include "Atlas_Rsc.h"
#include "Utils.h"


void MainFormInit(FormType *frmP)
{

}

Boolean MainFormDoCommand(UInt16 command)
{
    Boolean handled = false;

    switch (command) {
        // Menus
        // Control Objects
        // Default
        default:
            break;
    }

    return handled;
}

Boolean MainFormHandleEvent(EventType *eventP)
{
    Boolean handled = false;
    FormType *frmP;

    switch (eventP->eType) {
        case menuEvent:
            return MainFormDoCommand(eventP->data.menu.itemID);
        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            MainFormInit(frmP);
            handled = true;
            break;
        case frmUpdateEvent:
            break;
        case ctlSelectEvent:
            break;
        default:
            break;
    }
    
    return handled;
}
