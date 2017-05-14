#pragma once

#include <gctypes.h>
#include "GUI/gui.h"
#include "language/gettext.h"
#include "prompts/PromptWindows.h"
#include "menu.h"
#include "Debug.h"
#include "sys.h"

extern u8 shutdown;
extern u8 reset;

int MenuInstall();
int SelectPartitionMenu();
