/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Proprietary License. Redistrabution prohibited.
 */
#ifndef config_h
#define config_h

#include <PalmOS.h>
//#include <PalmOSGlue.h>

#define kAppFileCreator            'Atla'
#define kAppName                    "Atlas"
#define kAppVersionNum              0x01
#define kAppPrefID                  0x00
#define kAppPrefVersionNum          0x01

#define kOurMinVersion      sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0)
#define kPalmOS20Version    sysMakeROMVersion(2,0,0,sysROMStageDevelopment,0)

// Browser core defaults
#define kAboutBlankPage     "about:blank"
#define kAboutAtlasPage     "about:atlas"
#define kUserAgentString    "Mozilla/5.0 (PalmOS; U; en-uS) AppleWebKit/%s (KHTML, like Gecko) Atlas/0.1 Universe/4.0 (OpenMobl) Safari/%s"
#define kUserAgentProfile   "http://openmobl.mobi/profiles/atlas1.xml"
#define kMaxTabCount        3

#endif
