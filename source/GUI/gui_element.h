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
#include "gui_trigger.h"

typedef void(*UpdateCallback)(void * e);

enum
{
	ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER, ALIGN_TOP, ALIGN_BOTTOM, ALIGN_MIDDLE
};

enum
{
	STATE_DEFAULT, STATE_SELECTED, STATE_CLICKED, STATE_HELD, STATE_DISABLED
};

#define EFFECT_SLIDE_TOP			1
#define EFFECT_SLIDE_BOTTOM		 2
#define EFFECT_SLIDE_RIGHT		  4
#define EFFECT_SLIDE_LEFT		   8
#define EFFECT_SLIDE_IN			 16
#define EFFECT_SLIDE_OUT			32
#define EFFECT_FADE				 64
#define EFFECT_SCALE				128
#define EFFECT_COLOR_TRANSITION	 256
#define EFFECT_PULSE				512
#define EFFECT_ROCK_VERTICLE		1024
#define EFFECT_GOROUND			  2048

//!Primary GUI class. Most other classes inherit from this class.
class GuiElement
{
public:
	//!Constructor
	GuiElement();
	//!Destructor
	virtual ~GuiElement();
	//!Set the element's parent
	//!\param e Pointer to parent element
	void SetParent(GuiElement * e);
	//!Gets the element's parent
	//!\return Pointer to parent element
	GuiElement * GetParent() { return parentElement; }
	//!Gets the current leftmost coordinate of the element
	//!Considers horizontal alignment, x offset, width, and parent element's GetLeft() / GetWidth() values
	//!\return left coordinate
	int GetLeft();
	//!Gets the current topmost coordinate of the element
	//!Considers vertical alignment, y offset, height, and parent element's GetTop() / GetHeight() values
	//!\return top coordinate
	int GetTop();
	//!Sets the minimum y offset of the element
	//!\param y Y offset
	void SetMinY(int y);
	//!Gets the minimum y offset of the element
	//!\return Minimum Y offset
	int GetMinY() { return ymin; }
	//!Sets the maximum y offset of the element
	//!\param y Y offset
	void SetMaxY(int y);
	//!Gets the maximum y offset of the element
	//!\return Maximum Y offset
	int GetMaxY() { return ymax; }
	//!Sets the minimum x offset of the element
	//!\param x X offset
	void SetMinX(int x);
	//!Gets the minimum x offset of the element
	//!\return Minimum X offset
	int GetMinX() { return xmin; }
	//!Sets the maximum x offset of the element
	//!\param x X offset
	void SetMaxX(int x);
	//!Gets the maximum x offset of the element
	//!\return Maximum X offset
	int GetMaxX() { return xmax; }
	//!Gets the current width of the element. Does not currently consider the scale
	//!\return width
	virtual int GetWidth() { return width; }
	//!Gets the height of the element. Does not currently consider the scale
	//!\return height
	virtual int GetHeight() { return height; }
	//!Sets the size (width/height) of the element
	//!\param w Width of element
	//!\param h Height of element
	void SetSize(int w, int h);
	//!Checks whether or not the element is visible
	//!\return true if visible, false otherwise
	bool IsVisible() { return visible; }
	//!Checks whether or not the element is selectable
	//!\return true if selectable, false otherwise
	bool IsSelectable();
	//!Checks whether or not the element is clickable
	//!\return true if clickable, false otherwise
	bool IsClickable();
	//!Checks whether or not the element is holdable
	//!\return true if holdable, false otherwise
	bool IsHoldable();
	//!Sets whether or not the element is selectable
	//!\param s Selectable
	void SetSelectable(bool s);
	//!Sets whether or not the element is clickable
	//!\param c Clickable
	void SetClickable(bool c);
	//!Sets whether or not the element is holdable
	//!\param c Holdable
	void SetHoldable(bool d);
	//!Gets the element's current state
	//!\return state
	int GetState() { return state; }
	//!Gets the controller channel that last changed the element's state
	//!\return Channel number (0-3, -1 = no channel)
	int GetStateChan() { return stateChan; }
	//!Sets the element's alpha value
	//!\param a alpha value
	void SetAlpha(int a);
	//!Gets the element's alpha value
	//!Considers alpha, alphaDyn, and the parent element's GetAlpha() value
	//!\return alpha
	int GetAlpha();
	//!Gets the element's AngleDyn value
	//!\return alpha
	float GetAngleDyn();
	//!Sets the element's scale
	//!\param s scale (1 is 100%)
	void SetScale(float s);
	//!Gets the element's current scale
	//!Considers scale, scaleDyn, and the parent element's GetScale() value
	virtual float GetScale();
	//!Set a new GuiTrigger for the element
	//!\param t Pointer to GuiTrigger
	void SetTrigger(GuiTrigger * t);
	//!\overload
	//!\param i Index of trigger array to set
	//!\param t Pointer to GuiTrigger
	void SetTrigger(u8 i, GuiTrigger * t);
	//!Remove GuiTrigger for the element
	//!\param i Index of trigger array to set
	void RemoveTrigger(u8 i);
	//!Checks whether rumble was requested by the element
	//!\return true is rumble was requested, false otherwise
	bool Rumble() { return rumble; }
	//!Sets whether or not the element is requesting a rumble event
	//!\param r true if requesting rumble, false if not
	void SetRumble(bool r);
	//!Set an effect for the element
	//!\param e Effect to enable
	//!\param a Amount of the effect (usage varies on effect)
	//!\param t Target amount of the effect (usage varies on effect)
	void SetEffect(int e, int a, int t = 0);
	//!This SetEffect is for EFFECT_GOROUND only
	//!\param e Effect to enable
	//!\param speed is for Circlespeed
	//!\param circles Circleamount in degree ike 180 for 1/2 circle or 720 for 2 circles
	//!\param r Circle Radius in pixel
	//!\param startdegree Degree where to start circling
	//!\param anglespeedset Set the speed of Angle rotating make 1 for same speed as Circlespeed
	//!	   or 0.5 for half the speed of the circlingspeed. Turn Anglecircling off by 0 to this param.
	//!\param center_x x co-ordinate of the center of circle.
	//!\param center_y y co-ordinate of the center of circle.
	void SetEffect(int e, int speed, f32 circles, int r, f32 startdegree, f32 anglespeedset, int center_x,
		int center_y);
	//!Gets the frequency from the above effect
	//!\return element frequency
	float GetFrequency();
	//!Sets an effect to be enabled on wiimote cursor over
	//!\param e Effect to enable
	//!\param a Amount of the effect (usage varies on effect)
	//!\param t Target amount of the effect (usage varies on effect)
	void SetEffectOnOver(int e, int a, int t = 0);
	//!Shortcut to SetEffectOnOver(EFFECT_SCALE, 4, 110)
	void SetEffectGrow();
	//!Stops the current element effect
	void StopEffect();
	//!Gets the current element effects
	//!\return element effects
	int GetEffect() { return effects; }
	//!Gets the current element on over effects
	//!\return element on over effects
	int GetEffectOnOver() { return effectsOver; }
	//!Checks whether the specified coordinates are within the element's boundaries
	//!\param x X coordinate
	//!\param y Y coordinate
	//!\return true if contained within, false otherwise
	bool IsInside(int x, int y);
	//!Sets the element's position
	//!\param x X coordinate
	//!\param y Y coordinate
	void SetPosition(int x, int y, int z = 0);
	//!Sets the element's relative position
	int GetRelLeft();
	int GetRelTop();
	//!Sets the element's setup position
	int GetLeftPos() const { return xoffset; }
	int GetTopPos() const { return yoffset; }
	//!Updates the element's effects (dynamic values)
	//!Called by Draw(), used for animation purposes
	void UpdateEffects();
	//!Sets a function to called after after Update()
	//!Callback function can be used to response to changes in the state of the element, and/or update the element's attributes
	void SetUpdateCallback(UpdateCallback u);
	//!Sets the element's visibility
	//!\param v Visibility (true = visible)
	virtual void SetVisible(bool v);
	//!Sets the element's state
	//!\param s State (STATE_DEFAULT, STATE_SELECTED, STATE_CLICKED, STATE_DISABLED)
	//!\param c Controller channel (0-3, -1 = none)
	virtual void SetState(int s, int c = -1);
	//!Resets the element's state to STATE_DEFAULT
	virtual void ResetState();
	//!Gets whether or not the element is in STATE_SELECTED
	//!\return true if selected, false otherwise
	virtual int GetSelected();
	//!Sets the element's alignment respective to its parent element
	//!\param hor Horizontal alignment (ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
	//!\param vert Vertical alignment (ALIGN_TOP, ALIGN_BOTTOM, ALIGN_MIDDLE)
	virtual void SetAlignment(int hor, int vert);
	//!Called constantly to allow the element to respond to the current input data
	//!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
	virtual void Update(GuiTrigger * t);
	//!Called constantly to redraw the element
	virtual void Draw();
	virtual void DrawTooltip();
protected:
	void LockElement();
	void UnlockElement();
	//	  static mutex_t mutex;
	static mutex_t _lock_mutex;
	lwp_t _lock_thread;
	u16 _lock_count;
	lwpq_t _lock_queue;
	friend class SimpleLock;

