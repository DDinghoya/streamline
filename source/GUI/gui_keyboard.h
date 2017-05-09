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
#include "gui_window.h"
#include "gui_button.h"

typedef struct _keytype
{
	char ch, chShift, chalt, chalt2;
} Key;

//!On-screen keyboard
class GuiKeyboard : public GuiWindow
{
public:
	GuiKeyboard(char * t, u32 m, int min, int lang);
	virtual ~GuiKeyboard();
	void SetVisibleText(bool v) { textVisible = v; }
	const char *GetText() { return kbtextstr; }
	void Update(GuiTrigger * t);
protected:
	void SetDisplayText(const char *text);

	bool textVisible;
	char kbtextstr[256];
	u32 kbtextmaxlen;
	Key keys[4][11];
	int shift;
	int caps;
	int alt;
	int alt2;
	u16 min;
	GuiText * kbText;
	GuiImage * keyTextboxImg;
	GuiText * keyCapsText;
	GuiImage * keyCapsImg;
	GuiImage * keyCapsOverImg;
	GuiButton * keyCaps;
	GuiText * keyAltText;
	GuiImage * keyAltImg;
	GuiImage * keyAltOverImg;
	GuiButton * keyAlt;
	GuiText * keyAlt2Text;
	GuiImage * keyAlt2Img;
	GuiImage * keyAlt2OverImg;
	GuiButton * keyAlt2;
	GuiText * keyShiftText;
	GuiImage * keyShiftImg;
	GuiImage * keyShiftOverImg;
	GuiButton * keyShift;
	GuiText * keyBackText;
	GuiImage * keyBackImg;
	GuiImage * keyBackOverImg;
	GuiButton * keyBack;
	GuiText * keyClearText;
	GuiImage * keyClearImg;
	GuiImage * keyClearOverImg;
	GuiButton * keyClear;
	GuiImage * keySpaceImg;
	GuiImage * keySpaceOverImg;
	GuiButton * keySpace;
	GuiButton * keyBtn[4][11];
	GuiImage * keyImg[4][11];
	GuiImage * keyImgOver[4][11];
	GuiText * keyTxt[4][11];
	GuiImageData * keyTextbox;
	GuiImageData * key;
	GuiImageData * keyOver;
	GuiImageData * keyMedium;
	GuiImageData * keyLarge;
	GuiTrigger * trigA;
	GuiTrigger * trigB;
};
