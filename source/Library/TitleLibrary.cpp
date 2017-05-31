#include "TitleLibrary.h"

TitleLibrary::TitleLibrary()
{
}

TitleLibrary::~TitleLibrary()
{

}

struct discHdr * TitleLibrary::GetDiscHeader(const char * gameID) const
{
	return Games.GetDiscHeader(gameID);
}

bool TitleLibrary::IsNew(const u8 *titleid) const
{
	return NewTitles.IsNew(titleid);
}

void TitleLibrary::CheckGame(const u8 *titleid)
{
	NewTitles.CheckGame(titleid);
}
