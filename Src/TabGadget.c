/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 *
 * Proprietary License. Redistrabution prohibited.
 */
#include "config.h"
#include "TabGadget.h"

#include "Atlas.h"
#include "Atlas_Rsc.h"
#include "Utils.h"


#define max(a,b) (((a) > (b)) ? (a) : (b))


static TabGadgetPtr TabGadgetCreateGadget(FormType *formP, Int16 gadgetID);
static void TabGadgetDestroyGadget(FormType *formP, Int16 gadgetID);

static Boolean TabGadgetHandleDrawGadget(FormType *formP, Int16 gadgetID);
static Boolean TabGadgetHandleTapGadget(FormType *formP, Int16 gadgetID,
                                        Int16 x, Int16 y, Boolean penDown);

static void TabGadgetComputeTabBounds(FormType *formP, Int16 gadgetID, Int16 tabNo,
                                      RectanglePtr tabRectP);

Boolean TabGadgetHandler(struct FormGadgetTypeInCallback *gadgetP,
                                UInt16 cmd, void *paramP)
{
    Boolean handled = false;
    FormPtr formP = FrmGetActiveForm();
    
    switch (cmd) {
        case formGadgetDeleteCmd:
            //TabGadgetDestroyGadget(formP, gadgetP->id);
            handled = true;
            break;
        case formGadgetDrawCmd:
            if (!gadgetP->data) {
                gadgetP->data = TabGadgetCreateGadget(formP, gadgetP->id);
            }
            TabGadgetHandleDrawGadget(formP, gadgetP->id);
            gadgetP->attr.visible = true;
            handled = true;
            break;
        case formGadgetEraseCmd:
            break;
        case formGadgetHandleEventCmd:
            {
                EventType   *eventP = (EventType *)paramP;
                Boolean     isPenDown = false;
                Int16       x, y;
                
                switch (eventP->eType) {
                    case frmGadgetEnterEvent:
                        EvtGetPen(&x, &y, &isPenDown);
                        handled = TabGadgetHandleTapGadget(formP, gadgetP->id,
                                                           x, y, isPenDown);
                        break;
                    case penMoveEvent:
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }
    
    return handled;
}

static TabGadgetPtr TabGadgetCreateGadget(FormType *formP, Int16 gadgetID)
{
    TabGadgetPtr    tabs = NULL;
    
    tabs = MemPtrNew(sizeof(TabGadgetType));
    ErrFatalDisplayIf(tabs == NULL, "Could not create tabs!"); // gruff...
    
    MemSet(tabs, sizeof(TabGadgetType), 0);
    
    return tabs;
}

void TabGadgetInitGadget(FormType *formP, Int16 gadgetID, Int16 noOfTabs, Int16 tabLabelsRes)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    ErrFatalDisplayIf(noOfTabs > kMaxTabs, "Number of tabs requested larger than 16");
    ErrFatalDisplayIf(noOfTabs < 1, "Number of tabs requested less than 1");

    gadgetP->noOfTabs       = noOfTabs;
    gadgetP->currTab        = 0;
    gadgetP->tabLabelsRes   = tabLabelsRes;
}


void TabGadgetSetGadgetData(FormType *formP, Int16 gadgetID, void *dataP)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    gadgetP->dataP = dataP;
    
}

void TabGadgetSetSwitchHandler(FormType *formP, Int16 gadgetID, TabGadgetSwitchHandler *handlerP)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    gadgetP->switchHandlerP = handlerP;
    
}

void TabGadgetSetDrawHandler(FormType *formP, Int16 gadgetID, TabGadgetDrawTabHandler *handlerP)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");

    gadgetP->drawHandlerP = handlerP;
    
}

void TabGadgetSetTapHandler(FormType *formP, Int16 gadgetID, TabGadgetTapHandler *handlerP)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");

    gadgetP->tapHandlerP = handlerP;
    
}

void TabGadgetSetGadgetMargin(FormType *formP, Int16 gadgetID, MarginType margin, Int16 size)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");

    switch (margin) {
        case marginLeft:
            gadgetP->attrs.leftMargin = size;
            break;
        case marginRight:
            gadgetP->attrs.rightMargin = size;
            break;
        default:
            break;
    }
}

