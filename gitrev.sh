#! /bin/bash
#
ver_build_raw=$(git rev-list --count HEAD 2>/dev/null | tr '\n' ' ' | tr -d '\r')
ver_branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null)
ver_desc=$(git describe --tags 2>/dev/null | tr '\n' ' ' | tr -d '\r' | tr -d ' ')
ver_major=3
ver_minor=1
ver_patch=1
ver_build=0
a=$(echo $ver_build_raw | sed 's/\([0-9]*\).*/\1/')
let "a+=0"
#find max rev
while [ "$a" ]; do
	[ "$a" -gt "$ver_build" ] && ver_build=$a
	ver_build_raw=$(echo -n $ver_build_raw | sed 's/[0-9]*[^0-9]*\([0-9]*\)\(.*\)/\1 \2/')
	a=$(echo $ver_build_raw | sed 's/\([0-9]*\).*/\1/')
done

rev_old=$(cat ./source/Version.c 2>/dev/null | tr -d '\n' | sed 's/[^0-9]*\([0-9]*\).*/\1/')

ver_build=`expr $ver_build + 1`
rev_date=`date -u +%Y%m%d%H%M%S`

if [ "$ver_build" != "$rev_old" ] || [ ! -f ./source/Version.c ]; then
cat <<EOF > ./source/Version.c
#define SVN_REV "$ver_build"
#define SVN_MAJOR "$ver_major"
#define SVN_MINOR "$ver_minor"
#define SVN_PATCH "$ver_patch"
#define SVN_BRANCH "$ver_branch"
#define SVN_DESC "$ver_desc"

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

const char *Version_GetDescription()
{
	return SVN_DESC;
}

const char *Version_GetVersionString()
{
	return SVN_MAJOR "." SVN_MINOR "." SVN_PATCH "." SVN_REV "-" SVN_BRANCH;
}
EOF

	if [ -n "$ver_build" ]; then
		echo "Changed Rev $rev_old to $ver_build" >&2
	else
		echo "Version.c created" >&2
	fi
	echo >&2
fi


cat <<EOF > ./distrib/usbloader_gx/meta.xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<app version="1">
		<name>USB Loader GX</name>
		<coder>USB Loader GX Team</coder>
		<version>$ver_major.$ver_minor.$ver_patch.$ver_build-$ver_branch</version>
		<release_date>$rev_date</release_date>
		<!--   // remove this line to enable arguments
		<arguments>
				<arg>--ios=250</arg>
				<arg>--usbport=0</arg>
				<arg>--mountusb=1</arg>
		</arguments>
		// remove this line to enable arguments -->
		<ahb_access/>
		<short_description>Loads games from USB-devices</short_description>
		<long_description>USB Loader GX is a libwiigui based USB iso loader with a wii-like GUI. You can install games to your HDDs and boot them with shorter loading times.
The interactive GUI is completely controllable with WiiMote, Classic Controller or GC Controller.
Features are automatic widescreen detection, coverdownload, parental control, theme support and many more.

Credits:
Coding: Cyan, Dimok, nIxx, giantpune, ardi, Hungyip84, DrayX7, Lustar, r-win, WiiShizzza
Artworks: cyrex, NeoRame
Validation: Cyan and many others
Issue management: Cyan
WiiTDB / Hosting covers: Lustar
USBLoader sources: Waninkoko, Kwiirk, Hermes
cIOS maintenance: davebaol, xabby666, XFlak and Rodries
Languages files updates: Kinyo and translaters
Hosting themes: Deak Phreak

Libwiigui: Tantric
Libogc/Devkit: Shagkur and Wintermute
FreeTypeGX: Armin Tamzarian.

Links:
USB Loader GX Project Page 
https://sourceforge.net/projects/usbloadergx/
Support Site:
http://gbatemp.net/index.php?showtopic=149922
Help Website:
http://usbloadergx.koureio.net/
WiiTDB Site:
http://wiitdb.com
Themes Site:
http://wii.spiffy360.com
Languages Translaters Page:
http://gbatemp.net/index.php?showtopic=155252

Libwiigui Website:
http://wiibrew.org/wiki/Libwiigui/
FreeTypeGX Project Page:
http://code.google.com/p/freetypegx/
Gettext Official Page:
http://www.gnu.org/software/gettext/gettext.html
		</long_description>
	</app>
EOF
