#define SVN_REV "1275"
#define SVN_MAJOR "3"
#define SVN_MINOR "1"
#define SVN_BRANCH "master"

const char *Version_GetVersion()
{
	return SVN_MAJOR "." SVN_MINOR "." SVN_REV;
}

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
	return SVN_REV;
}

const char *Version_GetBranch()
{
	return SVN_BRANCH;
}

const char *Version_GetName()
{
	return SVN_MAJOR "." SVN_MINOR "." SVN_REV "-" SVN_BRANCH;
}
