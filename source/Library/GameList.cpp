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
#include <algorithm>
#include <string>
#include <malloc.h>
#include "GUI/gui_searchbar.h"
#include "usbloader/wbfs.h"
#include "GameCube/GCGames.h"
#include "App.h"
#include "FreeTypeGX.h"
#include "GameList.h"
#include "memory/memory.h"
#include "Channels/channels.h"
#include "prompts/ProgressWindow.h"
#include "language/gettext.h"

enum { DISABLED, ENABLED, HIDEFORBIDDEN };

void GameList::clear()
{
	GameFilter.clear();
	FullGameList.clear();
	GamePartitionList.clear();
	FilteredList.clear();
	//! Clear memory of the vector completely
	std::vector<struct discHdr *>().swap(FilteredList);
	std::vector<struct discHdr>().swap(FullGameList);
	std::vector<int>().swap(GamePartitionList);
}

struct discHdr * GameList::GetDiscHeader(const char * gameID) const
{
	if (!gameID) return NULL;

	for (u32 i = 0; i < FilteredList.size(); ++i)
	{
		if (strncasecmp(gameID, (const char *)FilteredList[i]->id, 6) == 0)
			return FilteredList[i];
	}

	return NULL;
}

int GameList::GetPartitionNumber(const u8 *gameID) const
{
	if (!gameID) return -1;

	for (u32 i = 0; i < FullGameList.size(); ++i)
	{
		if (strncasecmp((const char *)gameID, (const char *)FullGameList[i].id, 6) == 0)
			return GamePartitionList[i];
	}

	return -1;
}

void GameList::RemovePartition(int part)
{
	for (u32 i = 0; i < GamePartitionList.size(); ++i)
	{
		if (GamePartitionList[i] == part)
		{
			FullGameList.erase(FullGameList.begin() + i);
			GamePartitionList.erase(GamePartitionList.begin() + i);
			--i;
		}
	}

	if (FullGameList.size() > 0)
		FilterList();
}

