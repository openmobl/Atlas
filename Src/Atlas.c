/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "Atlas.h"

#include "MainForm.h"
#include "PrefsForm.h"
#include "PrivacyForm.h"
#include "TabGadget.h"
#include "URLForm.h"

static void DisplayInitDepth(Boolean start);
static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags);
static Boolean InitGlobals(void);
static void DestroyGlobals(void);
static Err AppStart(UInt16 cmd, void *cmdPBP);
static Boolean AppHandleEvent(EventType *eventP);
static void AppEventLoop(void);
static void AppStop(void);



static void DisplayInitDepth(Boolean start) // set start to false on AppStop, true on AppStart
{
	Err 		error;
	UInt32 		depthP;

	if (start) {   // AppStart
		error = WinScreenMode(winScreenModeGetSupportedDepths, NULL, NULL, &depthP, NULL);
		if ((error == errNone) && (depthP & 0x8000)) {
			depthP = 16;
			WinScreenMode(winScreenModeSet, NULL, NULL, &depthP, NULL);
		}
	} else {        // AppStop
		WinScreenMode(winScreenModeSetToDefaults, NULL, NULL, NULL, NULL);
	}

	return;
}

/*
 * FUNCTION: RomVersionCompatible
 *
 * DESCRIPTION: 
 *
 * This routine checks that a ROM version is meet your minimum 
 * requirement.
 *
 * PARAMETERS:
 *
 * requiredVersion
 *     minimum rom version required
 *     (see sysFtrNumROMVersion in SystemMgr.h for format)
 *
 * launchFlags
 *     flags that indicate if the application UI is initialized
 *     These flags are one of the parameters to your app's PilotMain
 *
 * RETURNED:
 *     error code or zero if ROM version is compatible
 */

static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{
    UInt32 romVersion;

    /* See if we're on in minimum required version of the ROM or later. */
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    if (romVersion < requiredVersion) {
        if ((launchFlags & 
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
            FrmAlert(kRomIncompatibleAlert);

            /* Palm OS versions before 2.0 will continuously relaunch this
             * app unless we switch to another safe one. */
            /* DCK: We will never be able to run on PalmOS < 2.0, or < 5.0 for
                    that matter... */
            /*if (romVersion < kPalmOS20Version) {
                AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
            }*/
        }

        return sysErrRomIncompatible;
    }

    return errNone;
}

static Boolean InitGlobals(void)
{
    GlobalsPtr  gP  = NULL;
    
    gP = (GlobalsPtr)MemPtrNew(sizeof(GlobalsType));
    if (!gP) {
        return false;
    }
    MemSet(gP, sizeof(GlobalsType), 0);
    FtrSet(kAppFileCreator, ftrGlobals, (UInt32)gP);
    
    return true;
}

GlobalsPtr GetGlobals(void)
{
    GlobalsPtr  gP  = NULL;
    
    FtrGet(kAppFileCreator, ftrGlobals, (UInt32 *)&gP);
    
    return gP;
}

static void DestroyGlobals(void)
{
    GlobalsPtr  gP = GetGlobals();
    
    if (!gP) {
        return;
    }
    
    if (gP->urlBoxLabel) {
        MemPtrFree(gP->urlBoxLabel);
    }
    if (gP->navigationOpen) {
        MemPtrFree(gP->navigationOpen);
    }
    
    FtrUnregister(kAppFileCreator, ftrGlobals);
    MemPtrFree(gP);
}

/*
 * FUNCTION: AppStart
 *
 * DESCRIPTION:  Get the current application's preferences.
 *
 * RETURNED:
 *     errNone - if nothing went wrong
 */

static Err AppStart(UInt16 cmd, void *cmdPBP)
{
    DisplayInitDepth(true);
    
    InitGlobals();
    
    FrmGotoForm(kMainForm);
    return errNone;
}

/*
 * FUNCTION: AppHandleEvent
 *
 * DESCRIPTION: 
 *
 * This routine loads form resources and set the event handler for
 * the form loaded.
 *
 * PARAMETERS:
 *
 * event
 *     a pointer to an EventType structure
 *
 * RETURNED:
 *     true if the event was handled and should not be passed
 *     to a higher level handler.
 */

static Boolean AppHandleEvent(EventType *eventP)
{
    UInt16 formId;
    FormType *frmP;

    if (eventP->eType == frmLoadEvent) {
        /* Load the form resource. */
        formId = eventP->data.frmLoad.formID;
        frmP = FrmInitForm(formId);
        FrmSetActiveForm(frmP);

        /* 
         * Set the event handler for the form.  The handler of the
         * currently active form is called by FrmHandleEvent each
         * time is receives an event. 
         */
        switch (formId) {
            case kMainForm:
                FrmSetEventHandler(frmP, MainFormHandleEvent);
                FrmSetGadgetHandler(frmP, FrmGetObjectIndex(frmP, kTabGadget), TabGadgetHandler);
                break;
            case kURLForm:
                FrmSetEventHandler(frmP, URLFormHandleEvent);
                break;
            case kPrivacyForm:
                FrmSetEventHandler(frmP, PrivacyFormHandleEvent);
                break;
            case kPrefsForm:
                FrmSetEventHandler(frmP, PrefsFormHandleEvent);
                FrmSetGadgetHandler(frmP, FrmGetObjectIndex(frmP, kPrefsTabGadget), TabGadgetHandler);
                break;
        }
        return true;
    }

    return false;
}

/*
 * FUNCTION: AppEventLoop
 *
 * DESCRIPTION: This routine is the event loop for the application.
 */

static void AppEventLoop(void)
{
    UInt16 error;
    EventType event;

    do {
        /* change timeout if you need periodic nilEvents */
        EvtGetEvent(&event, SysTicksPerSecond() / 5);

        if (!SysHandleEvent(&event)) {
            if (!MenuHandleEvent(0, &event, &error)) {
                if (!AppHandleEvent(&event)) {
                    FrmDispatchEvent(&event);
                }
            }
        }
    } while (event.eType != appStopEvent);
}

/*
 * FUNCTION: AppStop
 *
 * DESCRIPTION: Save the current state of the application.
 */

static void AppStop(void)
{
        
    /* Close all the open forms. */
    FrmCloseAllForms();
    
    DestroyGlobals();
    
    DisplayInitDepth(false);
}

/*
 * FUNCTION: PilotMain
 *
 * DESCRIPTION: This is the main entry point for the application.
 * 
 * PARAMETERS:
 *
 * cmd
 *     word value specifying the launch code. 
 *
 * cmdPB
 *     pointer to a structure that is associated with the launch code
 *
 * launchFlags
 *     word value providing extra information about the launch.
 *
 * RETURNED:
 *     Result of launch, errNone if all went OK
 */

UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    Err error;

    error = RomVersionCompatible(kOurMinVersion, launchFlags);
    if (error) return (error);

    switch (cmd) {
		  case sysAppLaunchCmdGoToURL:
		  case sysAppLaunchCmdGotoHomepage:
        case sysAppLaunchCmdGotoBlank:
        case sysAppLaunchCmdNormalLaunch:
            error = AppStart(cmd, cmdPBP);
            if (error) 
                return error;
            AppEventLoop();
            AppStop();
            break;
    }

    return errNone;
}
