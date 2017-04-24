#pragma once

#include "settings/CGameCategories.hpp"
#include "settings/CGameStatistics.h"
#include "settings/CGameSettings.h"
#include "settings/GameTitles.h"

class TitleLibrary
{
public:
	CGameCategories GameCategories;
	CGameStatistics GameStatistics;
	CGameSettings GameSettings;
	CGameTitles GameTitles;

protected:
	friend class Application;
	TitleLibrary();
	~TitleLibrary();

private:

};
