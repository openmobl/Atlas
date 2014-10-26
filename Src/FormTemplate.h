/*
 * Copyright (c) 2008, 2009, 2010 OpenMobl Systems.
 * All Rights Reserved.
 *
 * Find and replace: MainForm with <Name of Form>Form
 *
 * Proprietary License. Redistrabution prohibited.
 */
#ifndef MainForm_h
#define MainForm_h

extern void MainFormInit(FormType* frmP);
extern Boolean MainFormDoCommand(UInt16 command);
extern Boolean MainFormHandleEvent(EventType* eventP);

#endif
