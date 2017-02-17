#ifndef SVNREV_H
#define SVNREV_H

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

#endif /* SVNREV_H */
