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
#include "gui_element.h"
#include "gui_image.h"
#include "gui_tooltip.h"
#include "gui_sound.h"
#include "utils/timer.h"
#include "sigslot.h"

typedef struct _POINT {
	s16 x;
	s16 y;
} POINT;

//!Display, manage, and manipulate buttons in the GUI. Buttons can have images, icons, text, and sound set (all of which are optional)
class GuiButton : public GuiElement
{
public:
	//!Constructor
	//!\param w Width
	//!\param h Height
	GuiButton(int w, int h);
	//!\param img is the button GuiImage.  it uses the height & width of this image for the button
	//!\param imgOver is the button's over GuiImage
	//!\param hor is horizontal alingment of the button
	//!\param vert is verticle alignment of the button
	//!\param x is xposition of the button
	//!\param y is yposition of the button
	//!\param trig is a GuiTrigger to assign to this button
	//!\param sndOver is a GuiSound used for soundOnOver for this button
	//!\param sndClick is a GuiSound used for clickSound of this button
	//!\param grow sets effect grow for this button.  1 for yes ;0 for no
	GuiButton(GuiImage* img, GuiImage* imgOver, int hor, int vert, int x, int y, GuiTrigger* trig,
		GuiSound* sndOver, GuiSound* sndClick, u8 grow);
	//!\param same as all the parameters for the above button plus the following
	//!\param tt is a GuiTooltip assigned to this button
	//!\param ttx and tty are the xPOS and yPOS for this tooltip in relationship to the button
	//!\param h_align and v_align are horizontal and verticle alignment for the tooltip in relationship to the button
	GuiButton(GuiImage* img, GuiImage* imgOver, int hor, int vert, int x, int y, GuiTrigger* trig,
		GuiSound* sndOver, GuiSound* sndClick, u8 grow, GuiTooltip* tt, int ttx, int tty, int h_align,
		int v_align);
	//!Destructor
	virtual ~GuiButton();
	//!Sets the button's image
	//!\param i Pointer to GuiImage object
	void SetImage(GuiImage* i);
	//!Sets the button's image on over
	//!\param i Pointer to GuiImage object
	void SetImageOver(GuiImage* i);
	//!Sets the button's image on hold
	//!\param i Pointer to GuiImage object
	void SetAngle(float a);
	void SetImageHold(GuiImage* i);
	//!Sets the button's image on click
	//!\param i Pointer to GuiImage object
	void SetImageClick(GuiImage* i);
	//!Sets the button's icon
	//!\param i Pointer to GuiImage object
	void SetIcon(GuiImage* i);
	//!Sets the button's icon on over
	//!\param i Pointer to GuiImage object
	void SetIconOver(GuiImage* i);
	//!Sets the button's icon on hold
	//!\param i Pointer to GuiImage object
	void SetIconHold(GuiImage* i);
	//!Sets the button's icon on click
	//!\param i Pointer to GuiImage object
	void SetIconClick(GuiImage* i);
	//!Sets the button's label
	//!\param t Pointer to GuiText object
	//!\param n Index of label to set (optional, default is 0)
	void SetLabel(GuiText* t, int n = 0);
	//!Sets the button's label on over (eg: different colored text)
	//!\param t Pointer to GuiText object
	//!\param n Index of label to set (optional, default is 0)
	void SetLabelOver(GuiText* t, int n = 0);
	//!Sets the button's label on hold
	//!\param t Pointer to GuiText object
	//!\param n Index of label to set (optional, default is 0)
	void SetLabelHold(GuiText* t, int n = 0);
	//!Sets the button's label on click
	//!\param t Pointer to GuiText object
	//!\param n Index of label to set (optional, default is 0)
	void SetLabelClick(GuiText* t, int n = 0);
	//!Sets the sound to play on over
	//!\param s Pointer to GuiSound object
	void SetSoundOver(GuiSound * s);
	//!Sets the sound to play on hold
	//!\param s Pointer to GuiSound object
	void SetSoundHold(GuiSound * s);
	//!Sets the sound to play on click
	//!\param s Pointer to GuiSound object
	void SetSoundClick(GuiSound * s);
	//!\param reset the soundover to NULL
	void RemoveSoundOver();
	//!\param reset the soundclick to NULL
	void RemoveSoundClick();
	//!Constantly called to draw the GuiButtons ToolTip
	//!Sets the button's Tooltip on over
	//!\param tt Pointer to GuiElement object, x & y Positioning, h & v Align
	void SetToolTip(GuiTooltip* tt, int x, int y, int h = ALIGN_RIGHT, int v = ALIGN_TOP);

	void RemoveToolTip();
	//!Constantly called to draw the GuiButton
	void Draw();
	void DrawTooltip();
	//!Constantly called to allow the GuiButton to respond to updated input data
	//!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
	void Update(GuiTrigger * t);
	//!Deactivate/Activate pointing on Games while B scrolling
	void ScrollIsOn(int f);
	void SetSkew(int XX1, int YY1, int XX2, int YY2, int XX3, int YY3, int XX4, int YY4);
	void SetSkew(int *skew /* int skew[8] */);
	virtual void SetState(int s, int c = -1);
	sigslot::signal3<GuiButton *, int, const POINT&> Clicked;
	sigslot::signal3<GuiButton *, int, const POINT&> Held;

protected:
	GuiImage * image; //!< Button image (default)
	GuiImage * imageOver; //!< Button image for STATE_SELECTED
	GuiImage * imageHold; //!< Button image for STATE_HELD
	GuiImage * imageClick; //!< Button image for STATE_CLICKED
	GuiImage * icon; //!< Button icon (drawn after button image)
	GuiImage * iconOver; //!< Button icon for STATE_SELECTED
	GuiImage * iconHold; //!< Button icon for STATE_HELD
	GuiImage * iconClick; //!< Button icon for STATE_CLICKED
	GuiTooltip *toolTip;
	Timer ToolTipDelay;
	bool bOldTooltipVisible;
	GuiText * label[3]; //!< Label(s) to display (default)
	GuiText * labelOver[3]; //!< Label(s) to display for STATE_SELECTED
	GuiText * labelHold[3]; //!< Label(s) to display for STATE_HELD
	GuiText * labelClick[3]; //!< Label(s) to display for STATE_CLICKED
	GuiSound * soundOver; //!< Sound to play for STATE_SELECTED
	GuiSound * soundHold; //!< Sound to play for STATE_HELD
	GuiSound * soundClick; //!< Sound to play for STATE_CLICKED
};
