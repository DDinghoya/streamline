/****************************************************************************
 * Copyright (C) 2012 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <unistd.h>
#include "BannerSettingsMenu.hpp"
#include "App.h"
#include "settings/SettingsPrompts.h"
#include "prompts/PromptWindows.h"
#include "language/gettext.h"

static const char * AnimStartText[] =
{
	trNOOP( "During zoom" ),
	trNOOP( "After zoom" ),
};

static const char * OnOffText[] =
{
	trNOOP( "OFF" ),
	trNOOP( "ON" )
};

BannerSettingsMenu::BannerSettingsMenu()
	: SettingsMenu(tr("Banner Animation Settings"), &GuiOptions, MENU_NONE)
{
	int Idx = 0;
	Options->SetName(Idx++, "%s", tr( "Cache BNR Files" ));
	Options->SetName(Idx++, "%s", tr( "Animation Start" ));
	Options->SetName(Idx++, "%s", tr( "Zoom Duration (Speed)" ));
	Options->SetName(Idx++, "%s", tr( "Grid Scroll Speed" ));
	Options->SetName(Idx++, "%s", tr( "Frame Projection X-Offset" ));
	Options->SetName(Idx++, "%s", tr( "Frame Projection Y-Offset" ));
	Options->SetName(Idx++, "%s", tr( "Frame Projection Width" ));
	Options->SetName(Idx++, "%s", tr( "Frame Projection Height" ));
	Options->SetName(Idx++, "%s", tr( "GC Banner Scale" ));

	SetOptionValues();
}

void BannerSettingsMenu::SetOptionValues()
{
	int Idx = 0;

	//! Settings: Cache BNR Files
	Options->SetValue(Idx++, "%s", tr(OnOffText[App.Settings.CacheBNRFiles]));

	//! Settings: Animation Start
	Options->SetValue(Idx++, "%s", tr(AnimStartText[App.Settings.BannerAnimStart]));

	//! Settings: Zoom Duration (Speed)
	Options->SetValue(Idx++, "%i %s", App.Settings.BannerZoomDuration, tr("Frames"));

	//! Settings: Grid Scroll Speed
	Options->SetValue(Idx++, "%g %s/%s", App.Settings.BannerGridSpeed, tr("Pixels"), tr("Frame"));

	//! Settings: Frame Projection X-Offset
	Options->SetValue(Idx++, "%g %s", App.Settings.BannerProjectionOffsetX, tr("Pixels"));

	//! Settings: Frame Projection Y-Offset
	Options->SetValue(Idx++, "%g %s", App.Settings.BannerProjectionOffsetY, tr("Pixels"));

	//! Settings: Frame Projection Width
	Options->SetValue(Idx++, "%g %s", App.Settings.BannerProjectionWidth, tr("Pixels"));

	//! Settings: Frame Projection Height
	Options->SetValue(Idx++, "%g %s", App.Settings.BannerProjectionHeight, tr("Pixels"));

	//! Settings: GC Banner Scale
	Options->SetValue(Idx++, "%g", App.Settings.GCBannerScale);
}

int BannerSettingsMenu::GetMenuInternal()
{
	int ret = optionBrowser->GetClickedOption();

	if (ret < 0)
		return MENU_NONE;

	int Idx = -1;

	//! Settings: Cache BNR Files
	if (ret == ++Idx)
	{
		if (++App.Settings.CacheBNRFiles >= MAX_ON_OFF) App.Settings.CacheBNRFiles = 0;
	}

	//! Settings: Animation Start
	else if (ret == ++Idx)
	{
		if (++App.Settings.BannerAnimStart > 1) App.Settings.BannerAnimStart = 0;
	}

	//! Settings: Zoom Duration (Speed)
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%i", App.Settings.BannerZoomDuration);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.BannerZoomDuration = atoi(entrie);
	}

	//! Settings: Grid Scroll Speed
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.BannerGridSpeed);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.BannerGridSpeed = atof(entrie);
	}

	//! Settings: Frame Projection X-Offset
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.BannerProjectionOffsetX);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.BannerProjectionOffsetX = atof(entrie);
	}

	//! Settings: Frame Projection Y-Offset
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.BannerProjectionOffsetY);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.BannerProjectionOffsetY = atof(entrie);
	}

	//! Settings: Frame Projection Width
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.BannerProjectionWidth);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.BannerProjectionWidth = atof(entrie);
	}

	//! Settings: Frame Projection Height
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.BannerProjectionHeight);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.BannerProjectionHeight = atof(entrie);
	}

	//! Settings: GC Banner Scale
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.GCBannerScale);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.GCBannerScale = atof(entrie);
	}

	SetOptionValues();

	return MENU_NONE;
}
