/****************************************************************************
 * USB Loader GX Team
 *
 * libwiigui Template
 * by Tantric 2009
 *
 * menu.cpp
 * Menu flow routines - handles all menu logic
 ***************************************************************************/
#include <unistd.h>
#include <locale.h>

#include "GUI/gui.h"
#include "HBC/BootHomebrew.h"
#include "HBC/HomebrewBrowser.hpp"
#include "prompts/ProgressWindow.h"
#include "menu/GameBrowseMenu.hpp"
#include "SystemMenu/SystemMenuResources.h"
#include "menu/menus.h"
#include "mload/mload.h"
#include "mload/mload_modules.h"
#include "Net/networkops.h"
#include "patches/patchcode.h"
#include "settings/menus/GlobalSettings.hpp"
#include "themes/CTheme.h"
#include "themes/ThemeMenu.h"
#include "usbloader/disc.h"
#include "usbloader/GameList.h"
#include "usbloader/MountGamePartition.h"
#include "mload/mload_modules.h"
#include "video.h"
#include "audio.h"
#include "Input/input.h"
#include "menu.h"
#include "sys.h"
#include "Input/wpad.h"
#include "App.h"
#include "usbloader/frag.h"
#include "usbloader/wbfs.h"
#include "wad/nandtitle.h"

/*** Variables that are also used extern ***/
WiiPointer * pointer[4] = { NULL, NULL, NULL, NULL };
GuiImage * bgImg = NULL;
GuiImageData * background = NULL;
GuiBGM * bgMusic = NULL;
GuiSound *btnSoundClick = NULL;
GuiSound *btnSoundClick2 = NULL;
GuiSound *btnSoundOver = NULL;

static int currentMenu = 0;

static lwp_t guithread = LWP_THREAD_NULL;
static bool guiHalt = true;
static bool ExitRequested = false;

/****************************************************************************
 * ResumeGui
 *
 * Signals the GUI thread to start, and resumes the thread. This is called
 * after finishing the removal/insertion of new elements, and after initial
 * GUI setup.
 ***************************************************************************/
