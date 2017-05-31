#pragma once

#include <time.h>

class CNewTitles
{
protected:
	friend class TitleLibrary;
	CNewTitles();
	~CNewTitles();
	void Clean(void);
	void Reload(void);
	void CheckGame(const u8 *titleid);
	bool IsNew(const u8 *titleid) const;
	void Remove(const u8 *titleid);

	friend class GameList;
	void Save(void);

private:

	struct Title
	{
		char titleId[7];
		time_t timestamp;
		Title *next;
		bool isNew;
	};

	Title *firstTitle;
	Title *lastTitle;
	bool isDirty;
	bool isNewFile;
};
