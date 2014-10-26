/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#ifndef Atlas_h
#define Atlas_h

#include "Atlas_Rsc.h"

// Sent in FrmUpdateEvent
typedef enum FrmUpdateEnum
{
    frmUpdateGotoUrl = 0x09,
    frmUpdateGotoSearch,
    frmUpdateMode,
    frmUpdateNewTab,
    frmUpdateWebPageFind
} FrmUpdateEnum;

// Launch modes
typedef enum BrowserStartEnum
{
    startBlank,
    startHome,
    startBook,
    startLastURL
} BrowserStartEnum;

// Feature numbers
typedef enum AtlasFeaturesEnum
{
    ftrGlobals = 1000
} AtlasFeaturesEnum;

// Preferences
typedef struct AtlasPreferenceType
{
    Char    lastURL[255]; // TODO: This should be changed!
} AtlasPreferenceType;

typedef struct GlobalsType
{
    Char    *urlBoxLabel;
    Char    *navigationOpen;
} GlobalsType;

typedef GlobalsType *GlobalsPtr;

extern GlobalsPtr GetGlobals(void);

// TODO: Below needs to be moved to AtalsAPI.h

// Custom launch codes
#define sysAppLaunchCmdGotoHomepage     (sysAppLaunchCmdCustomBase | 0x01)
#define sysAppLaunchCmdGotoBlank        (sysAppLaunchCmdCustomBase | 0x02)
#define sysAppLaunchCmdViewBookmarks    (sysAppLaunchCmdCustomBase | 0x03)

#endif
