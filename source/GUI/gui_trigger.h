/****************************************************************************
* LibWiiGui by Tantric (C) 2009
* USB Loader GX Team (C) 2009-2011
*
* The LibWiiGui library was used as the base for the creation of
* the GUI in USB Loader GX.
* Several modifications and additions were made to the library
* It does no longer match the original LibWiiGui implementation.
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
#pragma once

#include <gctypes.h>
#include <wiiuse/wpad.h>

typedef struct _paddata
{
	u16 btns_d;
	u16 btns_u;
	u16 btns_h;
	s8 stickX;
	s8 stickY;
	s8 substickX;
	s8 substickY;
	u8 triggerL;
	u8 triggerR;
} PADData;

typedef struct _wupcfulldata {
	u32 btns_d;
	u32 btns_u;
	u32 btns_h;
	s16 stickX;
	s16 stickY;
	s16 substickX;
	s16 substickY;
} WUPCFULLData;

enum
{
	TRIGGER_SIMPLE, TRIGGER_HELD, TRIGGER_BUTTON_ONLY
};

//!Menu input trigger management. Determine if action is neccessary based on input data by comparing controller input data to a specific trigger element.
class GuiTrigger
{
public:
	//!Constructor
	GuiTrigger();
	//!Destructor
	virtual ~GuiTrigger();
	//!Sets a simple trigger. Requires: element is selected, and trigger button is pressed
	//!\param ch Controller channel number
	//!\param wiibtns Wii controller trigger button(s) - classic controller buttons are considered separately
	//!\param gcbtns GameCube controller trigger button(s)
	void SetSimpleTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
	//!Sets a held trigger. Requires: element is selected, and trigger button is pressed
	//!\param ch Controller channel number
	//!\param wiibtns Wii controller trigger button(s) - classic controller buttons are considered separately
	//!\param gcbtns GameCube controller trigger button(s)
	void SetHeldTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
	//!Sets a button-only trigger. Requires: Trigger button is pressed
	//!\param ch Controller channel number
	//!\param wiibtns Wii controller trigger button(s) - classic controller buttons are considered separately
	//!\param gcbtns GameCube controller trigger button(s)
	void SetButtonOnlyTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
	//!Get X/Y value from Wii Joystick (classic, nunchuk) input
	//!\param right Controller stick (left = 0, right = 1)
	//!\param axis Controller stick axis (x-axis = 0, y-axis = 1)
	//!\return Stick value
	s8 WPAD_Stick(u8 right, int axis);
	//!Move menu selection left (via pad/joystick). Allows scroll delay and button overriding
	//!\return true if selection should be moved left, false otherwise
	bool Left();
	//!Move menu selection right (via pad/joystick). Allows scroll delay and button overriding
	//!\return true if selection should be moved right, false otherwise
	bool Right();
	//!Move menu selection up (via pad/joystick). Allows scroll delay and button overriding
	//!\return true if selection should be moved up, false otherwise
	bool Up();
	//!Move menu selection down (via pad/joystick). Allows scroll delay and button overriding
	//!\return true if selection should be moved down, false otherwise
	bool Down();

	u8 type; //!< trigger type (TRIGGER_SIMPLE, TRIGGER_HELD, TRIGGER_BUTTON_ONLY)
	s32 chan; //!< Trigger controller channel (0-3, -1 for all)
	WPADData wpad; //!< Wii controller trigger data
	WUPCFULLData wupcdata;//!< Wii U pro controller trigger data
	PADData pad; //!< GameCube controller trigger data
};
