#pragma once

#include "settings/CSettings.h"
#include "Data/TitleLibrary.h"
#include "GUI/gui.h"

class Application
{
public:
    Application();
    virtual ~Application();

	// Main application settings container
    CSettings Settings;

	TitleLibrary Library;

	GuiWindow *MainWindow;

protected:

private:
};