	//int position2; //! B Scrollbariable
	bool visible; //!< Visibility of the element. If false, Draw() is skipped
	int width; //!< Element width
	int height; //!< Element height
	int xoffset; //!< Element X offset
	int yoffset; //!< Element Y offset
	int zoffset; //!< Element Z offset
	int ymin; //!< Element's min Y offset allowed
	int ymax; //!< Element's max Y offset allowed
	int xmin; //!< Element's min X offset allowed
	int xmax; //!< Element's max X offset allowed
	int xoffsetDyn; //!< Element X offset, dynamic (added to xoffset value for animation effects)
	int yoffsetDyn; //!< Element Y offset, dynamic (added to yoffset value for animation effects)
	int temp_xoffset; //!< Element Temp X offset
	int temp_yoffset; //!< Element Temp Y offset
	f32 degree; //!< Degree where to start for EFFECT_GOROUND enter it in ° like 60°
	f32 frequency; //!< Speed for EFFECT_GOROUND || can also be negative for other direction
	int Radius; //!< The radius in which the Element goes round for EFFECT_GOROUND
	f32 circleamount; //!< Circleamount for the EFFECT_GOROUND effect
	f32 xoffsetDynFloat; //!< Integer sucks float is need by some parts
	f32 yoffsetDynFloat; //!< Integer sucks float is need by some parts
	int changervar; //!< Changervariable for some stuff
	int alpha; //!< Element alpha value (0-255)
	f32 scale; //!< Element scale (1 = 100%)
	f32 angleDyn; //!< AngleDyn for EFFECT_GOROUND
	f32 anglespeed; //!<Anglespeedvariable for EFFECT_GOROUND
	int alphaDyn; //!< Element alpha, dynamic (multiplied by alpha value for blending/fading effects)
	f32 scaleDyn; //!< Element scale, dynamic (multiplied by alpha value for blending/fading effects)
	bool rumble; //!< Wiimote rumble (on/off) - set to on when this element requests a rumble event
	int effects; //!< Currently enabled effect(s). 0 when no effects are enabled
	int effectAmount; //!< Effect amount. Used by different effects for different purposes
	int effectTarget; //!< Effect target amount. Used by different effects for different purposes
	int effectsOver; //!< Effects to enable when wiimote cursor is over this element. Copied to effects variable on over event
	int effectAmountOver; //!< EffectAmount to set when wiimote cursor is over this element
	int effectTargetOver; //!< EffectTarget to set when wiimote cursor is over this element
	int alignmentHor; //!< Horizontal element alignment, respective to parent element (LEFT, RIGHT, CENTRE)
	int alignmentVert; //!< Horizontal element alignment, respective to parent element (TOP, BOTTOM, MIDDLE)
	int state; //!< Element state (DEFAULT, SELECTED, CLICKED, DISABLED)
	int stateChan; //!< Which controller channel is responsible for the last change in state
	bool selectable; //!< Whether or not this element selectable (can change to SELECTED state)
	bool clickable; //!< Whether or not this element is clickable (can change to CLICKED state)
	bool holdable; //!< Whether or not this element is holdable (can change to HELD state)
	GuiTrigger * trigger[6]; //!< GuiTriggers (input actions) that this element responds to
	GuiElement * parentElement; //!< Parent element
	UpdateCallback updateCB; //!< Callback function to call when this element is updated
};
class SimpleLock
{
public:
	SimpleLock(GuiElement *e);
	~SimpleLock();
private:
	GuiElement *element;
};
#define LOCK(e) SimpleLock LOCK(e)
