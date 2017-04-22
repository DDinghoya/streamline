#pragma once

#include "settings/CSettings.h"

class Application
{
public:
    Application();
    virtual ~Application();

	// Main application settings container
    CSettings Settings;

protected:

private:
};
