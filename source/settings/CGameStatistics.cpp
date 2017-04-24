#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CGameStatistics.h"
#include "FileOperations/fileops.h"
#include "Version.h"

CGameStatistics::CGameStatistics()
{
}

CGameStatistics::~CGameStatistics()
{
}

GameStatus * CGameStatistics::GetGameStatus(const char * id) const
{
	if (!id)
		return NULL;

	for (u32 i = 0; i < GameList.size(); ++i)
	{
		if (strncasecmp(id, GameList[i].id, 6) == 0)
		{
			return (GameStatus *)&GameList[i];
		}
	}

	return NULL;
}

bool CGameStatistics::AddGame(const GameStatus & NewGame)
{
	for (u32 i = 0; i < GameList.size(); ++i)
	{
		if (strncasecmp(NewGame.id, GameList[i].id, 6) == 0)
		{
			memcpy(&GameList[i], &NewGame, sizeof(GameStatus));
			return true;
		}
	}

	GameList.push_back(NewGame);

	return true;
}

bool CGameStatistics::RemoveAll()
{
	GameList.clear();
	std::vector<GameStatus>().swap(GameList);

	return Save();
}

bool CGameStatistics::Remove(const char * id)
{
	if (!id)
		return false;

	for (u32 i = 0; i < GameList.size(); ++i)
	{
		if (strncasecmp(id, GameList[i].id, 6) == 0)
		{
			GameList.erase(GameList.begin() + i);
			break;
		}
	}

	return true;
}

bool CGameStatistics::Load(const char * path)
{
	char line[1024];
	char filepath[300];
	snprintf(filepath, sizeof(filepath), "%sGXGameStatistics.cfg", path);

	ConfigPath = filepath;

	FILE *file = fopen(filepath, "r");
	if (!file) return false;

	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '#')
			continue;

		this->ParseLine(line);
	}
	fclose(file);

	return true;
}

bool CGameStatistics::Save()
{
	char filepath[300];
	strcpy(filepath, ConfigPath.c_str());

	char * ptr = strrchr(filepath, '/');
	if (ptr)
		ptr[0] = 0;

	if (!CreateSubfolder(filepath))
		return false;

	FILE * f = fopen(ConfigPath.c_str(), "w");
	if (!f) return false;

	fprintf(f, "# USB Loader GX R%s - Game statistics file\n", Version_GetPatch());
	fprintf(f, "# note: this file is automatically generated\n");
	fprintf(f, "# Num Games: %d\n", GameList.size());
	for (u32 i = 0; i < GameList.size(); i++)
	{
		fprintf(f, "game:%s = ", GameList[i].id);
		fprintf(f, "FavoriteRank:%d; ", GameList[i].FavoriteRank);
		fprintf(f, "PlayCount:%d;\n", GameList[i].PlayCount);
	}
	fprintf(f, "# END\n");
	fclose(f);

	return true;
}

bool CGameStatistics::SetSetting(GameStatus & game, char *name, char *value)
{
	int i = 0;

	if (strcmp(name, "FavoriteRank") == 0)
	{
		if (sscanf(value, "%d", &i) == 1)
		{
			game.FavoriteRank = i;
		}
		return true;
	}
	else if (strcmp(name, "PlayCount") == 0)
	{
		if (sscanf(value, "%d", &i) == 1)
		{
			game.PlayCount = i;
		}
		return true;
	}

	return false;
}

bool CGameStatistics::ReadGameID(const char * src, char * GameID, int size)
{
	if (strncasecmp(src, "game:", 5) != 0)
		return false;

	char * ptr = strchr(src, ':');
	if (!ptr)
		return false;

	ptr++;

	int i = 0;

	for (i = 0; i < size; i++, ptr++)
	{
		if (*ptr == ' ' || *ptr == '\0')
			break;

		GameID[i] = *ptr;
	}

	GameID[i] = 0;

	return true;
}

void CGameStatistics::ParseLine(char *line)
{
	char name[1024], value[1024];
	char GameID[8];

	if (!ReadGameID(line, GameID, 6))
		return;

	if (strlen(GameID) != 6 && strlen(GameID) != 4)
		return;

	GameStatus NewGame;
	memset(&NewGame, 0, sizeof(GameStatus));

	snprintf(NewGame.id, sizeof(NewGame.id), GameID);

	char * LinePtr = strchr(line, '=');

	while (LinePtr != NULL)
	{
		LinePtr++;

		char * eq = strchr(LinePtr, ':');

		if (!eq) break;

		this->TrimLine(name, LinePtr, sizeof(name));
		this->TrimLine(value, eq + 1, sizeof(value));

		SetSetting(NewGame, name, value);

		LinePtr = strchr(LinePtr, ';');
	}

	AddGame(NewGame);
}

void CGameStatistics::TrimLine(char *dest, const char *src, int size)
{
	while (*src == ' ')
		src++;

	int i = 0;

	for (i = 0; i < size; i++, src++)
	{
		if (*src == ';' || *src == ':' || *src == '\n' ||
			*src == '\r' || *src == '\0')
			break;

		dest[i] = *src;
	}

	dest[i] = '\0';
}

void CGameStatistics::SetPlayCount(const char * id, int count)
{
	if (!id)
		return;

	GameStatus NewStatus;
	snprintf(NewStatus.id, sizeof(NewStatus.id), id);
	NewStatus.FavoriteRank = 0;
	NewStatus.PlayCount = count;

	GameStatus * game = GetGameStatus(id);
	if (game)
	{
		NewStatus.FavoriteRank = game->FavoriteRank;
	}

	AddGame(NewStatus);
}

void CGameStatistics::SetFavoriteRank(const char * id, int rank)
{
	if (!id)
		return;

	GameStatus NewStatus;
	snprintf(NewStatus.id, sizeof(NewStatus.id), id);
	NewStatus.FavoriteRank = rank;
	NewStatus.PlayCount = 0;

	GameStatus * game = GetGameStatus(id);
	if (game)
	{
		NewStatus.PlayCount = game->PlayCount;
	}

	AddGame(NewStatus);
}

int CGameStatistics::GetPlayCount(const char * id) const
{
	if (!id)
		return 0;

	GameStatus * game = GetGameStatus(id);
	if (game)
		return game->PlayCount;

	return 0;
}

int CGameStatistics::GetFavoriteRank(const char * id) const
{
	if (!id)
		return 0;

	GameStatus * game = GetGameStatus(id);
	if (game)
		return game->FavoriteRank;

	return 0;
}
