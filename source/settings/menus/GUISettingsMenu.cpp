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
#include "GUISettingsMenu.hpp"
#include "Controls/DeviceHandler.hpp"
#include "App.h"
#include "prompts/PromptWindows.h"
#include "language/gettext.h"
#include "settings/SettingsPrompts.h"
#include "SystemMenu/SystemMenuResources.h"
#include "system/IosLoader.h"
#include "usbloader/wbfs.h"
#include "themes/CTheme.h"
#include "utils/tools.h"
#include "video.h"
#include "input.h"

static const char * OnOffText[MAX_ON_OFF] =
{
	trNOOP( "OFF" ),
	trNOOP( "ON" )
};

static const char * GameInfoText[GAMEINFO_MAX] =
{
	trNOOP( "Game ID" ),
	trNOOP( "Game Region" ),
	trNOOP( "Both" ),
	trNOOP( "Neither" )
};

static const char * FlipXText[XFLIP_MAX][3] =
{
	{ trNOOP( "Right" ), "/", trNOOP( "Next" ) },
	{ trNOOP( "Left" ), "/", trNOOP( "Prev" ) },
	{ trNOOP( "Like SysMenu" ), "", "" },
	{ trNOOP( "Right" ), "/", trNOOP( "Prev" ) },
	{ trNOOP( "DiskFlip" ), "", "" }
};

static const char * PromptButtonsText[MAX_ON_OFF] =
{
	trNOOP( "Normal" ),
	trNOOP( "Widescreen Fix" ),
};

static const char * GameWindowText[3] =
{
	trNOOP( "Banner Animation" ),
	trNOOP( "Rotating Disc" ),
	trNOOP( "Banner On Channels" ),
};

static const char * KeyboardText[KEYBOARD_MAX] =
{
	"QWERTY",
	"DVORAK",
	"QWERTZ",
	"AZERTY",
	"QWERTY 2"
};

static const char * DiscArtDownloadText[DISCARTS_MAX_CHOICE] =
{
	trNOOP( "Original/Customs" ),
	trNOOP( "Customs/Original" ),
	trNOOP( "Original" ),
	trNOOP( "Customs" )
};

static const char * CoversFullDownloadText[COVERSFULL_MAX_CHOICE] =
{
	trNOOP( "High Quality" ),
	trNOOP( "Low Quality" ),
	trNOOP( "High/Low" ),
	trNOOP( "Low/High" )
};

static const char *ScreensaverText[SCREENSAVER_MAX] =
{
	trNOOP( "OFF" ),
	trNOOP( "3 min" ),
	trNOOP( "5 min" ),
	trNOOP( "10 min" ),
	trNOOP( "20 min" ),
	trNOOP( "30 min" ),
	trNOOP( "1 hour" )
};

static const char * BannerFavIconText[BANNER_FAVICON_MAX_CHOICE] =
{
	trNOOP( "OFF" ),
	trNOOP( "Round" ),
	trNOOP( "/\\/\\" ),
	trNOOP( "Two Lines" ),
	trNOOP( "One Line A" ),
	trNOOP( "One Line B" )
};

static const char * HomeMenuText[HOME_MENU_MAX_CHOICE] =
{
	trNOOP( "System Default" ),
	trNOOP( "Full Menu" ),
	trNOOP( "Default" )
};