int GameList::InternalReadList(int part)
{
	// Retrieve all stuff from WBFS
	u32 cnt = 0;

	int ret = WBFS_GetCount(part, &cnt);
	if (ret < 0) return -1;

	// We are done here if no games are there
	if (cnt == 0)
		return 0;

	/* Buffer length */
	u32 len = sizeof(struct discHdr) * cnt;

	/* Allocate memory */
	struct discHdr *buffer = (struct discHdr *) allocate_memory(len);
	if (!buffer) return -1;

	/* Clear buffer */
	memset(buffer, 0, len);

	/* Get header list */
	ret = WBFS_GetHeaders(part, buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
	{
		free(buffer);
		return -1;
	}

	u32 oldSize = FullGameList.size();
	std::vector<struct discHdr> PartGameList(cnt);
	memcpy(&PartGameList[0], buffer, len);
	free(buffer);

	for (u32 i = 0; i < PartGameList.size(); i++)
	{
		for (u32 j = 0; j < FullGameList.size(); j++)
		{
			if (strncasecmp((const char *)PartGameList[i].id, (const char *)FullGameList[j].id, 6) == 0)
			{
				PartGameList.erase(PartGameList.begin() + i);
				--i;
				break;
			}
		}
	}
	FullGameList.resize(oldSize + PartGameList.size());
	memcpy(&FullGameList[oldSize], &PartGameList[0], PartGameList.size() * sizeof(struct discHdr));

	GamePartitionList.resize(oldSize + PartGameList.size());

	for (u32 i = oldSize; i < GamePartitionList.size(); ++i)
		GamePartitionList[i] = part;

	return PartGameList.size();
}

int GameList::ReadGameList()
{
	// Clear list
	FullGameList.clear();
	GamePartitionList.clear();
	//! Clear memory of the vector completely
	std::vector<struct discHdr>().swap(FullGameList);
	std::vector<int>().swap(GamePartitionList);

	int cnt = 0;

	if (!App.Settings.MultiplePartitions)
	{
		cnt = InternalReadList(App.Settings.partition);
	}
	else
	{
		int partitions = DeviceHandler::GetUSBPartitionCount();

		for (int part = 0; part < partitions; ++part)
		{
			int ret = InternalReadList(part);
			if (ret > 0) cnt += ret;
		}
	}

	return cnt;
}

void GameList::InternalFilterList(std::vector<struct discHdr> &FullList)
{
	for (u32 i = 0; i < FullList.size(); ++i)
	{
		struct discHdr *header = &FullList[i];

		/* Register game */
		App.Library.CheckGame(header->id);

		/* Filters */
		if (App.Settings.GameSort & SORT_FAVORITE)
		{
			GameStatus * GameStats = App.Library.Statistics.GetGameStatus(header->id);
			if (App.Settings.marknewtitles)
			{
				bool isNew = App.Library.IsNew(header->id);
				if (!isNew && (!GameStats || GameStats->FavoriteRank == 0)) continue;
			}
			else
			{
				if (!GameStats || GameStats->FavoriteRank == 0) continue;
			}
		}

		//ignore uLoader cfg "iso".  i was told it is "__CFG_"  but not confirmed
		if (strncasecmp((char*)header->id, "__CFG_", 6) == 0)
			continue;

		GameCFG * GameConfig = App.Library.Settings.GetGameCFG(header);

		/* Rating based parental control method */
		if (App.Settings.parentalcontrol != PARENTAL_LVL_ADULT && !App.Settings.godmode)
		{
			if (GameConfig && GameConfig->parentalcontrol > App.Settings.parentalcontrol)
				continue;

			// Check game rating in GameTDB, since the default Wii parental control setting is enabled
			int rating = App.Library.DisplayNames.GetParentalRating((char *)header->id);
			if (rating > App.Settings.parentalcontrol)
				continue;
		}

		//! Per game lock method
		if (!App.Settings.godmode && GameConfig && GameConfig->Locked)
			continue;

		//! Category filter
		u32 n;
		int allType = DISABLED;
		// verify the display mode for category "All"
		for (n = 0; n < App.Settings.EnabledCategories.size(); ++n)
		{
			if (App.Settings.EnabledCategories[n] == 0)
			{
				allType = ENABLED; // All = Enabled
				break;
			}
		}
		for (n = 0; n < App.Settings.ForbiddenCategories.size(); ++n)
		{
			if (App.Settings.ForbiddenCategories[n] == 0)
			{
				allType = HIDEFORBIDDEN; // All = Enabled but hide Forbidden categories
				break;
			}
		}

		if (allType == DISABLED)
		{
			// Remove TitleID if it contains a forbidden categories
			for (n = 0; n < App.Settings.ForbiddenCategories.size(); ++n)
			{
				if (App.Library.Categories.isInCategory((char *)header->id, App.Settings.ForbiddenCategories[n]))
					break;
			}
			if (n < App.Settings.ForbiddenCategories.size())
				continue;

			// Remove TitleID is it doesn't contain a required categories
			for (n = 0; n < App.Settings.RequiredCategories.size(); ++n)
			{
				if (!App.Library.Categories.isInCategory((char *)header->id, App.Settings.RequiredCategories[n]))
					break;
			}
			if (n < App.Settings.RequiredCategories.size())
				continue;

			// If there's no required categories, verify if the TitleID should be kept or removed
			if (App.Settings.RequiredCategories.size() == 0)
			{
				for (n = 0; n < App.Settings.EnabledCategories.size(); ++n)
				{
					if (App.Library.Categories.isInCategory((char *)header->id, App.Settings.EnabledCategories[n]))
						break;
				}
				if (n == App.Settings.EnabledCategories.size())
					continue;
			}
		}

		if (allType == HIDEFORBIDDEN)
		{
			// Remove TitleID if it contains a forbidden categories
			for (n = 0; n < App.Settings.ForbiddenCategories.size(); ++n)
			{
				if (App.Library.Categories.isInCategory((char *)header->id, App.Settings.ForbiddenCategories[n]))
					if (App.Settings.ForbiddenCategories[n] > 0)
						break;
			}
			if (n < App.Settings.ForbiddenCategories.size())
				continue;
		}

		FilteredList.push_back(header);
	}
}

int GameList::FilterList(const wchar_t * gameFilter)
{
	if ((App.Settings.LoaderMode & MODE_WIIGAMES) && (FullGameList.size() == 0))
		ReadGameList();

	if (gameFilter)
		GameFilter.assign(gameFilter);

	FilteredList.clear();

	// Filter current game list if selected
	if (App.Settings.LoaderMode & MODE_WIIGAMES)
		InternalFilterList(FullGameList);

	// Filter gc game list if selected
	if (App.Settings.LoaderMode & MODE_GCGAMES)
		InternalFilterList(GCGames::Instance()->GetHeaders());

	// Filter nand channel list if selected
	if (App.Settings.LoaderMode & MODE_NANDCHANNELS)
		InternalFilterList(Channels::Instance()->GetNandHeaders());

	// Filter emu nand channel list if selected
	if (App.Settings.LoaderMode & MODE_EMUCHANNELS)
		InternalFilterList(Channels::Instance()->GetEmuHeaders());

	App.Library.NewTitles.Save();
	GuiSearchBar::FilterList(FilteredList, GameFilter);

	SortList();

	return FilteredList.size();
}

void GameList::InternalLoadUnfiltered(std::vector<struct discHdr> &FullList)
{
	for (u32 i = 0; i < FullList.size(); ++i)
	{
		struct discHdr *header = &FullList[i];

		/* Register game */
		App.Library.CheckGame(header->id);

		FilteredList.push_back(header);
	}
}

int GameList::LoadUnfiltered()
{
	if ((App.Settings.LoaderMode & MODE_WIIGAMES) && (FullGameList.size() == 0))
		ReadGameList();

	GameFilter.clear();
	FilteredList.clear();

	// Filter current game list if selected
	if (App.Settings.LoaderMode & MODE_WIIGAMES)
		InternalLoadUnfiltered(FullGameList);

	// Filter gc game list if selected
	if (App.Settings.LoaderMode & MODE_GCGAMES)
		InternalLoadUnfiltered(GCGames::Instance()->GetHeaders());

	// Filter nand channel list if selected
	if (App.Settings.LoaderMode & MODE_NANDCHANNELS)
		InternalLoadUnfiltered(Channels::Instance()->GetNandHeaders());

	// Filter emu nand channel list if selected
	if (App.Settings.LoaderMode & MODE_EMUCHANNELS)
		InternalLoadUnfiltered(Channels::Instance()->GetEmuHeaders());

	App.Library.NewTitles.Save();
	GuiSearchBar::FilterList(FilteredList, GameFilter);

	SortList();

	return FilteredList.size();
}

void GameList::SortList()
{
	if (FilteredList.size() < 2) return;

	if (App.Settings.GameSort & SORT_PLAYCOUNT)
	{
		std::sort(FilteredList.begin(), FilteredList.end(), PlaycountSortCallback);
	}
	else if (App.Settings.GameSort & SORT_RANKING)
	{
		std::sort(FilteredList.begin(), FilteredList.end(), RankingSortCallback);
	}
	else if (App.Settings.GameSort & SORT_PLAYERS)
	{
		std::sort(FilteredList.begin(), FilteredList.end(), PlayersSortCallback);
	}
	else
	{
		std::sort(FilteredList.begin(), FilteredList.end(), NameSortCallback);
	}
}

bool GameList::NameSortCallback(const struct discHdr *a, const struct discHdr *b)
{
	return (strcasecmp(App.Library.DisplayNames.GetTitle((struct discHdr *) a), App.Library.DisplayNames.GetTitle((struct discHdr *) b)) < 0);
}

bool GameList::PlaycountSortCallback(const struct discHdr *a, const struct discHdr *b)
{
	int count1 = App.Library.Statistics.GetPlayCount(a->id);
	int count2 = App.Library.Statistics.GetPlayCount(b->id);

	if (count1 == count2) return NameSortCallback(a, b);

	return (count1 > count2);
}

bool GameList::RankingSortCallback(const struct discHdr *a, const struct discHdr *b)
{
	int fav1 = App.Library.Statistics.GetFavoriteRank(a->id);
	int fav2 = App.Library.Statistics.GetFavoriteRank(b->id);

	if (fav1 == fav2) return NameSortCallback(a, b);

	return (fav1 > fav2);
}

bool GameList::PlayersSortCallback(const struct discHdr *a, const struct discHdr *b)
{
	int count1 = App.Library.DisplayNames.GetPlayersCount((const char *)a->id);
	int count2 = App.Library.DisplayNames.GetPlayersCount((const char *)b->id);

	if (count1 == count2) return NameSortCallback(a, b);

	return (count1 > count2);
}