void TabGadgetOpenTab(FormType *formP, Int16 gadgetID)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    if ((gadgetP->noOfTabs + 1 > kMaxTabs)) { //, "Number of tabs requested larger than 16");
        return; // Ignore for now...
    }
    
    gadgetP->noOfTabs++;
    //FrmDrawForm(formP);
    FrmUpdateForm(FrmGetFormId(formP), frmRedrawUpdateCode);
}

void TabGadgetCloseTab(FormType *formP, Int16 gadgetID, Int16 tabNo)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    if ((gadgetP->noOfTabs - 1) < 1) { // Tried to close an empty tab set!
        return;
    }
    
    if (gadgetP->currTab == (gadgetP->noOfTabs - 1)) {
        gadgetP->currTab--;
    }
    
    gadgetP->noOfTabs--;
    //FrmDrawForm(formP);
    FrmUpdateForm(FrmGetFormId(formP), frmRedrawUpdateCode);
}

Int16 TabGadgetGetTabCount(FormType *formP, Int16 gadgetID)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    return gadgetP->noOfTabs;
}

Int16 TabGadgetGetActiveTab(FormType *formP, Int16 gadgetID)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    return gadgetP->currTab;
}

void TabGadgetSetActiveTab(FormType *formP, Int16 gadgetID, Int16 tabNo)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    ErrFatalDisplayIf(tabNo >= gadgetP->noOfTabs, "Tab requested larger than number of tabs");
    
    gadgetP->currTab = tabNo;
    
    //FrmDrawForm(formP);
    FrmUpdateForm(FrmGetFormId(formP), frmRedrawUpdateCode);
}


static void TabGadgetDestroyGadget(FormType *formP, Int16 gadgetID)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");

    MemPtrFree(gadgetP);
}

static void TabGadgetComputeTabBounds(FormType *formP, Int16 gadgetID, Int16 tabNo,
                                      RectanglePtr tabRectP)
{
    TabGadgetPtr    gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    Int16           tabWidth = 0;
    
    RectangleType rect;
    
    FrmGetObjectBounds(formP, FrmGetObjectIndex(formP, gadgetID), &rect);
    
    MemMove(tabRectP, &rect, sizeof(RectangleType));
    
    // Handle margins
    tabRectP->topLeft.x += gadgetP->attrs.leftMargin;
    tabRectP->extent.x  -= gadgetP->attrs.leftMargin;
    tabRectP->extent.x  -= gadgetP->attrs.rightMargin;
    
    // Find the tab's width + borders
    tabWidth = tabRectP->extent.x  / gadgetP->noOfTabs;
    
    tabRectP->topLeft.x += tabNo * tabWidth;
    tabRectP->extent.x  = tabWidth;
    
    // Subtract borders
    tabRectP->extent.x  -= 2;
    tabRectP->topLeft.x++;
    tabRectP->extent.y  -= 2;
    tabRectP->topLeft.y++;
}

