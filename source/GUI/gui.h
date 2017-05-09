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

#include "gui_imagedata.h"
#include "gui_sound.h"
#include "gui_bgm.h"
#include "gui_trigger.h"
#include "gui_element.h"
#include "gui_window.h"
#include "gui_image.h"
#include "gui_text.h"
#include "gui_tooltip.h"
#include "gui_button.h"
#include "gui_keyboard.h"

 //! Frequently used variables
extern GuiSound *btnSoundClick;
extern GuiSound *btnSoundClick2;
extern GuiSound *btnSoundOver;
extern GuiBGM *bgMusic;

#define PAGESIZE				9
#define FILEBROWSERSIZE		 8
#define MAX_OPTIONS			 170