GuiSettingsMenu::GuiSettingsMenu()
	: SettingsMenu(tr("GUI Settings"), &GuiOptions, MENU_NONE)
{
	int Idx = 0;
	Options->SetName(Idx++, "%s", tr( "App Language" ));
	Options->SetName(Idx++, "%s", tr( "Display" ));
	Options->SetName(Idx++, "%s", tr( "Clock" ));
	Options->SetName(Idx++, "%s", tr( "Clock Scale Factor" ));
	Options->SetName(Idx++, "%s", tr( "Tooltips" ));
	Options->SetName(Idx++, "%s", tr( "Tooltip Delay" ));
	Options->SetName(Idx++, "%s", tr( "Flip-X" ));
	Options->SetName(Idx++, "%s", tr( "Game Window Mode" ));
	Options->SetName(Idx++, "%s", tr( "Prompts Buttons" ));
	Options->SetName(Idx++, "%s", tr( "Widescreen Factor" ));
	Options->SetName(Idx++, "%s", tr( "Font Scale Factor" ));
	Options->SetName(Idx++, "%s", tr( "Keyboard" ));
	Options->SetName(Idx++, "%s", tr( "Disc Artwork Download" ));
	Options->SetName(Idx++, "%s", tr( "Full covers Download" ));
	Options->SetName(Idx++, "%s", tr( "Screensaver" ));
	Options->SetName(Idx++, "%s", tr( "Mark new games" ));
	Options->SetName(Idx++, "%s", tr( "Show Play Count" ));
	Options->SetName(Idx++, "%s", tr( "Show Favorite on banner" ));
	Options->SetName(Idx++, "%s", tr( "Show Free Space" ));
	Options->SetName(Idx++, "%s", tr( "HOME Menu" ));
	Options->SetName(Idx++, "%s", tr( "Use System Font" ));
	Options->SetName(Idx++, "%s", tr( "Virtual Pointer Speed" ));
	Options->SetName(Idx++, "%s", tr( "Adjust Overscan X" ));
	Options->SetName(Idx++, "%s", tr( "Adjust Overscan Y" ));

	SetOptionValues();
}

void GuiSettingsMenu::SetOptionValues()
{
	int Idx = 0;

	//! Settings: App Language
	const char * language = strrchr(App.Settings.language_path, '/');
	if(language)
		language += 1;
	if (!language || strcmp(App.Settings.language_path, "") == 0)
		Options->SetValue(Idx++, "%s", tr( "Default" ));
	else
		Options->SetValue(Idx++, "%s", language);

	//! Settings: Display
	Options->SetValue(Idx++, "%s", tr( GameInfoText[App.Settings.sinfo] ));

	//! Settings: Clock
	if (App.Settings.hddinfo == CLOCK_HR12)
		Options->SetValue(Idx++, "12 %s", tr( "Hour" ));
	else if (App.Settings.hddinfo == CLOCK_HR24)
		Options->SetValue(Idx++, "24 %s", tr( "Hour" ));
	else if (App.Settings.hddinfo == OFF)
		Options->SetValue(Idx++, "%s", tr( "OFF" ));

	//! Settings: Clock Font Scale Factor
	Options->SetValue(Idx++, "%g", App.Settings.ClockFontScaleFactor);

	//! Settings: Tooltips
	Options->SetValue(Idx++, "%s", tr(OnOffText[App.Settings.tooltips]));

	//! Settings: Tooltip Delay
	Options->SetValue(Idx++, "%i %s", App.Settings.TooltipDelay, tr("ms"));

	//! Settings: Flip-X
	Options->SetValue(Idx++, "%s%s%s", tr(FlipXText[App.Settings.xflip][0]),
				FlipXText[App.Settings.xflip][1], tr( FlipXText[App.Settings.xflip][2] ));

	//! Settings: Game Window Mode
	Options->SetValue(Idx++, "%s", tr( GameWindowText[App.Settings.GameWindowMode] ));

	//! Settings: Prompts Buttons
	Options->SetValue(Idx++, "%s", tr( PromptButtonsText[App.Settings.wsprompt] ));

	//! Settings: Widescreen Factor
	Options->SetValue(Idx++, "%g", App.Settings.WSFactor);

	//! Settings: Font Scale Factor
	Options->SetValue(Idx++, "%g", App.Settings.FontScaleFactor);

	//! Settings: Keyboard
	Options->SetValue(Idx++, "%s", KeyboardText[App.Settings.keyset]);

	//! Settings: Disc Artwork Download
	Options->SetValue(Idx++, "%s", tr( DiscArtDownloadText[App.Settings.discart] ));

	//! Settings: Covers Full Artwork Download
	Options->SetValue(Idx++, "%s", tr( CoversFullDownloadText[App.Settings.coversfull] ));

	//! Settings: Screensaver
	Options->SetValue(Idx++, "%s", tr( ScreensaverText[App.Settings.screensaver] ));

	//! Settings: Mark new games
	Options->SetValue(Idx++, "%s", tr( OnOffText[App.Settings.marknewtitles] ));

	//! Settings: Show Play Count
	Options->SetValue(Idx++, "%s", tr( OnOffText[App.Settings.ShowPlayCount] ));

	//! Settings: Show Favorite on banner window
	Options->SetValue(Idx++, "%s", tr( BannerFavIconText[App.Settings.bannerFavIcon] ));

	//! Settings: Show Free Space
	Options->SetValue(Idx++, "%s", tr( OnOffText[App.Settings.ShowFreeSpace] ));

	//! Settings: Home Menu style
	Options->SetValue(Idx++, "%s", tr( HomeMenuText[App.Settings.HomeMenu] ));

	//! Settings: Use System Font
	Options->SetValue(Idx++, "%s", tr( OnOffText[App.Settings.UseSystemFont] ));

	//! Settings: Virtual Pointer Speed
	Options->SetValue(Idx++, "%g", App.Settings.PointerSpeed);

	//! Settings: Adjust Overscan X
	Options->SetValue(Idx++, "%i", App.Settings.AdjustOverscanX);

	//! Settings: Adjust Overscan Y
	Options->SetValue(Idx++, "%i", App.Settings.AdjustOverscanY);
}

