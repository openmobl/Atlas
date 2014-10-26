/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "MainForm.h"

#include "Atlas.h"
#include "Atlas_Rsc.h"
#include "TabGadget.h"
#include "Utils.h"

static void MainFormUpdateURLTrigger(const Char *url);
static void MainFormKickOffPageLoad(const Char *url);
static void MainFormKickOffWebSearchRequest(const Char *search);
static void MainFormSearchCurrentPage(const Char *search);
static void MainFormDrawTitleBar(FormType *frmP);
static void MainFormInit(FormType *frmP);
static Boolean MainFormDoCommand(UInt16 command);
static Boolean MainFormUpdate(FormType *frmP, EventType *eventP);
static Boolean MainFormHandlePenEvent(FormType *frmP, EventType *eventP,
                                    Boolean down, Boolean moving);
static void MainFormClose(FormType *frmP);

static Int16 MainTabGadgetSwitchHandler(Int16 tabNo, void *dataP);
static Boolean MainTabGadgetDrawTabHandler(const RectanglePtr rect, Int16 tabNo, void *dataP);
static Boolean MainTabGadgetTapHandler(const RectanglePtr rect, Int16 x, Int16 y, Int16 tabNo, void *dataP);

static Int16 MainTabGadgetSwitchHandler(Int16 tabNo, void *dataP)
{
    return 0;
}

static Boolean MainTabGadgetDrawTabHandler(const RectanglePtr rect, Int16 tabNo, void *dataP)
{
    if (TabGadgetGetTabCount(FrmGetActiveForm(), kTabGadget) > 1) {
        MemHandle   resH = DmGetResource('abmp', kTabCloseBitmap);
        BitmapType  *bmp = (BitmapType *)MemHandleLock(resH);
        
        WinDrawBitmap(bmp, rect->topLeft.x + rect->extent.x - 9, rect->topLeft.y + 3);
        
        MemHandleUnlock(resH);
        
    }
    
    return true;
}

static Boolean MainTabGadgetTapHandler(const RectanglePtr rect, Int16 x, Int16 y, Int16 tabNo, void *dataP)
{
    if (TabGadgetGetTabCount(FrmGetActiveForm(), kTabGadget) > 1) {
        RectangleType   closeBtnRect;
        
        MemMove(&closeBtnRect, rect, sizeof(RectangleType));
        closeBtnRect.topLeft.x += rect->extent.x - 9;
        closeBtnRect.topLeft.y += 3;
        closeBtnRect.extent.x = 6;
        closeBtnRect.extent.y = 6;
        
        if (RctPtInRectangle(x, y, &closeBtnRect)) {
            TabGadgetCloseTab(FrmGetActiveForm(), kTabGadget, tabNo);
            return true;
        }
    }
    
    return false;
}

// For now, ":" won't seperate a scheme (such as with data:, mailto:, tel:)
// Since we want things like about:blank to show properly
#define kSchemeSeperator    "://"

static void MainFormUpdateURLTrigger(const Char *url)
{
    const Int16 maxBoxWidth = 92;
    const Int16 spaceWidth = FntCharWidth(' ');
    FormType    *frmP = FrmGetActiveForm();
    GlobalsPtr  gP = GetGlobals();
    
    if (gP && url && StrLen(url)) {
        Char    *offset = StrStr(url, kSchemeSeperator);
        Int16   urlLen = StrLen(url) - ((offset != NULL) ? StrLen(kSchemeSeperator) : 0);
        Int16   stringWidth = maxBoxWidth;
        Boolean fitWithinWidth;
        Char    *newLabel = NULL;
        
        // Strip the scheme from the URL, for display purposes
        if (offset) {
            url = offset + StrLen(kSchemeSeperator);
        }
        
        FntCharsInWidth(url, &stringWidth, &urlLen, &fitWithinWidth);
        if (fitWithinWidth && (stringWidth < maxBoxWidth)) {
            Int16   numExtraSpaces = (maxBoxWidth - stringWidth) / spaceWidth;
            
            newLabel = MemPtrNew(urlLen + numExtraSpaces + 1);
            if (newLabel) {
                Int16 i = 0;
                
                StrCopy(newLabel, url);
                for (i = 0; i < numExtraSpaces; i++) {
                    StrCat(newLabel, " ");
                }
            }
        } else {
            newLabel = MemPtrNew(urlLen + 1);
            if (newLabel) {
                StrNCopy(newLabel, url, urlLen);
                newLabel[urlLen] = '\0';
            }
            
            // TODO: What would be nice would be to have a call here to pad if
            //       the truncation made the string shorter than the desired
            //       width.
        }
        
        if (newLabel) {
            CtlSetLabel(FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, kURLBox)),
                        newLabel);
                        
            if (gP->urlBoxLabel) {
                MemPtrFree(gP->urlBoxLabel);
            }
            gP->urlBoxLabel = newLabel;
        }
   }
}

