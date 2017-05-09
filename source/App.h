/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * demo.h
 ***************************************************************************/

#pragma once

#include "settings/CSettings.h"
#include "GUI/gui.h"
#include "Library/TitleLibrary.h"
#include "themes/CTheme.h"

#undef USE_DEFAULT_IOS

class Application
{
public:
	Application();
    ~Application();

	/// <summary>
	/// Main application settings container
	/// </summary>
	CSettings Settings;

	TitleLibrary Library;

	CResources Resources;
	CTheme Theme;
	GuiWindow *MainWindow;


protected:

private:

};

// Main loader application instance
extern Application App;
