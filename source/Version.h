#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	const char *Version_GetName();
	const char *Version_GetVersion();
	const char *Version_GetMajor();
	const char *Version_GetMinor();
	const char *Version_GetPatch();
	const char *Version_GetBranch();

#ifdef __cplusplus
}
#endif
