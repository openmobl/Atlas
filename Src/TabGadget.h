/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 *
 * Proprietary License. Redistrabution prohibited.
 */
#ifndef TabGadget_h
#define TabGadget_h

#include "config.h"
#include "Atlas.h"

typedef Int16 (TabGadgetSwitchHandler)(Int16 tabNo, void *dataP);
typedef Boolean (TabGadgetDrawTabHandler)(const RectanglePtr rect, Int16 tabNo, void *dataP);
typedef Boolean (TabGadgetTapHandler)(const RectanglePtr rect, Int16 x, Int16 y, Int16 tabNo, void *dataP);

#define kMaxTabs    16

typedef struct TabGadgetAttrType {
    Int16   leftMargin;
    Int16   rightMargin;
} TabGadgetAttrType;

typedef enum MarginType {
    marginLeft,
    marginRight
} MarginType;

typedef struct TabGadgetType {
    TabGadgetSwitchHandler  *switchHandlerP;
    TabGadgetDrawTabHandler *drawHandlerP;
    TabGadgetTapHandler     *tapHandlerP;
    
    FormType                *formP;
    Int16                   gadgetID;
    
    Int16                   noOfTabs;
    Int16                   currTab;
    
    Int16                   tabLabelsRes;
    
    TabGadgetAttrType       attrs;
    
    void                    *dataP;
} TabGadgetType;
typedef TabGadgetType *TabGadgetPtr;

extern Boolean TabGadgetHandler(struct FormGadgetTypeInCallback *gadgetP,
                                UInt16 cmd, void *paramP);

extern void TabGadgetInitGadget(FormType *formP, Int16 gadgetID, Int16 noOfTabs, Int16 tabLabelsRes);
extern void TabGadgetSetGadgetData(FormType *formP, Int16 gadgetID, void *dataP);
extern void TabGadgetSetGadgetMargin(FormType *formP, Int16 gadgetID, MarginType margin, Int16 size);

extern void TabGadgetOpenTab(FormType *formP, Int16 gadgetID);
extern void TabGadgetCloseTab(FormType *formP, Int16 gadgetID, Int16 tabNo);
extern void TabGadgetSetActiveTab(FormType *formP, Int16 gadgetID, Int16 tabNo);
extern Int16 TabGadgetGetActiveTab(FormType *formP, Int16 gadgetID);
extern Int16 TabGadgetGetTabCount(FormType *formP, Int16 gadgetID);

// Does not follow PalmOS convention... egh...
// Extended Gadget callback handles this
//extern void TabGadgetDestroyGadget(FormType *formP, Int16 gadgetID);

extern void TabGadgetSetSwitchHandler(FormType *formP, Int16 gadgetID, TabGadgetSwitchHandler *handlerP);
extern void TabGadgetSetDrawHandler(FormType *formP, Int16 gadgetID, TabGadgetDrawTabHandler *handlerP);
extern void TabGadgetSetTapHandler(FormType *formP, Int16 gadgetID, TabGadgetTapHandler *handlerP);

/*extern void TabGadgetHandleDrawGadget(FormType *formP, Int16 gadgetID);
extern void TabGadgetHandleTapGadget(FormType *formP, Int16 gadgetID);*/

#endif
