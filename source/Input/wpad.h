#pragma once

#include <wiiuse/wpad.h>
#include <wupc/wupc.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/* Prototypes */
	s32 Wpad_Init(void);
	void Wpad_Disconnect(void);
	u32 ButtonsPressed(void);
	u32 ButtonsHold(void);
	bool IsWpadConnected();

#ifdef __cplusplus
}
#endif