void ResumeGui()
{
	guiHalt = false;
	LWP_ResumeThread(guithread);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the GUI thread to stop, and waits for GUI thread to stop
 * This is necessary whenever removing/inserting new elements into the GUI.
 * This eliminates the possibility that the GUI is in the middle of accessing
 * an element that is being changed.
 ***************************************************************************/
void HaltGui()
{
	if (guiHalt) return;
	guiHalt = true;

	// wait for thread to finish
	while (!LWP_ThreadIsSuspended(guithread))
		usleep(100);
}

/****************************************************************************
 * UpdateGUI
 *
 * Primary thread to allow GUI to respond to state changes, and draws GUI
 ***************************************************************************/
static void * UpdateGUI(void *arg)
{
	u8 i;

	while (!ExitRequested)
	{
		if (guiHalt)
		{
			LWP_SuspendThread(guithread);
			continue;
		}

		Input::UpdatePads();

		App.MainWindow->Draw();
		if (App.Settings.tooltips && App.Theme.ShowTooltips && App.MainWindow->GetState() != STATE_DISABLED)
			App.MainWindow->DrawTooltip();

		// Pointer modifies wpad data struct for easy implementation of "virtual pointer" with PAD-Sticks
		// That is why it has to be called right before updating other gui elements with the triggers
		i = 4;
		while(i--)
			pointer[i]->Draw(Input::GetUserInput(i));

		for (i = 0; i < 4; i++)
			App.MainWindow->Update(Input::GetUserInput(i));

		Menu_Render();

		if (bgMusic) bgMusic->UpdateState();
	}

	for (i = 5; i < 255; i += 10)
	{
		App.MainWindow->Draw();
		Menu_DrawRectangle(0, 0, screenwidth, screenheight, (GXColor) {0, 0, 0, i}, 1);
		Menu_Render();
	}

	App.MainWindow->RemoveAll();
	Input::ShutoffRumble();

	return NULL;
}

/****************************************************************************
 * InitGUIThread
 *
 * Startup GUI threads
 ***************************************************************************/
void InitGUIThreads()
{
	ExitRequested = false;

	if(guithread == LWP_THREAD_NULL)
		LWP_CreateThread(&guithread, UpdateGUI, NULL, NULL, 65536, LWP_PRIO_HIGHEST);
}

void ExitGUIThreads()
{
	ExitRequested = true;

	if(guithread != LWP_THREAD_NULL)
	{
		ResumeGui();
		LWP_JoinThread(guithread, NULL);
		guithread = LWP_THREAD_NULL;
	}
}

static void MenuStartup()
{
	// Do not allow banner grid mode without AHBPROT
	// this function does nothing if it was already initiated before
	if (!SystemMenuResources::Instance()->IsLoaded() && !SystemMenuResources::Instance()->Init()
		&& App.Settings.gameDisplay == BANNERGRID_MODE)
	{
		App.Settings.gameDisplay = LIST_MODE;
		App.Settings.GameWindowMode = GAMEWINDOW_DISC;
	}

	gprintf("\tLoading font...%s\n", App.Theme.LoadFont(App.Settings.ConfigPath) ? "done" : "failed (using default)");
	gprintf("\tLoading theme...%s\n", App.Theme.Load(App.Settings.theme) ? "done" : "failed (using default)");

	//! Init the rest of the System
	Sys_Init();
	InitAudio();
	setlocale(LC_CTYPE, "C-UTF-8");
	setlocale(LC_MESSAGES, "C-UTF-8");
	AdjustOverscan(App.Settings.AdjustOverscanX, App.Settings.AdjustOverscanY);
}

/****************************************************************************
 * MainMenu
 ***************************************************************************/
int MainMenu(int menu)
{
	MenuStartup();

	currentMenu = menu;

	InitGUIThreads();

	InitProgressThread();
	InitNetworkThread();

	if (App.Settings.autonetwork)
		ResumeNetworkThread();

	btnSoundClick = new GuiSound(NULL, 0, App.Settings.sfxvolume);
	btnSoundClick->LoadSoundEffect(App.Resources.GetFile("button_click.wav"), App.Resources.GetFileSize("button_click.wav"));
	btnSoundClick2 = new GuiSound(NULL, 0, App.Settings.sfxvolume);
	btnSoundClick2->LoadSoundEffect(App.Resources.GetFile("button_click2.wav"), App.Resources.GetFileSize("button_click2.wav"));
	btnSoundOver = new GuiSound(NULL, 0, App.Settings.sfxvolume);
	btnSoundOver->LoadSoundEffect(App.Resources.GetFile("button_over.wav"), App.Resources.GetFileSize("button_over.wav"));

	pointer[0] = new WiiPointer("player1_point.png");
	pointer[1] = new WiiPointer("player2_point.png");
	pointer[2] = new WiiPointer("player3_point.png");
	pointer[3] = new WiiPointer("player4_point.png");

	App.MainWindow = new GuiWindow(screenwidth, screenheight);

	background = App.Resources.GetImageData(App.Settings.widescreen ? "wbackground.png" : "background.png");

	bgImg = new GuiImage(background);
	App.MainWindow->Append(bgImg);

	ResumeGui();

	bgMusic = new GuiBGM(App.Resources.GetFile("bg_music.ogg"), App.Resources.GetFileSize("bg_music.ogg"), App.Settings.volume);
	bgMusic->SetLoop(App.Settings.musicloopmode); //loop music
	bgMusic->Load(App.Settings.ogg_path);
	bgMusic->Play();

	MountGamePartition();

	while (currentMenu != MENU_EXIT)
	{
		bgMusic->SetVolume(App.Settings.volume);

		switch (currentMenu)
		{
			case MENU_SETTINGS:
				currentMenu = GlobalSettings::Execute();
				break;
			case MENU_THEMEMENU:
				currentMenu = ThemeMenu::Execute();
				break;
			case MENU_HOMEBREWBROWSE:
				currentMenu = HomebrewBrowser::Execute();
				break;
			case MENU_DISCLIST:
			default: // unrecognized menu
				currentMenu = GameBrowseMenu::Execute();
				break;
		}
	}

	//! THIS SHOULD NEVER HAPPEN ANYMORE
	ExitApp();

	return -1;
}
