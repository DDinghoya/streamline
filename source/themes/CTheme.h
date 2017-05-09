#pragma once

#include <string>
#include <stdio.h>
#include <gctypes.h>
#include "Resources.h"
#include "gettheme.h"
#include "FreeTypeGX.h"

class CTheme
{
public:
	FreeTypeGX *fontSystem;
	FT_Byte *customFont;
	u32 customFontSize;

	CTheme();
	~CTheme();

	//!Set Default
	void SetDefault();
	
	//!Load
	bool Load(const char * path);
	
	//!Load font data
	bool LoadFont(const char *path);
	
	//!Load the original Wii System Menu font into memory only. It is not applied.
	bool loadSystemFont(bool korean);
	
	//!Reload the main images/sounds for the new theme
	void Reload();
	
	//!Clear all image/font/theme data and free the memory
	void CleanUp();

	//!Enable tooltips: special case treaded because it is called every frame
	bool ShowTooltips;

private:
	//!Clear the font data and free the memory
	void ClearFontData();
};