static void MainFormKickOffPageLoad(const Char *url)
{
    MainFormUpdateURLTrigger(url);
}

static void MainFormKickOffWebSearchRequest(const Char *search)
{
    GlobalsPtr  gP = GetGlobals();
    Char        *fullSearchURL = NULL;
    Char        *searchURL = NULL;
    Int32       searchProvider = kTextGoogleSearchProvider; // TODO: Will be a preference
    
    // 1. URL encode search term
    // 2. Place search term in new URL
    // 3. Invoke MainFormKickOffPageLoad
    if (search) {
        MemHandle resH = DmGetResource('tSTR', searchProvider);
        
        searchURL = (Char *)MemHandleLock(resH);
        
        fullSearchURL = MemPtrNewLarge(StrLen(searchURL) + StrLen(search) + 1);
        StrCopy(fullSearchURL, searchURL);
        StrCat(fullSearchURL, search);
        
        if (gP->navigationOpen) {
            MemPtrFree(gP->navigationOpen);
        }
        gP->navigationOpen = fullSearchURL;
        
        MemHandleUnlock(resH);
        
        MainFormKickOffPageLoad(gP->navigationOpen);
    }
}

static void MainFormSearchCurrentPage(const Char *search)
{

}

static void MainFormDrawTitleBar(FormType *frmP)
{
    // TODO: Save in a resource (App name)
    const Char          *title = "Web";
    Int16               titleLen = StrLen(title);
    Int16               titleX, titleY;
    RectangleType       titleBounds;
    const Int16         titleCornerRadius = 4;
    WinDrawOperation    oldMode;
    RGBColorType        rgb = { 0, 255, 255, 255 }; // White
    IndexedColorType    textIndex, origText = 0, titleIndex, origFore;
    
    // Setup the title box
    titleIndex = UIColorGetTableEntryIndex(UIFormFrame);
    origFore = WinSetForeColor(titleIndex);
    
    FrmGetObjectBounds(frmP, FrmGetObjectIndex(frmP, kTitleGadget), &titleBounds);
    WinDrawRectangle(&titleBounds, titleCornerRadius);

    WinSetForeColor(origFore);

    // Draw the text
    textIndex = WinRGBToIndex(&rgb);
    origText = WinSetTextColor(textIndex);

    oldMode = WinSetDrawMode(winOverlay);
    titleX = titleBounds.topLeft.x + (titleBounds.extent.x - FntCharsWidth(title, titleLen)) / 2;
    titleY = titleBounds.topLeft.y + (titleBounds.extent.y - FntCharHeight()) / 2;
    WinPaintChars(title, titleLen, titleX, titleY);
    WinSetDrawMode(oldMode);

    WinSetTextColor(origText);
}

static void MainFormInit(FormType *frmP)
{
    TabGadgetInitGadget(frmP, kTabGadget, 1, 0);
    TabGadgetSetGadgetMargin(frmP, kTabGadget, marginLeft, 32);
    TabGadgetSetGadgetMargin(frmP, kTabGadget, marginRight, 5);
    TabGadgetSetSwitchHandler(frmP, kTabGadget, MainTabGadgetSwitchHandler);
    TabGadgetSetDrawHandler(frmP, kTabGadget, MainTabGadgetDrawTabHandler);
    TabGadgetSetTapHandler(frmP, kTabGadget, MainTabGadgetTapHandler);
    FrmDrawForm(frmP);
    
    MainFormDrawTitleBar(frmP);
    
    // Set a "default" URL for now
    MainFormUpdateURLTrigger("about:blank");
}

