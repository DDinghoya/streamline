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

#include <gccore.h>
#include <vector>
#include "gui_element.h"
#include "FreeTypeGX.h"

enum
{
	WRAP, DOTTED, SCROLL_HORIZONTAL, SCROLL_NONE
};

//!Display, manage, and manipulate text in the GUI
class GuiText : public GuiElement
{
public:
	//!Constructor
	//!\param t Text
	//!\param s Font size
	//!\param c Font color
	GuiText(const char * t, int s, GXColor c);
	//!\overload
	//!\param t Text
	//!\param s Font size
	//!\param c Font color
	GuiText(const wchar_t * t, int s, GXColor c);
	//!\overload
	//!\Assumes SetPresets() has been called to setup preferred text attributes
	//!\param t Text
	GuiText(const char * t);
	//!Destructor
	virtual ~GuiText();
	//!Sets the text of the GuiText element
	//!\param t Text
	virtual void SetText(const char * t);
	virtual void SetText(const wchar_t * t);
	virtual void SetTextf(const char *format, ...) __attribute__((format(printf, 2, 3)));
	//!Sets up preset values to be used by GuiText(t)
	//!Useful when printing multiple text elements, all with the same attributes set
	//!\param sz Font size
	//!\param c Font color
	//!\param w Maximum width of texture image (for text wrapping)
	//!\param wrap Wrapmode when w>0
	//!\param s Font style
	//!\param h Text alignment (horizontal)
	//!\param v Text alignment (vertical)
	static void SetPresets(int sz, GXColor c, int w, u16 s, int h, int v);
	//!Sets the font size
	//!\param s Font size
	void SetFontSize(int s);
	//!Sets the maximum width of the drawn texture image
	//!If the text exceeds this, it is wrapped to the next line
	//!\param w Maximum width
	//!\param m WrapMode
	void SetMaxWidth(int w = 0, int m = WRAP);
	//!Sets the font color
	//!\param c Font color
	void SetColor(GXColor c);
	//!Sets the FreeTypeGX style attributes
	//!\param s Style attributes
	//!\param m Style-Mask attributes
	void SetStyle(u16 s);
	//!Sets the text alignment
	//!\param hor Horizontal alignment (ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
	//!\param vert Vertical alignment (ALIGN_TOP, ALIGN_BOTTOM, ALIGN_MIDDLE)
	void SetAlignment(int hor, int vert);
	//!Set PassChar
	void SetPassChar(wchar_t p);
	//!Sets the font
	//!\param f Font
	void SetFont(FreeTypeGX *f);
	//!Get the original text as char
	virtual const wchar_t * GetText();
	//!Overload for GetWidth()
	int GetWidth() { return GetTextWidth(); }
	//!Get the Horizontal Size of Text
	int GetTextWidth();
	int GetTextWidth(int ind);
	//!Get the max textwidth
	int GetTextMaxWidth();
	//!Gets the total line number
	virtual int GetLinesCount() { return 1; }
	//!Get fontsize
	int GetFontSize() { return size; }
	//!Set max lines to draw
	void SetLinesToDraw(int l);
	void SetWidescreen(bool b) { widescreen = b; }
	//!Get current Textline (for position calculation)
	const wchar_t * GetDynText(int ind = 0);
	virtual const wchar_t * GetTextLine(int ind) { return GetDynText(ind); }
	//!Change the font
	//!\param font bufferblock
	//!\param font filesize
	bool SetFont(const u8 *font, const u32 filesize);
	//!Constantly called to draw the text
	void Draw();

protected:
	//!Clear the dynamic text
	void ClearDynamicText();
	//!Create a dynamic dotted text if the text is too long
	void MakeDottedText();
	//!Scroll the text once
	void ScrollText();
	//!Wrap the text to several lines
	void WrapText();

	wchar_t *text;
	std::vector<wchar_t *> textDyn;
	int wrapMode; //!< Wrapping toggle
	int textScrollPos; //!< Current starting index of text string for scrolling
	int textScrollInitialDelay; //!< Delay to wait before starting to scroll
	int textScrollDelay; //!< Scrolling speed
	int size; //!< Font size
	int maxWidth; //!< Maximum width of the generated text object (for text wrapping)
	u16 style; //!< FreeTypeGX style attributes
	GXColor color; //!< Font color
	FreeTypeGX *font;
	int textWidth;
	int currentSize;
	int linestodraw;
	wchar_t passChar;
	bool widescreen;
};