int GuiSettingsMenu::GetMenuInternal()
{
	int ret = optionBrowser->GetClickedOption();

	if (ret < 0)
		return MENU_NONE;

	int Idx = -1;

	//! Settings: App Language
	if (ret == ++Idx)
	{
		if (!App.Settings.godmode)
		{
			WindowPrompt(tr( "Language change:" ), tr( "Console should be unlocked to modify it." ), tr( "OK" ));
			return MENU_NONE;
		}
		SetEffect(EFFECT_FADE, -20);
		while (GetEffect() > 0) usleep(100);
		HaltGui();
		if(parentElement)
		{
			((GuiWindow *) parentElement)->Remove(this);
			((GuiWindow *) parentElement)->SetState(STATE_DISABLED);
		}
		ResumeGui();

		int returnhere = 1;
		while (returnhere == 1)
			returnhere = MenuLanguageSelect();

		if (returnhere == 2)
		{
			//! Language changed. Reload game titles with new lang code.
			App.Library.GameTitles.SetDefault();
			App.Library.GameTitles.LoadTitlesFromGameTDB(App.Settings.titlestxt_path);
			return MENU_SETTINGS;
		}

		HaltGui();
		if(parentElement)
		{
			((GuiWindow *) parentElement)->Append(this);
			((GuiWindow *) parentElement)->SetState(STATE_DEFAULT);
		}
		SetEffect(EFFECT_FADE, 20);
		ResumeGui();
		while (GetEffect() > 0) usleep(100);
	}

	//! Settings: Display
	else if (ret == ++Idx)
	{
		if (++App.Settings.sinfo >= GAMEINFO_MAX) App.Settings.sinfo = 0;
	}

	//! Settings: Clock
	else if (ret == ++Idx)
	{
		if (++App.Settings.hddinfo >= CLOCK_MAX) App.Settings.hddinfo = 0; //CLOCK
	}

	//! Settings: Clock Font Scale Factor
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.ClockFontScaleFactor);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.ClockFontScaleFactor = LIMIT(atof(entrie), 0.01f, 1.5f);
	}

	//! Settings: Tooltips
	else if (ret == ++Idx)
	{
		if (++App.Settings.tooltips >= MAX_ON_OFF) App.Settings.tooltips = 0;
	}

	//! Settings: Tooltip Delay
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%i", App.Settings.TooltipDelay);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.TooltipDelay = atoi(entrie);
	}

	//! Settings: Flip-X
	else if (ret == ++Idx)
	{
		if (++App.Settings.xflip >= XFLIP_MAX) App.Settings.xflip = 0;
	}

	//! Settings: Game Window Mode
	else if (ret == ++Idx)
	{
		if (++App.Settings.GameWindowMode >= 3) App.Settings.GameWindowMode = 0;

		if(App.Settings.GameWindowMode != GAMEWINDOW_DISC && !SystemMenuResources::Instance()->IsLoaded()) {
			WindowPrompt(tr( "Error:" ), tr( "Banner window is only available with AHBPROT! Please consider installing new HBC version." ), tr( "OK" ));
			App.Settings.GameWindowMode = GAMEWINDOW_DISC;
		}
	}

	//! Settings: Prompts Buttons
	else if (ret == ++Idx)
	{
		if (++App.Settings.wsprompt >= MAX_ON_OFF) App.Settings.wsprompt = 0;
	}

	//! Settings: Widescreen Factor
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.WSFactor);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.WSFactor = LIMIT(atof(entrie), 0.01f, 1.5f);
	}

	//! Settings: Font Scale Factor
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.FontScaleFactor);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.FontScaleFactor = LIMIT(atof(entrie), 0.01f, 1.5f);
	}

	//! Settings: Keyboard
	else if (ret == ++Idx)
	{
		if (++App.Settings.keyset >= KEYBOARD_MAX) App.Settings.keyset = 0;
	}

	//! Settings: Disc Artwork Download
	else if (ret == ++Idx)
	{
		if (++App.Settings.discart >= DISCARTS_MAX_CHOICE) App.Settings.discart = 0;
	}

	//! Settings: Covers Full Artwork Download
	else if (ret == ++Idx)
	{
		if (++App.Settings.coversfull >= COVERSFULL_MAX_CHOICE) App.Settings.coversfull = 0;
	}

	//! Settings: Screensaver
	else if (ret == ++Idx)
	{
		if (++App.Settings.screensaver >= SCREENSAVER_MAX) App.Settings.screensaver = 0;

		Input::SetWPADTimeout();
	}

	//! Settings: Mark new games
	else if (ret == ++Idx)
	{
		if (++App.Settings.marknewtitles >= MAX_ON_OFF) App.Settings.marknewtitles = 0;
	}

	//! Settings: Show Play Count
	else if (ret == ++Idx)
	{
		if (++App.Settings.ShowPlayCount >= MAX_ON_OFF) App.Settings.ShowPlayCount = 0;
	}

	//! Settings: Show favorite on banner window
	else if (ret == ++Idx)
	{
		if (++App.Settings.bannerFavIcon >= BANNER_FAVICON_MAX_CHOICE) App.Settings.bannerFavIcon = 0;
	}

	//! Settings: Show Free Space
	else if (ret == ++Idx)
	{
		if (++App.Settings.ShowFreeSpace >= MAX_ON_OFF) App.Settings.ShowFreeSpace = 0;
	}

	//! Settings: Home Menu Style
	else if (ret == ++Idx)
	{
		if (++App.Settings.HomeMenu >= HOME_MENU_MAX_CHOICE) App.Settings.HomeMenu = 0;
	}

	//! Settings: Use System Font
	else if (ret == ++Idx)
	{
		if (++App.Settings.UseSystemFont >= MAX_ON_OFF) App.Settings.UseSystemFont = 0;

		HaltGui();
		App.Theme.LoadFont(App.Settings.ConfigPath);
		ResumeGui();

		if(App.Settings.FontScaleFactor == 1.0f && App.Settings.UseSystemFont == ON)
			App.Settings.FontScaleFactor = 0.8f;
		else if(App.Settings.FontScaleFactor == 0.8f && App.Settings.UseSystemFont == OFF)
			App.Settings.FontScaleFactor = 1.0f;
	}

	//! Settings: Virtual Pointer Speed
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%g", App.Settings.PointerSpeed);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
			App.Settings.PointerSpeed = atof(entrie);
	}

	//! Settings: Adjust Overscan X
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%i", App.Settings.AdjustOverscanX);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
		{
			App.Settings.AdjustOverscanX = atoi(entrie);
			AdjustOverscan(App.Settings.AdjustOverscanX, App.Settings.AdjustOverscanY);
		}
	}

	//! Settings: Adjust Overscan Y
	else if (ret == ++Idx)
	{
		char entrie[20];
		snprintf(entrie, sizeof(entrie), "%i", App.Settings.AdjustOverscanY);
		int ret = OnScreenNumpad(entrie, sizeof(entrie));
		if(ret)
		{
			App.Settings.AdjustOverscanY = atoi(entrie);
			AdjustOverscan(App.Settings.AdjustOverscanX, App.Settings.AdjustOverscanY);
		}
	}


	SetOptionValues();

	return MENU_NONE;
}
