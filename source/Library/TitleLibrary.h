#pragma once

#include "CGameCategories.hpp"
#include "CGameStatistics.h"
#include "newtitles.h"
#include "CGameSettings.h"
#include "GameTitles.h"

/// <summary>
/// Manages all data related to game titles
/// </summary>
class TitleLibrary
{
public:
	CGameCategories GameCategories;
	CGameStatistics GameStatistics;
	CGameSettings GameSettings;
	CGameTitles GameTitles;
	CNewTitles NewTitles;

protected:
	friend class Application;
	TitleLibrary();
	~TitleLibrary();

private:

};
