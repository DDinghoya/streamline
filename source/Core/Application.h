#pragma once

#include "settings/CSettings.h"
#include "GUI/gui.h"
#include "Library/TitleLibrary.h"
#include "themes/CTheme.h"
#include "Debug.h"

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