static Boolean TabGadgetHandleDrawGadget(FormType *formP, Int16 gadgetID)
{
    TabGadgetPtr        gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    Boolean             handled = false;
    Int16               i = 0;
    Int16               rightX = 0;
    RectangleType       gadgetRect;
    RGBColorType        lineRGB = { 0, 106, 106, 106 }; // Dark grey
    IndexedColorType    lineIndex = WinRGBToIndex(&lineRGB), origFore;
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    FrmGetObjectBounds(formP, FrmGetObjectIndex(formP, gadgetID), &gadgetRect);
    
    for (i = 0; i < gadgetP->noOfTabs; i++) {
        RectangleType       rect;
        RectangleType       oldClip;
        
        TabGadgetComputeTabBounds(formP, gadgetID, i, &rect);
        
        rightX = rect.topLeft.x + rect.extent.x + 1;
        
        WinGetClip(&oldClip);
        WinSetClip(&rect);
        
        // Draw tab
        if (gadgetP->drawHandlerP) {
            handled = gadgetP->drawHandlerP(&rect, i, gadgetP->dataP);
        }
        
        // Draw title
        if (!handled && gadgetP->tabLabelsRes) {
            Char    title[256];
            Int16   titleLen = 0;
            
            SysStringByIndex(gadgetP->tabLabelsRes, i, title, 255);
            titleLen = StrLen(title);
            
            if (titleLen) {
                RGBColorType        textRGB = { 0, 182, 182, 182 }; // light grey
                IndexedColorType    textIndex = WinRGBToIndex(&textRGB), origText = 0;
                WinDrawOperation    oldMode;
                
                if (i != gadgetP->currTab)
                    origText = WinSetTextColor(textIndex);
                oldMode = WinSetDrawMode(winOverlay);
                
                WinPaintChars(title, titleLen,
                             rect.topLeft.x +
                             max((rect.extent.x - FntCharsWidth(title, titleLen)) / 2, 0),
                             rect.topLeft.y +
                             max((rect.extent.y - FntCharHeight()) / 2, 0));
                             
                WinSetDrawMode(oldMode);
                if (i != gadgetP->currTab)
                    WinSetTextColor(origText);
            }
        }
        
        WinSetClip(&oldClip);
        
        origFore = WinSetForeColor(lineIndex);
        // Draw border
        if (i == gadgetP->currTab) {
            UInt16 coordSys = kCoordinatesStandard;
            
            WinDrawLine(rect.topLeft.x - 1, rect.topLeft.y,
                        rect.topLeft.x - 1, rect.topLeft.y + rect.extent.y);
            WinDrawLine(rect.topLeft.x, rect.topLeft.y - 1,
                        rect.topLeft.x + rect.extent.x - 1, rect.topLeft.y - 1);
            WinDrawLine(rect.topLeft.x + rect.extent.x, rect.topLeft.y,
                        rect.topLeft.x + rect.extent.x, rect.topLeft.y + rect.extent.y);
                        
            coordSys = WinSetCoordinateSystem(kCoordinatesDouble);
            WinDrawLine(rect.topLeft.x * 2 - 1, rect.topLeft.y * 2 - 1,
                        rect.topLeft.x * 2, rect.topLeft.y * 2);
            WinDrawLine((rect.topLeft.x + rect.extent.x) * 2, rect.topLeft.y * 2 - 1,
                        (rect.topLeft.x + rect.extent.x) * 2 - 1, rect.topLeft.y * 2);
            WinSetCoordinateSystem(coordSys);
        } else {
            WinDrawLine(rect.topLeft.x - 1, rect.topLeft.y + rect.extent.y,
                        rect.topLeft.x + rect.extent.x + 1, rect.topLeft.y + rect.extent.y);
        }
        WinSetForeColor(origFore);
    }
    
    origFore = WinSetForeColor(lineIndex);
    if (gadgetP->attrs.leftMargin > 0) {
        WinDrawLine(gadgetRect.topLeft.x,
                    gadgetRect.topLeft.y + gadgetRect.extent.y - 1,
                    gadgetRect.topLeft.x + gadgetP->attrs.leftMargin,
                    gadgetRect.topLeft.y + gadgetRect.extent.y - 1);
    }
    if (gadgetP->attrs.rightMargin > 0) {
        WinDrawLine(rightX,
                    gadgetRect.topLeft.y + gadgetRect.extent.y - 1,
                    gadgetRect.topLeft.x + gadgetRect.extent.x,
                    gadgetRect.topLeft.y + gadgetRect.extent.y - 1);
    }
    WinSetForeColor(origFore);
    
    return handled;
}

static Boolean TabGadgetHandleTapGadget(FormType *formP, Int16 gadgetID, Int16 x, Int16 y, Boolean penDown)
{
    TabGadgetPtr        gadgetP = FrmGetGadgetData(formP, FrmGetObjectIndex(formP, gadgetID));
    Boolean             handled = false;
    Int16               i = 0;
    RectangleType       gadgetRect;
    
    ErrFatalDisplayIf(gadgetP == NULL, "NULL gadget pointer");
    
    FrmGetObjectBounds(formP, FrmGetObjectIndex(formP, gadgetID), &gadgetRect);
    
    for (i = 0; i < gadgetP->noOfTabs; i++) {
        RectangleType       rect;
        
        TabGadgetComputeTabBounds(formP, gadgetID, i, &rect);
        
        if (RctPtInRectangle(x, y, &rect)) {
            // Draw tab
            if (gadgetP->tapHandlerP) {
                handled = gadgetP->tapHandlerP(&rect, x, y, i, gadgetP->dataP);
            }
            
            // Draw title
            if (!handled && (i != gadgetP->currTab)) {
                gadgetP->currTab = i;
                
                if (gadgetP->switchHandlerP) {
                    handled = gadgetP->switchHandlerP(i, gadgetP->dataP);
                }

                //FrmDrawForm(formP);
                FrmUpdateForm(FrmGetFormId(formP), frmRedrawUpdateCode);
                handled = true;
            }
            
            break;
        }
        
    }
    
    return handled;
}
