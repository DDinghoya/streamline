#pragma once

#include <time.h>

class CNewTitles
{
public:
	//static CNewTitles *Instance() { if (!instance) instance = new CNewTitles(); return instance; }
	//static void DestroyInstance() { delete instance; instance = NULL; }

	void Save(void);
	void Clean(void);
	void Reload(void);
	void CheckGame(const u8 *titleid);
	bool IsNew(const u8 *titleid) const;
	void Remove(const u8 *titleid);

protected:
	friend class TitleLibrary;
	CNewTitles();
	~CNewTitles();

private:
	//static CNewTitles *instance;

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