static Boolean MainFormDoCommand(UInt16 command)
{
    FormType    *frmP = FrmGetActiveForm();
    Boolean     handled = false;

    switch (command) {
        // Menus
        case kMainMenuPagePrivBrowsOp:
            FrmPopupForm(kPrivacyForm);
            break;
        case kMainMenuGoOpenURL:
        case kOpenButton:
        case kURLBox:
            FrmPopupForm(kURLForm);
            break;
        case kMainMenuOptionsPrefs:
            FrmPopupForm(kPrefsForm);
            break;
        case kMenuOptionsAboutAtlas:
            break;
        // Control Objects
        // Default
        default:
            break;
    }

    return handled;
}

static Boolean MainFormUpdate(FormType *frmP, EventType *eventP)
{
    Boolean     handled = false;
    GlobalsPtr  gP = GetGlobals();
    
    switch (eventP->data.frmUpdate.updateCode) {
        case frmUpdateGotoUrl:
            MainFormKickOffPageLoad(gP->navigationOpen);
            handled = true;
            break;
        case frmUpdateGotoSearch:
            MainFormKickOffWebSearchRequest(gP->navigationOpen);
            handled = true;
            break;
        case frmUpdateMode:
            //MainFormDrawTitleBar(frmP);
            break;
        case frmUpdateNewTab:
            if (TabGadgetGetTabCount(frmP, kTabGadget) < kMaxTabCount) {
                TabGadgetOpenTab(frmP, kTabGadget);
                TabGadgetSetActiveTab(frmP, kTabGadget,
                                      TabGadgetGetActiveTab(frmP, kTabGadget) + 1);
                MainFormDrawTitleBar(frmP);
            }
            handled = true;
            break;
        case frmRedrawUpdateCode:
            FrmDrawForm(frmP);
            MainFormDrawTitleBar(frmP);
            handled = true;
            break;
        default:
            break;
    }
    
    return handled;
}

static Boolean MainFormHandlePenEvent(FormType *frmP, EventType *eventP,
                                      Boolean down, Boolean moving)
{
    Boolean         handled = false;
    RectangleType   titleBounds;
    static Boolean  penPointInTitle = false;
    
    FrmGetObjectBounds(frmP, FrmGetObjectIndex(frmP, kTitleGadget), &titleBounds);
    
    if (RctPtInRectangle(eventP->screenX, eventP->screenY, &titleBounds)) {
        if (down) {
            penPointInTitle = true;
        } else if (penPointInTitle && !moving) {
            EvtEnqueueKey(menuChr, 0, commandKeyMask);
            penPointInTitle = false;
        }
        handled = true;
    } else {
        penPointInTitle = false;
    }
    
    return handled;
}

static void MainFormClose(FormType *frmP)
{
    GlobalsPtr  gP = GetGlobals();
    
    if (gP && gP->urlBoxLabel) {
        MemPtrFree(gP->urlBoxLabel);
        gP->urlBoxLabel = NULL;
    }
}

Boolean MainFormHandleEvent(EventType *eventP)
{
    Boolean     handled = false;
    FormType    *frmP   = FrmGetActiveForm();

    switch (eventP->eType) {
        case menuEvent:
            return MainFormDoCommand(eventP->data.menu.itemID);
        case frmOpenEvent:
            FrmDrawForm(frmP);
            MainFormInit(frmP);
            handled = true;
            break;
        case frmCloseEvent:
            MainFormClose(frmP);
            break;
        case frmUpdateEvent:
            handled = MainFormUpdate(frmP, eventP);
            break;
        case ctlSelectEvent:
            handled = MainFormDoCommand(eventP->data.ctlSelect.controlID);
            break;
        case penDownEvent:
        case penMoveEvent:
        case penUpEvent:
            handled = MainFormHandlePenEvent(frmP, eventP, eventP->eType == penDownEvent,
                                                           eventP->eType == penMoveEvent);
            break;
        default:
            break;
    }
    
    return handled;
}
