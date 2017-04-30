/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * demo.h
 ***************************************************************************/

#pragma once

#include "settings/CSettings.h"
#include "GUI/gui.h"
#include "Data/TitleLibrary.h"

class Application
{
public:
	Application();
    ~Application();
	// Main application settings container
	CSettings Settings;

	TitleLibrary Library;
	GuiWindow *MainWindow;


protected:

private:

};

// Main loader application reference
extern Application App;
