#pragma once

#include <gctypes.h>
#include "CGameCategories.hpp"
#include "CGameStatistics.h"
#include "newtitles.h"
#include "CGameSettings.h"
#include "GameTitles.h"
#include "GameList.h"

/// <summary>
/// Manages all data related to game titles
/// </summary>
class TitleLibrary
{
public:
	GameList Games;
	struct discHdr * GetDiscHeader(const char * gameID) const;

	CGameCategories Categories;
	CGameStatistics Statistics;
	CGameSettings Settings;
	CGameTitles DisplayNames;

	CNewTitles NewTitles;
	void CheckGame(const u8 *titleid);
	bool IsNew(const u8 *titleid) const;

protected:
	friend class Application;
	TitleLibrary();
	~TitleLibrary();

private:
};
