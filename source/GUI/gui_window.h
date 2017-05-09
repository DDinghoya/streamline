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
#include <vector>
#include "gui_element.h"

//!Allows GuiElements to be grouped together into a "window"
class GuiWindow : public GuiElement
{
public:
	//!Constructor
	GuiWindow();
	//!\overload
	//!\param w Width of window
	//!\param h Height of window
	GuiWindow(int w, int h);
	//!Destructor
	virtual ~GuiWindow();
	//!Appends a GuiElement to the GuiWindow
	//!\param e The GuiElement to append. If it is already in the GuiWindow, it is removed first
	void Append(GuiElement* e);
	//!Inserts a GuiElement into the GuiWindow at the specified index
	//!\param e The GuiElement to insert. If it is already in the GuiWindow, it is removed first
	//!\param i Index in which to insert the element
	void Insert(GuiElement* e, u32 i);
	//!Removes the specified GuiElement from the GuiWindow
	//!\param e GuiElement to be removed
	void Remove(GuiElement* e);
	//!Removes all GuiElements
	void RemoveAll();
	//!Returns the GuiElement at the specified index
	//!\param index The index of the element
	//!\return A pointer to the element at the index, NULL on error (eg: out of bounds)
	GuiElement* GetGuiElementAt(u32 index) const;
	//!Returns the size of the list of elements
	//!\return The size of the current element list
	u32 GetSize();
	//!Sets the visibility of the window
	//!\param v visibility (true = visible)
	void SetVisible(bool v);
	//!Resets the window's state to STATE_DEFAULT
	void ResetState();
	//!Sets the window's state
	//!\param s State
	void SetState(int s);
	//!Gets the index of the GuiElement inside the window that is currently selected
	//!\return index of selected GuiElement
	int GetSelected();
	//!Moves the selected element to the element to the left or right
	//!\param d Direction to move (-1 = left, 1 = right)
	void MoveSelectionHor(int d);
	//!Moves the selected element to the element above or below
	//!\param d Direction to move (-1 = up, 1 = down)
	void MoveSelectionVert(int d);
	//!Allow dim of screen on disable or not
	void SetAllowDim(bool d) { allowDim = d; }
	void SetDimScreen(bool d) { forceDim = d; }
	//!Draws all the elements in this GuiWindow
	void Draw();
	void DrawTooltip();
	//!Updates the window and all elements contains within
	//!Allows the GuiWindow and all elements to respond to the input data specified
	//!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
	void Update(GuiTrigger * t);
protected:
	bool forceDim;
	bool allowDim;
	std::vector<GuiElement*> _elements; //!< Contains all elements within the GuiWindow
};
