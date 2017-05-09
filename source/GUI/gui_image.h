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
#include <gctypes.h>
#include "gui_element.h"
#include "gui_imagedata.h"

enum
{
	IMAGE_TEXTURE, IMAGE_COLOR, IMAGE_DATA, IMAGE_COPY
};

//!Display, manage, and manipulate images in the GUI
class GuiImage : public GuiElement
{
public:
	//!Constructor
	GuiImage();
	//!\overload
	//!\param img Pointer to GuiImageData element
	GuiImage(GuiImageData * img);
	//!\overload
	//!Sets up a new image from the image data specified
	//!\param img
	//!\param w Image width
	//!\param h Image height
	GuiImage(u8 * img, int w, int h);
	//!\overload
	//!Creates an image filled with the specified color
	//!\param w Image width
	//!\param h Image height
	//!\param c Image color
	GuiImage(int w, int h, GXColor c);
	//! Copy Constructor
	GuiImage(GuiImage &srcimage);
	GuiImage(GuiImage *srcimage);
	//! = operator for copying images
	GuiImage &operator=(GuiImage &srcimage);
	//!Destructor
	virtual ~GuiImage();
	//!Sets the image rotation angle for drawing
	//!\param a Angle (in degrees)
	void SetAngle(float a);
	//!Gets the image rotation angle for drawing
	float GetAngle();
	//!Sets the number of times to draw the image horizontally
	//!\param t Number of times to draw the image
	void SetTileHorizontal(int t);
	void SetTileVertical(int t);
	// true set horizontal scale to 0.8 //added
	void SetWidescreen(bool w);
	//!Constantly called to draw the image
	void Draw();
	//!Gets the image data
	//!\return pointer to image data
	u8 * GetImage();
	//!Sets up a new image using the GuiImageData object specified
	//!\param img Pointer to GuiImageData object
	void SetImage(GuiImageData * img);
	//!\overload
	//!\param img Pointer to image data
	//!\param w Width
	//!\param h Height
	void SetImage(u8 * img, int w, int h);
	//!Gets the pixel color at the specified coordinates of the image
	//!\param x X coordinate
	//!\param y Y coordinate
	GXColor GetPixel(int x, int y);
	//!Sets the pixel color at the specified coordinates of the image
	//!\param x X coordinate
	//!\param y Y coordinate
	//!\param color Pixel color
	void SetPixel(int x, int y, GXColor color);
	//!Sets the image to grayscale
	void SetGrayscale(void);
	//!Set/disable the use of parentelement angle (default true)
	void SetParentAngle(bool a);
	//!Directly modifies the image data to create a color-striped effect
	//!Alters the RGB values by the specified amount
	//!\param s Amount to increment/decrement the RGB values in the image
	void ColorStripe(int s);
	//!Sets a stripe effect on the image, overlaying alpha blended rectangles
	//!Does not alter the image data
	//!\param s Alpha amount to draw over the image
	void SetStripe(int s);
	s32 z;
	void SetSkew(int XX1, int YY1, int XX2, int YY2, int XX3, int YY3, int XX4, int YY4);
	void SetSkew(int *skew /* int skew[8] */);
	int xx1;
	int yy1;
	int xx2;
	int yy2;
	int xx3;
	int yy3;
	int xx4;
	int yy4;
	int rxx1;
	int ryy1;
	int rxx2;
	int ryy2;
	int rxx3;
	int ryy3;
	int rxx4;
	int ryy4;
protected:
	int imgType; //!< Type of image data (IMAGE_TEXTURE, IMAGE_COLOR, IMAGE_DATA)
	u8 * image; //!< Poiner to image data. May be shared with GuiImageData data
	f32 imageangle; //!< Angle to draw the image
	int tileHorizontal; //!< Number of times to draw (tile) the image horizontally
	int tileVertical; //!< Number of times to draw (tile) the image vertically
	u8 stripe; //!< Alpha value (0-255) to apply a stripe effect to the texture
	short widescreen; //added
	bool parentangle;
};
