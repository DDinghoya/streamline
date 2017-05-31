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
#include "UninstallSM.hpp"
#include "IO/fileops.h"
#include "GameCube/GCGames.h"
#include "Channels/channels.h"
#include "App.h"
#include "prompts/PromptWindows.h"
#include "language/gettext.h"
#include "usbloader/wbfs.h"
#include "utils/wstring.hpp"

UninstallSM::UninstallSM(struct discHdr * header)
	: SettingsMenu(tr("Uninstall Menu"), &GuiOptions, MENU_NONE)
{
	DiscHeader = header;

	int Idx = 0;

	if (DiscHeader->type == TYPE_GAME_WII_IMG
		|| DiscHeader->type == TYPE_GAME_GC_IMG
		|| DiscHeader->type == TYPE_GAME_EMUNANDCHAN)
	{
		Options->SetName(Idx++, "%s", tr("Uninstall Game"));
	}
	Options->SetName(Idx++, "%s", tr("Reset Playcounter"));
	Options->SetName(Idx++, "%s", tr("Delete Cover Artwork"));
	Options->SetName(Idx++, "%s", tr("Delete Disc Artwork"));
	Options->SetName(Idx++, "%s", tr("Delete Cached Banner"));
	Options->SetName(Idx++, "%s", tr("Delete Cheat TXT"));
	Options->SetName(Idx++, "%s", tr("Delete Cheat GCT"));

	SetOptionValues();
}

void UninstallSM::SetOptionValues()
{
	int Idx = 0;

	if (DiscHeader->type == TYPE_GAME_WII_IMG
		|| DiscHeader->type == TYPE_GAME_GC_IMG
		|| DiscHeader->type == TYPE_GAME_EMUNANDCHAN)
	{
		//! Settings: Uninstall Game
		Options->SetValue(Idx++, " ");
	}

	//! Settings: Reset Playcounter
	Options->SetValue(Idx++, " ");

	//! Settings: Delete Cover Artwork
	Options->SetValue(Idx++, " ");

	//! Settings: Delete Disc Artwork
	Options->SetValue(Idx++, " ");

	//! Settings: Delete Cached Banner
	Options->SetValue(Idx++, " ");

	//! Settings: Delete Cheat TXT
	Options->SetValue(Idx++, " ");

	//! Settings: Delete Cheat GCT
	Options->SetValue(Idx++, " ");
}

