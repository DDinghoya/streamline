/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * input.h
 * Wii/GameCube controller management
 ***************************************************************************/

#pragma once

#include <wiiuse/wpad.h>
#include "GUI/gui_trigger.h"

namespace Input
{
#define PADCAL		  50
#define WUPCCAL		  400

	extern int rumbleRequest[4];

	GuiTrigger *GetUserInput(int index);
	void SetupPads();
	void UpdatePads();
	void ShutoffRumble();
	void DoRumble(int i);
	void SetWPADTimeout();
	bool ControlActivityTimeout(void);
}
