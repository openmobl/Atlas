/*
 *  macros.h
 *  
 *
 *  Copyright (c) 2007, 2008, 2009, 2010 OpenMobl Systems.  All rights reserved.
 *  Proprietary license. Redistrabution prohibited.
 */
#ifndef macros_h
#define macros_h

#ifndef ABS
#define ABS(x) ( (x < 0) ? -(x) : (x) )
#endif

#ifndef MIN
#define MIN(x, y) ( (x < y) ? x : y )
#endif

#ifndef MAX
#define MAX(x, y) ( (x > y) ? x : y )
#endif

#endif