int UninstallSM::GetMenuInternal()
{
	int ret = optionBrowser->GetClickedOption();

	if (ret < 0)
		return MENU_NONE;

	int Idx = -1;

	//! Settings: Uninstall Game
	if ((DiscHeader->type == TYPE_GAME_WII_IMG
		|| DiscHeader->type == TYPE_GAME_GC_IMG
		|| DiscHeader->type == TYPE_GAME_EMUNANDCHAN)
		&& ret == ++Idx)
	{
		int choice = WindowPrompt(App.Library.DisplayNames.GetTitle(DiscHeader), tr("What should be deleted for this game title:"), tr("Game Only"), tr("Uninstall all"), tr("Cancel"));
		if (choice == 0)
			return MENU_NONE;

		char GameID[7];
		snprintf(GameID, sizeof(GameID), "%s", (char *)DiscHeader->id);

		std::string Title = App.Library.DisplayNames.GetTitle(DiscHeader);
		App.Library.Settings.Remove(DiscHeader->id);
		App.Library.Settings.Save();
		App.Library.Statistics.Remove(DiscHeader->id);
		App.Library.Statistics.Save();

		int ret = 0;
		char filepath[512];

		if (DiscHeader->type == TYPE_GAME_WII_IMG)
		{
			ret = WBFS_RemoveGame((u8 *)GameID);
			if (ret >= 0)
			{
				wString oldFilter(App.Library.Games.GetCurrentFilter());
				App.Library.Games.ReadGameList();
				App.Library.Games.FilterList(oldFilter.c_str());
			}
		}
		else if (DiscHeader->type == TYPE_GAME_GC_IMG)
		{
			GCGames::Instance()->RemoveGame(GameID);
			// Reload list
			GCGames::Instance()->LoadAllGames();
		}
		else if (DiscHeader->type == TYPE_GAME_EMUNANDCHAN && DiscHeader->tid != 0)
		{
			// Remove ticket
			snprintf(filepath, sizeof(filepath), "%s/ticket/%08x/%08x.tik", App.Settings.NandEmuChanPath, (unsigned int)(DiscHeader->tid >> 32), (unsigned int)DiscHeader->tid);
			RemoveFile(filepath);

			// Remove contents / data
			snprintf(filepath, sizeof(filepath), "%s/title/%08x/%08x/", App.Settings.NandEmuChanPath, (unsigned int)(DiscHeader->tid >> 32), (unsigned int)DiscHeader->tid);
			RemoveDirectory(filepath);

			Channels::Instance()->GetEmuChannelList();
		}

		if (choice == 2)
		{
			snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.covers_path, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.covers2d_path, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.coversFull_path, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.disc_path, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%s.bnr", App.Settings.BNRCachePath, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%.4s.bnr", App.Settings.BNRCachePath, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%.3s.bnr", App.Settings.BNRCachePath, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%s.txt", App.Settings.TxtCheatcodespath, GameID);
			if (CheckFile(filepath)) remove(filepath);
			snprintf(filepath, sizeof(filepath), "%s%s.gct", App.Settings.Cheatcodespath, GameID);
			if (CheckFile(filepath)) remove(filepath);
		}

		if (ret < 0)
			WindowPrompt(tr("Can't delete:"), Title.c_str(), tr("OK"));
		else
			WindowPrompt(tr("Successfully deleted:"), Title.c_str(), tr("OK"));

		return MENU_DISCLIST;
	}

	//! Settings: Reset Playcounter
	else if (ret == ++Idx)
	{
		int result = WindowPrompt(tr("Are you sure?"), 0, tr("Yes"), tr("Cancel"));
		if (result == 1)
		{
			App.Library.Statistics.SetPlayCount(DiscHeader->id, 0);
			App.Library.Statistics.Save();
		}
	}

	//! Settings: Delete Cover Artwork
	else if (ret == ++Idx)
	{
		int choice = WindowPrompt(tr("Delete"), tr("Are you sure?"), tr("Yes"), tr("No"));
		if (choice != 1)
			return MENU_NONE;

		char GameID[7];
		snprintf(GameID, sizeof(GameID), "%s", (char *)DiscHeader->id);
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.covers_path, GameID);
		if (CheckFile(filepath)) remove(filepath);
		snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.covers2d_path, GameID);
		if (CheckFile(filepath)) remove(filepath);
		snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.coversFull_path, GameID);
		if (CheckFile(filepath)) remove(filepath);
	}

	//! Settings: Delete Disc Artwork
	else if (ret == ++Idx)
	{
		char GameID[7];
		snprintf(GameID, sizeof(GameID), "%s", (char *)DiscHeader->id);
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.png", App.Settings.disc_path, GameID);

		int choice = WindowPrompt(tr("Delete"), filepath, tr("Yes"), tr("No"));
		if (choice == 1)
			if (CheckFile(filepath)) remove(filepath);
	}

	//! Settings: Delete Cached Banner
	else if (ret == ++Idx)
	{
		int choice = WindowPrompt(tr("Delete"), tr("Are you sure?"), tr("Yes"), tr("No"));
		if (choice != 1)
			return MENU_NONE;

		char GameID[7];
		snprintf(GameID, sizeof(GameID), "%s", (char *)DiscHeader->id);
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.bnr", App.Settings.BNRCachePath, GameID);
		if (CheckFile(filepath)) remove(filepath);
		snprintf(filepath, sizeof(filepath), "%s%.4s.bnr", App.Settings.BNRCachePath, GameID);
		if (CheckFile(filepath)) remove(filepath);
		snprintf(filepath, sizeof(filepath), "%s%.3s.bnr", App.Settings.BNRCachePath, GameID);
		if (CheckFile(filepath)) remove(filepath);
	}

	//! Settings: Delete Cheat TXT
	else if (ret == ++Idx)
	{
		char GameID[7];
		snprintf(GameID, sizeof(GameID), "%s", (char *)DiscHeader->id);
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.txt", App.Settings.TxtCheatcodespath, GameID);

		int choice = WindowPrompt(tr("Delete"), filepath, tr("Yes"), tr("No"));
		if (choice == 1)
			if (CheckFile(filepath)) remove(filepath);
	}

	//! Settings: Delete Cheat GCT
	else if (ret == ++Idx)
	{
		char GameID[7];
		snprintf(GameID, sizeof(GameID), "%s", (char *)DiscHeader->id);
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.gct", App.Settings.Cheatcodespath, GameID);

		int choice = WindowPrompt(tr("Delete"), filepath, tr("Yes"), tr("No"));
		if (choice == 1)
			if (CheckFile(filepath)) remove(filepath);
	}

	SetOptionValues();

	return MENU_NONE;
}
