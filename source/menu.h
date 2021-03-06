/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.h
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#include <ogcsys.h>
#include "GUI/gui.h"
#include "Controls/WiiPointer.h"
#include "App.h"

void InitGUIThreads(void);
void ExitGUIThreads(void);

int MainMenu(int menuitem);

enum
{
	MENU_EXIT = -1,
	MENU_NONE,
	MENU_SETTINGS,
	MENU_DISCLIST,
	MENU_GAME_SETTINGS,
	MENU_HOMEBREWBROWSE,
	BOOTHOMEBREW,
	MENU_THEMEMENU,
};

void ResumeGui();
void HaltGui();

extern WiiPointer *pointer[4];
extern GuiImageData *background;
extern GuiImage *bgImg;
extern GuiText *GameRegionTxt;
extern GuiText *GameIDTxt;
extern GuiImageData *cover;
extern GuiImage *coverImg;

#endif
