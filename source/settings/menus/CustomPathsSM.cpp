/****************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#include <unistd.h>
#include <sys/param.h>
#include "Channels/channels.h"
#include "Controls/DeviceHandler.hpp"
#include "GameCube/GCGames.h"
#include "CustomPathsSM.hpp"
#include "settings/SettingsPrompts.h"
#include "App.h"
#include "settings/SettingsEnums.h"
#include "prompts/PromptWindows.h"
#include "prompts/ProgressWindow.h"
#include "language/gettext.h"
#include "prompts/filebrowser.h"
#include "themes/CTheme.h"
#include "IO/fileops.h"

CustomPathsSM::CustomPathsSM()
	: SettingsMenu(tr("Custom Paths"), &GuiOptions, MENU_NONE)
{
	int Idx = 0;
	Options->SetName(Idx++, tr("3D Cover Path"));
	Options->SetName(Idx++, tr("2D Cover Path"));
	Options->SetName(Idx++, tr("Full Cover Path"));
	Options->SetName(Idx++, tr("Disc Artwork Path"));
	Options->SetName(Idx++, tr("Theme Path"));
	Options->SetName(Idx++, tr("Titles Path"));
	Options->SetName(Idx++, tr("Update Path"));
	Options->SetName(Idx++, tr("GCT Cheatcodes Path"));
	Options->SetName(Idx++, tr("TXT Cheatcodes Path"));
	Options->SetName(Idx++, tr("DOL Path"));
	Options->SetName(Idx++, tr("Homebrew Apps Path"));
	Options->SetName(Idx++, tr("BCA Codes Path"));
	Options->SetName(Idx++, tr("WIP Patches Path"));
	Options->SetName(Idx++, tr("Languagefiles Path"));
	Options->SetName(Idx++, tr("WDM Files Path"));
	Options->SetName(Idx++, tr("Nand Emu Path"));
	Options->SetName(Idx++, tr("Nand Emu Channel Path"));
	Options->SetName(Idx++, tr("Main GameCube Path"));
	Options->SetName(Idx++, tr("SD GameCube Path"));
	Options->SetName(Idx++, tr("Devolution Loader Path"));
	Options->SetName(Idx++, tr("Nintendont Loader Path"));
	Options->SetName(Idx++, tr("Cache BNR Files Path"));

	SetOptionValues();
}

void CustomPathsSM::SetOptionValues()
{
	int Idx = 0;

	//! Settings: 3D Cover Path
	Options->SetValue(Idx++, App.Settings.covers_path);

	//! Settings: 2D Cover Path
	Options->SetValue(Idx++, App.Settings.covers2d_path);

	//! Settings: Full Cover Path
	Options->SetValue(Idx++, App.Settings.coversFull_path);

	//! Settings: Disc Artwork Path
	Options->SetValue(Idx++, App.Settings.disc_path);

	//! Settings: Theme Path
	Options->SetValue(Idx++, App.Settings.theme_path);

	//! Settings: Titles Path
	Options->SetValue(Idx++, App.Settings.titlestxt_path);

	//! Settings: Update Path
	Options->SetValue(Idx++, App.Settings.update_path);

	//! Settings: GCT Cheatcodes Path
	Options->SetValue(Idx++, App.Settings.Cheatcodespath);

	//! Settings: TXT Cheatcodes Path
	Options->SetValue(Idx++, App.Settings.TxtCheatcodespath);

	//! Settings: DOL Path
	Options->SetValue(Idx++, App.Settings.dolpath);

	//! Settings: Homebrew Apps Path
	Options->SetValue(Idx++, App.Settings.homebrewapps_path);

	//! Settings: BCA Codes Path
	Options->SetValue(Idx++, App.Settings.BcaCodepath);

	//! Settings: WIP Patches Path
	Options->SetValue(Idx++, App.Settings.WipCodepath);

	//! Settings: Languagefiles Path
	Options->SetValue(Idx++, App.Settings.languagefiles_path);

	//! Settings: WDM Files Path
	Options->SetValue(Idx++, App.Settings.WDMpath);

	//! Settings: Nand Emu Path
	Options->SetValue(Idx++, App.Settings.NandEmuPath);

	//! Settings: Nand Emu Channel Path
	Options->SetValue(Idx++, App.Settings.NandEmuChanPath);

	//! Settings: GameCube Games Path
	Options->SetValue(Idx++, App.Settings.GameCubePath);

	//! Settings: SD GameCube Games Path
	Options->SetValue(Idx++, App.Settings.GameCubeSDPath);

	//! Settings: GameCube Devolution loader.bin Path
	Options->SetValue(Idx++, App.Settings.DEVOLoaderPath);

	//! Settings: GameCube Nintendont boot.dol Path
	Options->SetValue(Idx++, App.Settings.NINLoaderPath);

	//! Settings: Cache BNR Files Path
	Options->SetValue(Idx++, App.Settings.BNRCachePath);
}

int CustomPathsSM::GetMenuInternal()
{
	int ret = optionBrowser->GetClickedOption();

	if (ret < 0)
		return MENU_NONE;

	int Idx = -1;

	//! Settings: 3D Cover Path
	if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "3D Cover Path" ));
		ChangePath(App.Settings.covers_path, sizeof(App.Settings.covers_path));
	}

	//! Settings: 2D Cover Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "2D Cover Path" ));
		ChangePath(App.Settings.covers2d_path, sizeof(App.Settings.covers2d_path));
	}

	//! Settings: Full Cover Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Full Cover Path" ));
		ChangePath(App.Settings.coversFull_path, sizeof(App.Settings.coversFull_path));
	}

	//! Settings: Disc Artwork Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Disc Artwork Path" ));
		ChangePath(App.Settings.disc_path, sizeof(App.Settings.disc_path));
	}

	//! Settings: Theme Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Theme Path" ));
		ChangePath(App.Settings.theme_path, sizeof(App.Settings.theme_path));
	}

	//! Settings: Titles Path
	else if (ret == ++Idx)
	{
		char oldPath[100];
		strncpy(oldPath, App.Settings.titlestxt_path, sizeof(App.Settings.titlestxt_path));

		titleTxt->SetText(tr("Titles Path"));
		if (ChangePath(App.Settings.titlestxt_path, sizeof(App.Settings.titlestxt_path)))
		{
			if(strlen(oldPath) != strlen(App.Settings.titlestxt_path) || strcmp(oldPath, App.Settings.titlestxt_path) != 0)
			{
				if (WindowPrompt(tr("Move File"), tr("Do you want to move the file(s)? Any existing ones will be deleted!"), tr("Yes"), tr("Cancel")) == 1)
				{
					MoveDbFile(oldPath, App.Settings.titlestxt_path, "wiitdb.xml");
					MoveDbFile(oldPath, App.Settings.titlestxt_path, "TitlesCache.bin");
					MoveDbFile(oldPath, App.Settings.titlestxt_path, "wiitdb_offsets.bin");
					MoveDbFile(oldPath, App.Settings.titlestxt_path, "GameTimestamps.txt");
				
					WindowPrompt(tr("Process finished."), 0, tr("OK"));
				}
			}
		}
	}

	//! Settings: Update Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Update Path" ));
		ChangePath(App.Settings.update_path, sizeof(App.Settings.update_path));
	}

	//! Settings: GCT Cheatcodes Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "GCT Cheatcodes Path" ));
		ChangePath(App.Settings.Cheatcodespath, sizeof(App.Settings.Cheatcodespath));
	}

	//! Settings: TXT Cheatcodes Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "TXT Cheatcodes Path" ));
		ChangePath(App.Settings.TxtCheatcodespath, sizeof(App.Settings.TxtCheatcodespath));
	}

	//! Settings: DOL Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "DOL Path" ));
		ChangePath(App.Settings.dolpath, sizeof(App.Settings.dolpath));
	}

	//! Settings: Homebrew Apps Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Homebrew Apps Path" ));
		ChangePath(App.Settings.homebrewapps_path, sizeof(App.Settings.homebrewapps_path));
	}

	//! Settings: BCA Codes Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "BCA Codes Path" ));
		ChangePath(App.Settings.BcaCodepath, sizeof(App.Settings.BcaCodepath));
	}

	//! Settings: WIP Patches Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "WIP Patches Path" ));
		ChangePath(App.Settings.WipCodepath, sizeof(App.Settings.WipCodepath));
	}

	//! Settings: Languagefiles Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Languagefiles Path" ));
		ChangePath(App.Settings.languagefiles_path, sizeof(App.Settings.languagefiles_path));
	}

	//! Settings: WDM Files Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "WDM Files Path" ));
		ChangePath(App.Settings.WDMpath, sizeof(App.Settings.WDMpath));
	}

	//! Settings: Nand Emu Path
	else if (ret == ++Idx)
	{
		char oldPath[sizeof(App.Settings.NandEmuPath)];
		snprintf(oldPath, sizeof(oldPath), App.Settings.NandEmuPath);

		titleTxt->SetText(tr( "Nand Emu Path" ));
		ChangePath(App.Settings.NandEmuPath, sizeof(App.Settings.NandEmuPath));
		if(strncasecmp(DeviceHandler::PathToFSName(App.Settings.NandEmuPath), "FAT", 3) != 0)
		{
			snprintf(App.Settings.NandEmuPath, sizeof(App.Settings.NandEmuPath), oldPath);
			WindowPrompt(tr("Error:"), tr("Nand Emulation only works on FAT/FAT32 partitions!"), tr("OK"));
		}
	}

	//! Settings: Nand Emu Channel Path
	else if (ret == ++Idx)
	{
		char oldPath[sizeof(App.Settings.NandEmuChanPath)];
		snprintf(oldPath, sizeof(oldPath), App.Settings.NandEmuChanPath);

		titleTxt->SetText(tr( "Nand Emu Channel Path" ));
		int result = ChangePath(App.Settings.NandEmuChanPath, sizeof(App.Settings.NandEmuChanPath));
		if(strncasecmp(DeviceHandler::PathToFSName(App.Settings.NandEmuChanPath), "FAT", 3) != 0)
		{
			snprintf(App.Settings.NandEmuChanPath, sizeof(App.Settings.NandEmuChanPath), oldPath);
			WindowPrompt(tr("Error:"), tr("Nand Emulation only works on FAT/FAT32 partitions!"), tr("OK"));
		}
		else if(result == 1)
		{
			Channels::Instance()->GetEmuChannelList();
			App.Library.GameTitles.LoadTitlesFromGameTDB(App.Settings.titlestxt_path, false);
		}
	}

	//! Settings: GameCube Games Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Main GameCube Games Path" ));
		if(ChangePath(App.Settings.GameCubePath, sizeof(App.Settings.GameCubePath)))
		{
			GCGames::Instance()->LoadAllGames();
		}
	}

	//! Settings: SD GameCube Games Path
	else if (ret == ++Idx)
	{
		char tmp_path[sizeof(App.Settings.GameCubeSDPath)];
		snprintf(tmp_path, sizeof(tmp_path), "%s", App.Settings.GameCubeSDPath);

		titleTxt->SetText(tr( "SD GameCube Games Path" ));
		if(ChangePath(tmp_path, sizeof(tmp_path)))
		{
			if(strncmp(tmp_path, "sd", 2) != 0)
			{
				WindowPrompt(tr("Error:"), tr("This path must be on SD!"), tr("OK"));
			}
			else
			{
				snprintf(App.Settings.GameCubeSDPath, sizeof(App.Settings.GameCubeSDPath), "%s", tmp_path);
				GCGames::Instance()->LoadAllGames();
			}
		}
	}

	//! Settings: GameCube Devolution loader.bin path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Devolution Loader Path" ));
		ChangePath(App.Settings.DEVOLoaderPath, sizeof(App.Settings.DEVOLoaderPath));
	}

	//! Settings: GameCube Nintendont boot.dol path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Nintendont Loader Path" ));
		ChangePath(App.Settings.NINLoaderPath, sizeof(App.Settings.NINLoaderPath));
	}

	//! Settings: Cache BNR Files Path
	else if (ret == ++Idx)
	{
		titleTxt->SetText(tr( "Cache BNR Files Path" ));
		ChangePath(App.Settings.BNRCachePath, sizeof(App.Settings.BNRCachePath));
	}

	//! Global set back of the titleTxt after a change
	titleTxt->SetText(tr( "Custom Paths" ));
	SetOptionValues();

	return MENU_NONE;
}

int CustomPathsSM::ChangePath(char * SettingsPath, int SizeOfPath)
{
	char entered[300];
	snprintf(entered, sizeof(entered), SettingsPath);

	HaltGui();
	GuiWindow * parent = (GuiWindow *) parentElement;
	if(parent) parent->SetState(STATE_DISABLED);
	this->SetState(STATE_DEFAULT);
	this->Remove(optionBrowser);
	ResumeGui();

	int result = BrowseDevice(entered, sizeof(entered), FB_DEFAULT, noFILES);

	if(parent) parent->SetState(STATE_DEFAULT);
	this->Append(optionBrowser);

	if (result == 1)
	{
		if (entered[strlen(entered)-1] != '/')
			strcat(entered, "/");

		snprintf(SettingsPath, SizeOfPath, entered);
		WindowPrompt(tr( "Path Changed" ), 0, tr( "OK" ));
	}

	return result;
}

void CustomPathsSM::MoveDbFile(const char* oldPath, const char* newPath, const char* fileName)
{
	char srcPath[300], destPath[300];
	memset(srcPath, 0, 300);
	memset(destPath, 0, 300);

	snprintf(srcPath, sizeof(srcPath), "%s/%s", oldPath, fileName);
	snprintf(destPath, sizeof(destPath), "%s/%s", newPath, fileName);

	if (CheckFile(srcPath))
	{
		if (CheckFile(destPath))
			RemoveFile(destPath);

		MoveFile(srcPath, destPath);
	}
}
