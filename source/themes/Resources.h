#pragma once

#include <gctypes.h>
#include "GUI/gui_imagedata.h"

typedef struct _RecourceFile
{
	const char *filename;
	const u8   *DefaultFile;
	const u32   DefaultFileSize;
	u8		 *CustomFile;
	u32		 CustomFileSize;
} RecourceFile;

class CResources
{
public:
	void Clear();
	bool LoadFiles(const char * path);
	const u8 * GetFile(const char * filename);
	u32 GetFileSize(const char * filename);
	GuiImageData * GetImageData(const char * filename);

private:
	static RecourceFile RecourceFiles[];
};
