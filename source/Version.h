#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	const char *Version_GetVersionString();
	const char *Version_GetMajor();
	const char *Version_GetMinor();
	const char *Version_GetBuild();
	const char *Version_GetPatch();
	const char *Version_GetBranch();
	const char *Version_GetDescription();

#ifdef __cplusplus
}
#endif
