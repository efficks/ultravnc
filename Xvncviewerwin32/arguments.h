//  Copyright (C) 2002 Ultr@Vnc Team Members. All Rights Reserved.
//  Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.

XrmOptionDescRec cmdLineOptions[] = {
  {"-shared",        "*shareDesktop",       XrmoptionNoArg,  "True"},
  {"-noshared",      "*shareDesktop",       XrmoptionNoArg,  "False"},
  {"-viewonly",      "*viewOnly",           XrmoptionNoArg,  "True"},
  {"-fullscreen",    "*fullScreen",         XrmoptionNoArg,  "True"},
  {"-noraiseonbeep", "*raiseOnBeep",        XrmoptionNoArg,  "False"},
  {"-passwd",        "*passwordFile",       XrmoptionSepArg, 0},
  {"-encodings",     "*encodings",          XrmoptionSepArg, 0},
  {"-bgr233",        "*useBGR233",          XrmoptionNoArg,  "True"},
  {"-owncmap",       "*forceOwnCmap",       XrmoptionNoArg,  "True"},
  {"-truecolor",     "*forceTrueColour",    XrmoptionNoArg,  "True"},
  {"-truecolour",    "*forceTrueColour",    XrmoptionNoArg,  "True"},
  {"-depth",         "*requestedDepth",     XrmoptionSepArg, 0},
  {"-compresslevel", "*compressLevel",      XrmoptionSepArg, 0},
  {"-quality",       "*qualityLevel",       XrmoptionSepArg, 0},
  {"-nojpeg",        "*enableJPEG",         XrmoptionNoArg,  "False"},
  {"-nocursorshape", "*useRemoteCursor",    XrmoptionNoArg,  "False"},
};

int numCmdLineOptions = XtNumber(cmdLineOptions);

char *fallback_resources[] = {

  "Vncviewer.title: TightVNC: %s",

  "Vncviewer.translations:\
    <Enter>: SelectionToVNC()\\n\
    <Leave>: SelectionFromVNC()",

  "*form.background: black",

  "*viewport.allowHoriz: True",
  "*viewport.allowVert: True",
  "*viewport.useBottom: True",
  "*viewport.useRight: True",
  "*viewport*Scrollbar*thumb: None",

  "*desktop.baseTranslations:\
     <Key>F8: ShowPopup()\\n\
     <ButtonPress>: SendRFBEvent()\\n\
     <ButtonRelease>: SendRFBEvent()\\n\
     <Motion>: SendRFBEvent()\\n\
     <KeyPress>: SendRFBEvent()\\n\
     <KeyRelease>: SendRFBEvent()",

  "*serverDialog.dialog.label: VNC server:",
  "*serverDialog.dialog.value:",
  "*serverDialog.dialog.value.translations: #override\\n\
     <Key>Return: ServerDialogDone()",

  "*passwordDialog.dialog.label: Password:",
  "*passwordDialog.dialog.value:",
  "*passwordDialog.dialog.value.AsciiSink.echo: False",
  "*passwordDialog.dialog.value.translations: #override\\n\
     <Key>Return: PasswordDialogDone()",

  "*popup.title: TightVNC popup",
  "*popup*background: grey",
  "*popup*font: -*-helvetica-bold-r-*-*-16-*-*-*-*-*-*-*",
  "*popup.buttonForm.Command.borderWidth: 0",
  "*popup.buttonForm.Toggle.borderWidth: 0",

  "*popup.translations: #override <Message>WM_PROTOCOLS: HidePopup()",
  "*popup.buttonForm.translations: #override\\n\
     <KeyPress>: SendRFBEvent() HidePopup()",

  "*popupButtonCount: 7",

  "*popup*button1.label: Dismiss popup",
  "*popup*button1.translations: #override\\n\
     <Btn1Down>,<Btn1Up>: HidePopup()",

  "*popup*button2.label: Quit viewer",
  "*popup*button2.translations: #override\\n\
     <Btn1Down>,<Btn1Up>: Quit()",

  "*popup*button3.label: Full screen",
  "*popup*button3.type: toggle",
  "*popup*button3.translations: #override\\n\
     <Visible>: SetFullScreenState()\\n\
     <Btn1Down>,<Btn1Up>: toggle() ToggleFullScreen() HidePopup()",

  "*popup*button4.label: Clipboard: local -> remote",
  "*popup*button4.translations: #override\\n\
     <Btn1Down>,<Btn1Up>: SelectionToVNC(always) HidePopup()",

  "*popup*button5.label: Clipboard: local <- remote",
  "*popup*button5.translations: #override\\n\
     <Btn1Down>,<Btn1Up>: SelectionFromVNC(always) HidePopup()",

  "*popup*button6.label: Send ctrl-alt-del",
  "*popup*button6.translations: #override\\n\
     <Btn1Down>,<Btn1Up>: SendRFBEvent(keydown,Control_L)\
                          SendRFBEvent(keydown,Alt_L)\
                          SendRFBEvent(key,Delete)\
                          SendRFBEvent(keyup,Alt_L)\
                          SendRFBEvent(keyup,Control_L)\
                          HidePopup()",

  "*popup*button7.label: Send F8",
  "*popup*button7.translations: #override\\n\
     <Btn1Down>,<Btn1Up>: SendRFBEvent(key,F8) HidePopup()",

  NULL
};