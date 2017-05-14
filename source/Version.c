#define SVN_REV "1280"
#define SVN_MAJOR "3"
#define SVN_MINOR "1"
#define SVN_PATCH "1"
#define SVN_BRANCH "master"
#define SVN_DESC "b1278-1-g79cc3c31"

const char *Version_GetMajor()
{
	return SVN_REV;
}

const char *Version_GetMinor()
{
	return SVN_REV;
}

const char *Version_GetPatch()
{
	return SVN_PATCH;
}

const char *Version_GetBuild()
{
	return SVN_REV;
}

const char *Version_GetBranch()
{
	return SVN_BRANCH;
}

const char *Version_GetVersionString()
{
	return SVN_DESC;
}
