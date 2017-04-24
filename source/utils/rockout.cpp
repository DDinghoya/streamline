#include "GUI/gui.h"
#include "themes/CTheme.h"
#include "usbloader/GameList.h"
#include "App.h"
#include "menu/menus.h"

void rockout(struct discHdr *header)
{
	static bool rockoutSet = false;

	HaltGui();

	if (!rockoutSet && header &&
		header->id[0] != 'W' && header->id[0] != 'G' && // Exclude Wiiware and GameCube games
		(   strcasestr(App.Library.GameTitles.GetTitle(header), "guitar")
		 || strcasestr(App.Library.GameTitles.GetTitle(header), "band")
		 || strcasestr(App.Library.GameTitles.GetTitle(header), "rock")))
	{
		pointer[0]->SetImage("rplayer1_point.png");
		pointer[1]->SetImage("rplayer2_point.png");
		pointer[2]->SetImage("rplayer3_point.png");
		pointer[3]->SetImage("rplayer4_point.png");

		rockoutSet = true;
	}
	else if(rockoutSet)
	{
		pointer[0]->SetImage("player1_point.png");
		pointer[1]->SetImage("player2_point.png");
		pointer[2]->SetImage("player3_point.png");
		pointer[3]->SetImage("player4_point.png");

		rockoutSet = false;
	}
	ResumeGui();
}
