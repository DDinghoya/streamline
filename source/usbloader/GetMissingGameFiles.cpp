#include <stdio.h>
#include <string>
#include <vector>
#include "IO/fileops.h"
#include "utils/wstring.hpp"
#include "App.h"

extern struct discHdr *dvdheader;

/**************************************************************************************
 * FindMissingFiles
 * Inputs:
 * path - Path to search in with. example "SD:/covers/"
 * fileext - the file extension. example ".png"
 * MissingFilesList - string vector where the IDs of missing game files will be put in.
 **************************************************************************************/
int GetMissingGameFiles(const char * path, const char * fileext, std::vector<std::string> & MissingFilesList)
{
	char gameID[7];
	char filepath[512];
	MissingFilesList.clear();
	wString oldFilter(App.Library.Games.GetCurrentFilter());

	//! make sure that all games are added to the gamelist
	App.Library.Games.LoadUnfiltered();

	for (int i = 0; i < App.Library.Games.size(); ++i)
	{
		struct discHdr* header = App.Library.Games[i];
		snprintf(gameID, sizeof(gameID), "%s", (char *)header->id);
		snprintf(filepath, sizeof(filepath), "%s/%s%s", path, gameID, fileext);

		if (CheckFile(filepath))
			continue;

		//! Not found. Try 4 ID path.
		gameID[4] = '\0';
		snprintf(filepath, sizeof(filepath), "%s/%s%s", path, gameID, fileext);

		if (CheckFile(filepath))
			continue;

		//! Not found. Try 3 ID path.
		gameID[3] = '\0';
		snprintf(filepath, sizeof(filepath), "%s/%s%s", path, gameID, fileext);

		if (CheckFile(filepath))
			continue;

		//! Not found add to missing list
		snprintf(gameID, sizeof(gameID), "%s", (char *)header->id);
		MissingFilesList.push_back(std::string(gameID));
	}

	if (dvdheader)
	{
		snprintf(gameID, sizeof(gameID), "%s", (char *)dvdheader->id);
		snprintf(filepath, sizeof(filepath), "%s/%s%s", path, gameID, fileext);

		if (!CheckFile(filepath)) {
			//! Not found, add missing dvd header to list
			MissingFilesList.push_back(std::string(gameID));
		}
	}

	//! Bring game list to the old state
	App.Library.Games.FilterList(oldFilter.c_str());

	gprintf(" = %i", MissingFilesList.size());

	return MissingFilesList.size();
}
