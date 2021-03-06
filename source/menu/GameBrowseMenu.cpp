#include <unistd.h>
#include "GameBrowseMenu.hpp"
#include "banner/BannerAsync.h"
#include "Controls/DeviceHandler.hpp"
#include "GUI/gui_gamelist.h"
#include "GUI/gui_gamegrid.h"
#include "GUI/gui_gamecarousel.h"
#include "GUI/GuiBannerGrid.h"
#include "GUI/LoadCoverImage.h"
#include "prompts/PromptWindows.h"
#include "prompts/gameinfo.h"
#include "prompts/DiscBrowser.h"
#include "prompts/GameWindow.hpp"
#include "prompts/BannerWindow.hpp"
#include "prompts/CategorySwitchPrompt.hpp"
#include "prompts/CheckboxPrompt.hpp"
#include "themes/CTheme.h"
#include "language/gettext.h"
#include "usbloader/wbfs.h"
#include "usbloader/wdvd.h"
#include "Net/networkops.h"
#include "Net/update.h"
#include "Net/ImageDownloader.h"
#include "App.h"
#include "SystemMenu/SystemMenuResources.h"
#include "system/IosLoader.h"
#include "utils/StringTools.h"
#include "utils/tools.h"
#include "utils/PasswordCheck.h"
#include "menus.h"
#include "Input/wpad.h"
#include "sys.h"
#include "video.h"
#include "Input/input.h"

extern bool updateavailable;

struct discHdr *dvdheader = NULL;
static bool Exiting = false;

GameBrowseMenu::GameBrowseMenu()
	: GuiWindow(screenwidth, screenheight)
{
	returnMenu = MENU_NONE;
	gameSelectedOld = -1;
	lastrawtime = 0;
	Exiting = false;
	show_searchwindow = false;
	gameBrowser = NULL;
	searchBar = NULL;
	gameCover = NULL;
	gameCoverImg = NULL;
	GameIDTxt = NULL;
	GameRegionTxt = NULL;
	listBackground = NULL;
	carouselBackground = NULL;
	gridBackground = NULL;
	WDVD_GetCoverStatus(&DiscDriveCoverOld);
	App.Library.Games.FilterList();
	HDDSizeCallback.SetCallback(this, &GameBrowseMenu::UpdateFreeSpace);

	btnInstall = App.Resources.GetImageData("button_install.png");
	btnInstallOver = App.Resources.GetImageData("button_install_over.png");
	btnSettings = App.Resources.GetImageData("settings_button.png");
	btnSettingsOver = App.Resources.GetImageData("settings_button_over.png");
	btnpwroff = App.Resources.GetImageData("wiimote_poweroff.png");
	btnpwroffOver = App.Resources.GetImageData("wiimote_poweroff_over.png");
	btnhome = App.Resources.GetImageData("menu_button.png");
	btnhomeOver = App.Resources.GetImageData("menu_button_over.png");
	btnsdcardOver = App.Resources.GetImageData("sdcard_over.png");
	btnsdcard = App.Resources.GetImageData("sdcard.png");

	imgfavIcon = App.Resources.GetImageData("favIcon.png");
	imgfavIcon_gray = App.Resources.GetImageData("favIcon_gray.png");
	imgsearchIcon = App.Resources.GetImageData("searchIcon.png");
	imgsearchIcon_gray = App.Resources.GetImageData("searchIcon_gray.png");
	imgabcIcon = App.Resources.GetImageData("abcIcon.png");
	imgrankIcon = App.Resources.GetImageData("rankIcon.png");
	imgplayCountIcon = App.Resources.GetImageData("playCountIcon.png");
	imgplayersSortIcon = App.Resources.GetImageData("playersSort.png");
	imgarrangeGrid = App.Resources.GetImageData("arrangeGrid.png");
	imgarrangeGrid_gray = App.Resources.GetImageData("arrangeGrid_gray.png");
	imgarrangeList = App.Resources.GetImageData("arrangeList.png");
	imgarrangeList_gray = App.Resources.GetImageData("arrangeList_gray.png");
	imgarrangeCarousel = App.Resources.GetImageData("arrangeCarousel.png");
	imgarrangeCarousel_gray = App.Resources.GetImageData("arrangeCarousel_gray.png");
	imgBannerGrid = App.Resources.GetImageData("arrangeBannerGrid.png");
	imgBannerGrid_gray = App.Resources.GetImageData("arrangeBannerGrid_gray.png");
	imgdvd = App.Resources.GetImageData("dvd.png");
	imgdvd_gray = App.Resources.GetImageData("dvd_gray.png");
	imgLock = App.Resources.GetImageData("lock.png");
	imgLock_gray = App.Resources.GetImageData("lock_gray.png");
	imgUnlock = App.Resources.GetImageData("unlock.png");
	imgUnlock_gray = App.Resources.GetImageData("unlock_gray.png");
	imgCategory = App.Resources.GetImageData("category.png");
	imgCategory_gray = App.Resources.GetImageData("category_gray.png");
	imgLoaderMode = App.Resources.GetImageData("loader_mode.png");

	homebrewImgData = App.Resources.GetImageData("browser.png");
	homebrewImgDataOver = App.Resources.GetImageData("browser_over.png");

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigHome = new GuiTrigger;
	trigHome->SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, PAD_BUTTON_START);
	trig2 = new GuiTrigger;
	trig2->SetButtonOnlyTrigger(-1, WPAD_BUTTON_2 | WPAD_CLASSIC_BUTTON_X, PAD_BUTTON_Y);
	trig1 = new GuiTrigger;
	trig1->SetButtonOnlyTrigger(-1, WPAD_BUTTON_1 | WPAD_CLASSIC_BUTTON_Y, PAD_BUTTON_X);

	usedSpaceTxt = new GuiText(" ", 18, thColor("r=55 g=190 b=237 a=255 - hdd info color"));
	usedSpaceTxt->SetAlignment(thAlign("center - hdd info align hor"), thAlign("top - hdd info align ver"));
	usedSpaceTxt->SetPosition(thInt("0 - hdd info pos x"), thInt("400 - hdd info pos y"));

	gamecntTxt = new GuiText((char *) NULL, 18, thColor("r=55 g=190 b=237 a=255 - game count color"));
	gamecntBtn = new GuiButton(100, 18);
	gamecntBtn->SetAlignment(thAlign("center - game count align hor"), thAlign("top - game count align ver"));
	gamecntBtn->SetPosition(thInt("0 - game count pos x"), thInt("420 - game count pos y"));
	gamecntBtn->SetLabel(gamecntTxt);
	gamecntBtn->SetEffectGrow();
	gamecntBtn->SetTrigger(trigA);

	installBtnTT = new GuiTooltip(tr( "Install a game" ));
	if (App.Settings.wsprompt) installBtnTT->SetWidescreen(App.Settings.widescreen);
	installBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	installBtnImg = new GuiImage(btnInstall);
	installBtnImgOver = new GuiImage(btnInstallOver);
	installBtnImg->SetWidescreen(App.Settings.widescreen);
	installBtnImgOver->SetWidescreen(App.Settings.widescreen);

	installBtn = new GuiButton(installBtnImg, installBtnImgOver, ALIGN_LEFT, ALIGN_TOP,
							   thInt("16 - install btn pos x"), thInt("355 - install btn pos y"),
							   trigA, btnSoundOver, btnSoundClick2, 1, installBtnTT, 24, -30, 0, 5);

	settingsBtnTT = new GuiTooltip(tr( "Settings" ));
	if (App.Settings.wsprompt) settingsBtnTT->SetWidescreen(App.Settings.widescreen);
	settingsBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	settingsBtnImg = new GuiImage(btnSettings);
	settingsBtnImg->SetWidescreen(App.Settings.widescreen);
	settingsBtnImgOver = new GuiImage(btnSettingsOver);
	settingsBtnImgOver->SetWidescreen(App.Settings.widescreen);
	settingsBtn = new GuiButton(settingsBtnImg, settingsBtnImgOver, 0, 3,
								thInt("64 - settings btn pos x"), thInt("371 - settings btn pos y"),
								trigA, btnSoundOver, btnSoundClick2, 1, settingsBtnTT, 65, -30, 0, 5);

	homeBtnTT = new GuiTooltip(tr( "Back to HBC or Wii Menu" ));
	if (App.Settings.wsprompt) homeBtnTT->SetWidescreen(App.Settings.widescreen);
	homeBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	homeBtnImg = new GuiImage(btnhome);
	homeBtnImg->SetWidescreen(App.Settings.widescreen);
	homeBtnImgOver = new GuiImage(btnhomeOver);
	homeBtnImgOver->SetWidescreen(App.Settings.widescreen);
	homeBtn = new GuiButton(homeBtnImg, homeBtnImgOver, 0, 3,
							thInt("489 - home menu btn pos x"), thInt("371 - home menu btn pos y"),
							trigA, btnSoundOver, btnSoundClick2, 1, homeBtnTT, 15, -30, 1, 5);
	homeBtn->RemoveSoundClick();
	homeBtn->SetTrigger(trigHome);

	poweroffBtnTT = new GuiTooltip(tr( "Power off the Wii" ));
	if (App.Settings.wsprompt) poweroffBtnTT->SetWidescreen(App.Settings.widescreen);
	poweroffBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	poweroffBtnImg = new GuiImage(btnpwroff);
	poweroffBtnImgOver = new GuiImage(btnpwroffOver);
	poweroffBtnImg->SetWidescreen(App.Settings.widescreen);
	poweroffBtnImgOver->SetWidescreen(App.Settings.widescreen);
	poweroffBtn = new GuiButton(poweroffBtnImg, poweroffBtnImgOver, 0, 3,
								thInt("576 - power off btn pos x"), thInt("355 - power off btn pos y"),
								trigA, btnSoundOver, btnSoundClick2, 1, poweroffBtnTT, -10, -30, 1, 5);

	sdcardBtnTT = new GuiTooltip(tr( "Reload SD" ));
	if (App.Settings.wsprompt) sdcardBtnTT->SetWidescreen(App.Settings.widescreen);
	sdcardBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	sdcardImg = new GuiImage(btnsdcard);
	sdcardImgOver = new GuiImage(btnsdcardOver);
	sdcardImg->SetWidescreen(App.Settings.widescreen);
	sdcardImgOver->SetWidescreen(App.Settings.widescreen);
	sdcardBtn = new GuiButton(sdcardImg, sdcardImgOver, 0, 3,
							  thInt("160 - sd card btn pos x"), thInt("395 - sd card btn pos y"),
							  trigA, btnSoundOver, btnSoundClick2, 1, sdcardBtnTT, 15, -30, 0, 5);

	gameInfo = new GuiButton(0, 0);
	gameInfo->SetTrigger(trig2);
	gameInfo->SetSoundClick(btnSoundClick2);

	favoriteBtnTT = new GuiTooltip(tr( "Display favorites only" ));
	if (App.Settings.wsprompt) favoriteBtnTT->SetWidescreen(App.Settings.widescreen);
	favoriteBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	favoriteBtnImg = new GuiImage(imgfavIcon);
	favoriteBtnImg->SetWidescreen(App.Settings.widescreen);
	favoriteBtnImg_g = new GuiImage(imgfavIcon_gray);
	favoriteBtnImg_g->SetWidescreen(App.Settings.widescreen);
	favoriteBtn = new GuiButton(favoriteBtnImg_g, favoriteBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0,
								trigA, btnSoundOver, btnSoundClick2, 1, favoriteBtnTT, -15, 52, 0, 3);
	favoriteBtn->SetSelectable(false);

	searchBtnTT = new GuiTooltip(tr( "Set Search-Filter" ));
	if (App.Settings.wsprompt) searchBtnTT->SetWidescreen(App.Settings.widescreen);
	searchBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	searchBtnImg = new GuiImage(imgsearchIcon);
	searchBtnImg->SetWidescreen(App.Settings.widescreen);
	searchBtnImg_g = new GuiImage(imgsearchIcon_gray);
	searchBtnImg_g->SetWidescreen(App.Settings.widescreen);
	searchBtn = new GuiButton(searchBtnImg_g, searchBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0,
							  trigA, btnSoundOver, btnSoundClick2, 1, searchBtnTT, -15, 52, 0, 3);
	searchBtn->SetSelectable(false);

	sortBtnTT = new GuiTooltip(" ");
	if (App.Settings.wsprompt) sortBtnTT->SetWidescreen(App.Settings.widescreen);
	sortBtnTT->SetAlpha(thInt("255 - tooltip alpha"));

	sortBtnImg = new GuiImage(imgabcIcon);
	sortBtnImg->SetWidescreen(App.Settings.widescreen);
	sortBtn = new GuiButton(sortBtnImg, sortBtnImg, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, sortBtnTT, -15, 52, 0, 3);
	sortBtn->SetSelectable(false);

	loaderModeBtnTT = new GuiTooltip(tr("Select loader mode"));
	if (App.Settings.wsprompt) loaderModeBtnTT->SetWidescreen(App.Settings.widescreen);
	loaderModeBtnTT->SetAlpha(thInt("255 - tooltip alpha"));

	loaderModeBtnImg = new GuiImage(imgLoaderMode);
	loaderModeBtnImg->SetWidescreen(App.Settings.widescreen);
	loaderModeBtn = new GuiButton(loaderModeBtnImg, loaderModeBtnImg, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, loaderModeBtnTT, -15, 52, 0, 3);
	loaderModeBtn->SetSelectable(false);

	categBtnTT = new GuiTooltip(tr("Select game categories"));
	if (App.Settings.wsprompt) categBtnTT->SetWidescreen(App.Settings.widescreen);
	categBtnTT->SetAlpha(thInt("255 - tooltip alpha"));

	categBtnImg = new GuiImage(imgCategory);
	categBtnImg->SetWidescreen(App.Settings.widescreen);
	categBtnImg_g = new GuiImage(imgCategory_gray);
	categBtnImg_g->SetWidescreen(App.Settings.widescreen);
	categBtn = new GuiButton(categBtnImg, categBtnImg, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, categBtnTT, -15, 52, 0, 3);
	categBtn->SetSelectable(false);

	listBtnTT = new GuiTooltip(tr( "Display as a list" ));
	if (App.Settings.wsprompt) listBtnTT->SetWidescreen(App.Settings.widescreen);
	listBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	listBtnImg = new GuiImage(imgarrangeList);
	listBtnImg->SetWidescreen(App.Settings.widescreen);
	listBtnImg_g = new GuiImage(imgarrangeList_gray);
	listBtnImg_g->SetWidescreen(App.Settings.widescreen);
	listBtn = new GuiButton(listBtnImg_g, listBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, listBtnTT, 15, 52, 1, 3);
	listBtn->SetSelectable(false);

	gridBtnTT = new GuiTooltip(tr( "Display as a grid" ));
	if (App.Settings.wsprompt) gridBtnTT->SetWidescreen(App.Settings.widescreen);
	gridBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	gridBtnImg = new GuiImage(imgarrangeGrid);
	gridBtnImg->SetWidescreen(App.Settings.widescreen);
	gridBtnImg_g = new GuiImage(imgarrangeGrid_gray);
	gridBtnImg_g->SetWidescreen(App.Settings.widescreen);
	gridBtn = new GuiButton(gridBtnImg_g, gridBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, gridBtnTT, 15, 52, 1, 3);
	gridBtn->SetSelectable(false);

	carouselBtnTT = new GuiTooltip(tr( "Display as a carousel" ));
	if (App.Settings.wsprompt) carouselBtnTT->SetWidescreen(App.Settings.widescreen);
	carouselBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	carouselBtnImg = new GuiImage(imgarrangeCarousel);
	carouselBtnImg->SetWidescreen(App.Settings.widescreen);
	carouselBtnImg_g = new GuiImage(imgarrangeCarousel_gray);
	carouselBtnImg_g->SetWidescreen(App.Settings.widescreen);
	carouselBtn = new GuiButton(carouselBtnImg_g, carouselBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, carouselBtnTT, 15, 52, 1, 3);
	carouselBtn->SetSelectable(false);

	bannerGridBtnTT = new GuiTooltip(tr( "Display as a channel grid" ));
	if (App.Settings.wsprompt) bannerGridBtnTT->SetWidescreen(App.Settings.widescreen);
	bannerGridBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	bannerGridBtnImg = new GuiImage(imgBannerGrid);
	bannerGridBtnImg->SetWidescreen(App.Settings.widescreen);
	bannerGridBtnImg_g = new GuiImage(imgBannerGrid_gray);
	bannerGridBtnImg_g->SetWidescreen(App.Settings.widescreen);
	bannerGridBtn = new GuiButton(bannerGridBtnImg_g, bannerGridBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, bannerGridBtnTT, 15, 52, 1, 3);
	bannerGridBtn->SetSelectable(false);

	lockBtnTT = new GuiTooltip(NULL);
	if (App.Settings.wsprompt) lockBtnTT->SetWidescreen(App.Settings.widescreen);
	lockBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	lockBtnImg = new GuiImage(imgLock);
	lockBtnImg->SetWidescreen(App.Settings.widescreen);
	lockBtnImg_g = new GuiImage(imgLock_gray);
	lockBtnImg_g->SetWidescreen(App.Settings.widescreen);
	lockBtn = new GuiButton(lockBtnImg_g, lockBtnImg_g, ALIGN_LEFT, ALIGN_TOP, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1, lockBtnTT, 15, 52, 1, 3);
	lockBtn->SetSelectable(false);

	unlockBtnImg = new GuiImage(imgUnlock);
	unlockBtnImg->SetWidescreen(App.Settings.widescreen);
	unlockBtnImg_g = new GuiImage(imgUnlock_gray);
	unlockBtnImg_g->SetWidescreen(App.Settings.widescreen);

	dvdBtnTT = new GuiTooltip(tr( "Mount DVD drive" ));
	if (App.Settings.wsprompt) dvdBtnTT->SetWidescreen(App.Settings.widescreen);
	dvdBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	dvdBtnImg = new GuiImage(imgdvd);
	dvdBtnImg->SetWidescreen(App.Settings.widescreen);
	dvdBtnImg_g = new GuiImage(imgdvd_gray);
	dvdBtnImg_g->SetWidescreen(App.Settings.widescreen);
	dvdBtn = new GuiButton(dvdBtnImg_g, 0, ALIGN_LEFT, ALIGN_TOP, 0, 0,
						   trigA, btnSoundOver, btnSoundClick2, 1, dvdBtnTT, 15, 52, 1, 3);
	dvdBtn->SetSelectable(false);

	homebrewBtnTT = new GuiTooltip(tr( "Homebrew Launcher" ));
	if (App.Settings.wsprompt) homebrewBtnTT->SetWidescreen(App.Settings.widescreen);
	homebrewBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	homebrewImg = new GuiImage(homebrewImgData);
	homebrewImgOver = new GuiImage(homebrewImgDataOver);
	homebrewImg->SetWidescreen(App.Settings.widescreen);
	homebrewImgOver->SetWidescreen(App.Settings.widescreen);
	homebrewBtn = new GuiButton(homebrewImg, homebrewImgOver, ALIGN_LEFT, ALIGN_TOP, thInt("410 - HBC btn pos x"), thInt("405 - HBC btn pos y"),
								trigA, btnSoundOver, btnSoundClick2, 1, homebrewBtnTT, 15, -30, 1, 5);
	//Downloading Covers
	DownloadBtnTT = new GuiTooltip(tr( "Click to Download Covers" ));
	if (App.Settings.wsprompt) DownloadBtnTT->SetWidescreen(App.Settings.widescreen);
	DownloadBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	DownloadBtn = new GuiButton (0, 0);
	DownloadBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	DownloadBtn->SetPosition(thInt("26 - cover/download btn pos x"), thInt("58 - cover/download btn pos y"));
	DownloadBtn->SetSoundOver(btnSoundOver);
	DownloadBtn->SetTrigger(0, trigA);
	DownloadBtn->SetTrigger(1, trig1);
	DownloadBtn->SetToolTip(DownloadBtnTT, 205, -30);
	DownloadBtn->SetSelectable(false);

	gameCoverImg = new GuiImage();
	gameCoverImg->SetPosition(thInt("26 - cover/download btn pos x"), thInt("58 - cover/download btn pos y"));
	gameCoverImg->SetWidescreen(App.Settings.widescreen);

	IDBtnTT = new GuiTooltip(tr( "Click to change game ID" ));
	if (App.Settings.wsprompt) IDBtnTT->SetWidescreen(App.Settings.widescreen);
	IDBtnTT->SetAlpha(thInt("255 - tooltip alpha"));
	idBtn = new GuiButton(60, 23);
	idBtn->SetPosition(thInt("68 - gameID btn pos x"), thInt("305 - gameID btn pos y"));
	idBtn->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	idBtn->SetSoundOver(btnSoundOver);
	idBtn->SetTrigger(0, trigA);
	idBtn->SetToolTip(IDBtnTT, 205, -30);
	idBtn->SetSelectable(false);

	GXColor clockColor = thColor("r=138 g=138 b=138 a=240 - clock color");
	float clockFontScaleFactor = thFloat("1.0 - Overrided clock scale factor. 1.0=allow user setting") != 1.0f ? thFloat("1.0 - Overrided clock scale factor. 1.0=allow user setting") : App.Settings.ClockFontScaleFactor;
	clockTimeBack = new GuiText("88:88", 40 / App.Settings.FontScaleFactor * clockFontScaleFactor, (GXColor) {clockColor.r, clockColor.g, clockColor.b, (u8)(clockColor.a / 6)});
	clockTimeBack->SetAlignment(thAlign("left - clock align hor"), thAlign("top - clock align ver"));
	clockTimeBack->SetPosition(thInt("275 - clock pos x"), thInt("335 - clock pos y"));
	clockTimeBack->SetFont(App.Resources.GetFile("clock.ttf"), App.Resources.GetFileSize("clock.ttf"));

	clockTime = new GuiText("", 40 / App.Settings.FontScaleFactor * clockFontScaleFactor, clockColor);
	clockTime->SetAlignment(thAlign("left - clock align hor"), thAlign("top - clock align ver"));
	clockTime->SetPosition(thInt("275 - clock pos x"), thInt("335 - clock pos y"));
	clockTime->SetFont(App.Resources.GetFile("clock.ttf"), App.Resources.GetFileSize("clock.ttf"));

	ToolBar.push_back(favoriteBtn);
	ToolBar.push_back(searchBtn);
	ToolBar.push_back(sortBtn);
	ToolBar.push_back(categBtn);
	ToolBar.push_back(listBtn);
	ToolBar.push_back(gridBtn);
	ToolBar.push_back(loaderModeBtn);
	ToolBar.push_back(carouselBtn);
	ToolBar.push_back(bannerGridBtn);
	ToolBar.push_back(lockBtn);
	ToolBar.push_back(dvdBtn);
	SetUpdateCallback(UpdateCallback);

	ReloadBrowser();
}

GameBrowseMenu::~GameBrowseMenu()
{
	Exiting = true;
	ResumeGui();

	SetEffect(EFFECT_FADE, -20);
	while(parentElement && this->GetEffect() > 0) usleep(100);

	HaltGui();
	if(parentElement)
		((GuiWindow *) parentElement)->Remove(this);

	RemoveAll();

	//! Reset optional background image
	bgImg->SetImage(background);

	delete listBackground;
	delete carouselBackground;
	delete gridBackground;
	delete btnInstall;
	delete btnInstallOver;
	delete btnSettings;
	delete btnSettingsOver;
	delete btnpwroff;
	delete btnpwroffOver;
	delete btnhome;
	delete btnhomeOver;
	delete btnsdcardOver;
	delete btnsdcard;
	delete imgfavIcon;
	delete imgfavIcon_gray;
	delete imgsearchIcon;
	delete imgsearchIcon_gray;
	delete imgabcIcon;
	delete imgrankIcon;
	delete imgplayCountIcon;
	delete imgplayersSortIcon;
	delete imgarrangeGrid;
	delete imgarrangeGrid_gray;
	delete imgarrangeCarousel;
	delete imgarrangeCarousel_gray;
	delete imgBannerGrid;
	delete imgBannerGrid_gray;
	delete imgarrangeList;
	delete imgarrangeList_gray;
	delete imgdvd;
	delete imgdvd_gray;
	delete imgLock;
	delete imgLock_gray;
	delete imgUnlock;
	delete imgUnlock_gray;
	delete imgCategory;
	delete imgCategory_gray;
	delete imgLoaderMode;
	delete homebrewImgData;
	delete homebrewImgDataOver;
	delete gameCover;

	delete trigA;
	delete trigHome;
	delete trig1;
	delete trig2;

	delete installBtnImg;
	delete installBtnImgOver;
	delete settingsBtnImg;
	delete settingsBtnImgOver;
	delete homeBtnImg;
	delete homeBtnImgOver;
	delete poweroffBtnImg;
	delete poweroffBtnImgOver;
	delete sdcardImg;
	delete sdcardImgOver;
	delete favoriteBtnImg;
	delete favoriteBtnImg_g;
	delete searchBtnImg;
	delete searchBtnImg_g;
	delete sortBtnImg;
	delete listBtnImg;
	delete listBtnImg_g;
	delete gridBtnImg;
	delete gridBtnImg_g;
	delete carouselBtnImg;
	delete carouselBtnImg_g;
	delete bannerGridBtnImg;
	delete bannerGridBtnImg_g;
	delete lockBtnImg;
	delete lockBtnImg_g;
	delete unlockBtnImg;
	delete unlockBtnImg_g;
	delete dvdBtnImg;
	delete dvdBtnImg_g;
	delete categBtnImg;
	delete categBtnImg_g;
	delete loaderModeBtnImg;
	delete homebrewImg;
	delete homebrewImgOver;
	delete gameCoverImg;

	delete GameIDTxt;
	delete GameRegionTxt;
	delete usedSpaceTxt;
	delete gamecntTxt;
	delete clockTimeBack;
	delete clockTime;

	delete gamecntBtn;
	delete installBtn;
	delete settingsBtn;
	delete homeBtn;
	delete poweroffBtn;
	delete sdcardBtn;
	delete gameInfo;
	delete favoriteBtn;
	delete searchBtn;
	delete sortBtn;
	delete listBtn;
	delete gridBtn;
	delete carouselBtn;
	delete bannerGridBtn;
	delete lockBtn;
	delete dvdBtn;
	delete categBtn;
	delete loaderModeBtn;
	delete homebrewBtn;
	delete DownloadBtn;
	delete idBtn;

	delete installBtnTT;
	delete settingsBtnTT;
	delete homeBtnTT;
	delete poweroffBtnTT;
	delete sdcardBtnTT;
	delete favoriteBtnTT;
	delete searchBtnTT;
	delete sortBtnTT;
	delete listBtnTT;
	delete gridBtnTT;
	delete carouselBtnTT;
	delete bannerGridBtnTT;
	delete lockBtnTT;
	delete dvdBtnTT;
	delete categBtnTT;
	delete loaderModeBtnTT;
	delete homebrewBtnTT;
	delete DownloadBtnTT;
	delete IDBtnTT;

	delete gameBrowser;
	App.MainWindow->Remove(searchBar);
	delete searchBar;

	ResumeGui();
}

int GameBrowseMenu::Execute()
{
	int retMenu = MENU_NONE;

	GameBrowseMenu * Menu = new GameBrowseMenu();
	App.MainWindow->Append(Menu);

	if(App.Settings.ShowFreeSpace)
	{
		ThreadedTask::Instance()->AddCallback(&Menu->HDDSizeCallback);
		ThreadedTask::Instance()->Execute();
	}

	while(retMenu == MENU_NONE)
	{
		usleep(50000);

		if (shutdown)
			Sys_Shutdown();
		if (reset)
			Sys_Reboot();

		retMenu = Menu->MainLoop();
	}

	delete Menu;

	return retMenu;
}

void GameBrowseMenu::ReloadBrowser()
{
	ResumeGui();

	SetEffect(EFFECT_FADE, -40);
	while(parentElement && this->GetEffect() > 0) usleep(100);

	HaltGui();
	RemoveAll();
	App.MainWindow->Remove(searchBar);

	gamecntTxt->SetText(fmt("%s: %i", tr( "Games" ), App.Library.Games.size()));

	const char * sortTTText = NULL;
	GuiImageData * sortImgData = NULL;

	if(App.Settings.GameSort & SORT_RANKING)
	{
		sortTTText = tr( "Sort by rank" );
		sortImgData = imgrankIcon;
	}
	else if(App.Settings.GameSort & SORT_PLAYCOUNT)
	{
		sortTTText = tr( "Sort order by most played");
		sortImgData = imgplayCountIcon;
	}
	else if(App.Settings.GameSort & SORT_PLAYERS)
	{
		sortTTText = tr( "Sort by number of players");
		sortImgData = imgplayersSortIcon;
	}
	else
	{
		sortTTText = tr("Sort alphabetically");
		sortImgData = imgabcIcon;
	}

	sortBtnTT->SetText(sortTTText);
	sortBtnImg->SetImage(sortImgData);

	if(DiscDriveCoverOld & 0x02)
		dvdBtn->SetImage(dvdBtnImg);
	else
		dvdBtn->SetImage(dvdBtnImg_g);

	if (App.Settings.GameSort & SORT_FAVORITE)
	{
		favoriteBtn->SetImage(favoriteBtnImg);
		favoriteBtn->SetImageOver(favoriteBtnImg);
	}
	else
	{
		favoriteBtn->SetImage(favoriteBtnImg_g);
		favoriteBtn->SetImageOver(favoriteBtnImg_g);
	}

	if (*App.Library.Games.GetCurrentFilter())
	{
		if (!show_searchwindow) searchBtn->SetEffect(EFFECT_PULSE, 10, 105);
		searchBtn->SetImage(searchBtnImg);
		searchBtn->SetImageOver(searchBtnImg);
	}
	else if(!show_searchwindow)
	{
		searchBtn->SetImage(searchBtnImg_g);
		searchBtn->SetImageOver(searchBtnImg_g);
	}

	if (App.Settings.godmode)
	{
		GuiImage * unlockImage = strcmp(App.Settings.unlockCode, "") == 0 ? unlockBtnImg_g : unlockBtnImg;
		lockBtn->SetImage(unlockImage);
		lockBtn->SetImageOver(unlockImage);
		lockBtnTT->SetText(tr( "Lock USB Loader GX" ));
	}
	else
	{
		lockBtn->SetImage(lockBtnImg);
		lockBtn->SetImageOver(lockBtnImg);
		lockBtnTT->SetText(tr( "Unlock USB Loader GX" ));
	}

	categBtn->SetImage(categBtnImg);
	for(u32 n = 0; n < App.Settings.EnabledCategories.size(); ++n)
	{
		if(App.Settings.EnabledCategories[n] == 0)
		{
			categBtn->SetImage(categBtnImg_g);
			break;
		}
	}

	//! Check if the loaded setting is still in range
	App.Settings.SelectedGame = LIMIT(App.Settings.SelectedGame, 0, App.Library.Games.size()-1);
	App.Settings.GameListOffset = LIMIT(App.Settings.GameListOffset, 0, App.Library.Games.size()-1);

	delete gameBrowser;
	delete searchBar;
	gameBrowser = NULL;
	searchBar = NULL;

	if (App.Settings.gameDisplay == LIST_MODE)
	{
		//! only one image, reload it since it won't be changeable later
		if(App.Library.Games.size() == 1)
			LoadCover(App.Library.Games[0]);
		if(App.Library.Games.size() > 0)
			Append(gameCoverImg);
		DownloadBtn->SetSize(160, 224);
		listBtn->SetImage(listBtnImg);
		listBtn->SetImageOver(listBtnImg);
		gridBtn->SetImage(gridBtnImg_g);
		gridBtn->SetImageOver(gridBtnImg_g);
		carouselBtn->SetImage(carouselBtnImg_g);
		carouselBtn->SetImageOver(carouselBtnImg_g);
		bannerGridBtn->SetImage(bannerGridBtnImg_g);
		bannerGridBtn->SetImageOver(bannerGridBtnImg_g);

		favoriteBtn->SetPosition(App.Settings.widescreen ? thInt("214 - list layout favorite btn pos x widescreen") : thInt("168 - list layout favorite btn pos x"),
								thInt("13 - list layout favorite btn pos y"));
		searchBtn->SetPosition(App.Settings.widescreen ? thInt("246 - list layout search btn pos x widescreen") : thInt("208 - list layout search btn pos x"),
								thInt("13 - list layout search btn pos y"));
		sortBtn->SetPosition(App.Settings.widescreen ? thInt("278 - list layout abc/sort btn pos x widescreen") : thInt("248 - list layout abc/sort btn pos x"),
								thInt("13 - list layout abc/sort btn pos y"));
		loaderModeBtn->SetPosition(App.Settings.widescreen ? thInt("310 - list layout loadermode btn pos x widescreen") : thInt("288 - list layout loadermode btn pos x"),
								thInt("13 - list layout loadermode btn pos y"));
		categBtn->SetPosition(App.Settings.widescreen ? thInt("342 - list layout category btn pos x widescreen") : thInt("328 - list layout category btn pos x"),
								thInt("13 - list layout category btn pos y"));
		listBtn->SetPosition(App.Settings.widescreen ? thInt("374 - list layout list btn pos x widescreen") : thInt("368 - list layout list btn pos x"),
								thInt("13 - list layout list btn pos y"));
		gridBtn->SetPosition(App.Settings.widescreen ? thInt("406 - list layout grid btn pos x widescreen") : thInt("408 - list layout grid btn pos x"),
								thInt("13 - list layout grid btn pos y"));
		carouselBtn->SetPosition(App.Settings.widescreen ? thInt("438 - list layout carousel btn pos x widescreen") : thInt("448 - list layout carousel btn pos x"),
								thInt("13 - list layout carousel btn pos y"));
		bannerGridBtn->SetPosition(App.Settings.widescreen ? thInt("470 - list layout bannergrid btn pos x widescreen") : thInt("488 - list layout bannergrid btn pos x"),
								thInt("13 - list layout bannergrid btn pos y"));
		lockBtn->SetPosition(App.Settings.widescreen ? thInt("502 - list layout lock btn pos x widescreen") : thInt("528 - list layout lock btn pos x"),
								thInt("13 - list layout lock btn pos y"));
		dvdBtn->SetPosition(App.Settings.widescreen ? thInt("534 - list layout dvd btn pos x widescreen") : thInt("568 - list layout dvd btn pos x"),
								thInt("13 - list layout dvd btn pos y"));

		gameBrowser = new GuiGameList(thInt("396 - game list layout width"), thInt("280 - game list layout height"), App.Settings.GameListOffset);
		gameBrowser->SetPosition(thInt("200 - game list layout pos x"), thInt("49 - game list layout pos y"));
		gameBrowser->SetAlignment(ALIGN_LEFT, ALIGN_CENTER);
		gameBrowser->SetSelectedOption(App.Settings.SelectedGame);

		//! Setup optional background image
		if(!listBackground)
			listBackground = App.Resources.GetImageData("listBackground.png");
		if(listBackground)
			bgImg->SetImage(listBackground);
		else
			bgImg->SetImage(background);
	}
	else if (App.Settings.gameDisplay == GRID_MODE)
	{
		DownloadBtn->SetSize(0, 0);
		UpdateGameInfoText(NULL);
		gridBtn->SetImage(gridBtnImg);
		gridBtn->SetImageOver(gridBtnImg);
		listBtn->SetImage(listBtnImg_g);
		listBtn->SetImageOver(listBtnImg_g);
		carouselBtn->SetImage(carouselBtnImg_g);
		carouselBtn->SetImageOver(carouselBtnImg_g);
		bannerGridBtn->SetImage(bannerGridBtnImg_g);
		bannerGridBtn->SetImageOver(bannerGridBtnImg_g);

		favoriteBtn->SetPosition(App.Settings.widescreen ? thInt("144 - grid layout favorite btn pos x widescreen") : thInt("100 - grid layout favorite btn pos x"),
								thInt("13 - grid layout favorite btn pos y"));
		searchBtn->SetPosition(App.Settings.widescreen ? thInt("176 - grid layout search btn pos x widescreen") : thInt("140 - grid layout search btn pos x"),
								thInt("13 - grid layout search btn pos y"));
		sortBtn->SetPosition(App.Settings.widescreen ? thInt("208 - grid layout abc/sort btn pos x widescreen") : thInt("180 - grid layout abc/sort btn pos x"),
								thInt("13 - grid layout abc/sort btn pos y"));
		loaderModeBtn->SetPosition(App.Settings.widescreen ? thInt("240 - grid layout loadermode btn pos x widescreen") : thInt("220 - grid layout loadermode btn pos x"),
								thInt("13 - grid layout loadermode btn pos y"));
		categBtn->SetPosition(App.Settings.widescreen ? thInt("272 - grid layout category btn pos x widescreen") : thInt("260 - grid layout category btn pos x"),
								thInt("13 - grid layout category btn pos y"));
		listBtn->SetPosition(App.Settings.widescreen ? thInt("304 - grid layout list btn pos x widescreen") : thInt("300 - grid layout list btn pos x"),
								thInt("13 - grid layout list btn pos y"));
		gridBtn->SetPosition(App.Settings.widescreen ? thInt("336 - grid layout grid btn pos x widescreen") : thInt("340 - grid layout grid btn pos x"),
								thInt("13 - grid layout grid btn pos y"));
		carouselBtn->SetPosition(App.Settings.widescreen ? thInt("368 - grid layout carousel btn pos x widescreen") : thInt("380 - grid layout carousel btn pos x"),
								thInt("13 - grid layout carousel btn pos y"));
		bannerGridBtn->SetPosition(App.Settings.widescreen ? thInt("400 - grid layout bannergrid btn pos x widescreen") : thInt("420 - grid layout bannergrid btn pos x"),
								thInt("13 - grid layout bannergrid btn pos y"));
		lockBtn->SetPosition(App.Settings.widescreen ? thInt("432 - grid layout lock btn pos x widescreen") : thInt("460 - grid layout lock btn pos x"),
								thInt("13 - grid layout lock btn pos y"));
		dvdBtn->SetPosition(App.Settings.widescreen ? thInt("464 - grid layout dvd btn pos x widescreen") : thInt("500 - grid layout dvd btn pos x"),
								thInt("13 - grid layout dvd btn pos y"));

		gameBrowser = new GuiGameGrid(thInt("640 - game grid layout width"), thInt("400 - game grid layout height"), App.Settings.theme_path, App.Settings.GameListOffset);
		gameBrowser->SetPosition(thInt("0 - game grid layout pos x"), thInt("20 - game grid layout pos y"));
		gameBrowser->SetAlignment(ALIGN_LEFT, ALIGN_CENTER);

		//! Setup optional background image
		if(!gridBackground)
			gridBackground = App.Resources.GetImageData("gridBackground.png");
		if(gridBackground)
			bgImg->SetImage(gridBackground);
		else
			bgImg->SetImage(background);
	}
	else if (App.Settings.gameDisplay == CAROUSEL_MODE)
	{
		DownloadBtn->SetSize(0, 0);
		UpdateGameInfoText(NULL);
		carouselBtn->SetImage(carouselBtnImg);
		carouselBtn->SetImageOver(carouselBtnImg);
		listBtn->SetImage(listBtnImg_g);
		listBtn->SetImageOver(listBtnImg_g);
		gridBtn->SetImage(gridBtnImg_g);
		gridBtn->SetImageOver(gridBtnImg_g);
		bannerGridBtn->SetImage(bannerGridBtnImg_g);
		bannerGridBtn->SetImageOver(bannerGridBtnImg_g);

		favoriteBtn->SetPosition(App.Settings.widescreen ? thInt("144 - carousel layout favorite btn pos x widescreen") : thInt("100 - carousel layout favorite btn pos x"),
								thInt("13 - carousel layout favorite btn pos y"));
		searchBtn->SetPosition(App.Settings.widescreen ? thInt("176 - carousel layout search btn pos x widescreen") : thInt("140 - carousel layout search btn pos x"),
								thInt("13 - carousel layout search btn pos y"));
		sortBtn->SetPosition(App.Settings.widescreen ? thInt("208 - carousel layout abc/sort btn pos x widescreen") : thInt("180 - carousel layout abc/sort btn pos x"),
								thInt("13 - carousel layout abc/sort btn pos y"));
		loaderModeBtn->SetPosition(App.Settings.widescreen ? thInt("240 - carousel layout loadermode btn pos x widescreen") : thInt("220 - carousel layout loadermode btn pos x"),
								thInt("13 - carousel layout loadermode btn pos y"));
		categBtn->SetPosition(App.Settings.widescreen ? thInt("272 - carousel layout category btn pos x widescreen") : thInt("260 - carousel layout category btn pos x"),
								thInt("13 - carousel layout category btn pos y"));
		listBtn->SetPosition(App.Settings.widescreen ? thInt("304 - carousel layout list btn pos x widescreen") : thInt("300 - carousel layout list btn pos x"),
								thInt("13 - carousel layout list btn pos y"));
		gridBtn->SetPosition(App.Settings.widescreen ? thInt("336 - carousel layout grid btn pos x widescreen") : thInt("340 - carousel layout grid btn pos x"),
								thInt("13 - carousel layout grid btn pos y"));
		carouselBtn->SetPosition(App.Settings.widescreen ? thInt("368 - carousel layout carousel btn pos x widescreen") : thInt("380 - carousel layout carousel btn pos x"),
								thInt("13 - carousel layout carousel btn pos y"));
		bannerGridBtn->SetPosition(App.Settings.widescreen ? thInt("400 - carousel layout bannergrid btn pos x widescreen") : thInt("420 - carousel layout bannergrid btn pos x"),
								thInt("13 - carousel layout bannergrid btn pos y"));
		lockBtn->SetPosition(App.Settings.widescreen ? thInt("432 - carousel layout lock btn pos x widescreen") : thInt("460 - carousel layout lock btn pos x"),
								thInt("13 - carousel layout lock btn pos y"));
		dvdBtn->SetPosition(App.Settings.widescreen ? thInt("464 - carousel layout dvd btn pos x widescreen") : thInt("500 - carousel layout dvd btn pos x"),
								thInt("13 - carousel layout dvd btn pos y"));

		gameBrowser = new GuiGameCarousel(thInt("640 - game carousel layout width"), thInt("400 - game carousel layout height"), App.Settings.theme_path, App.Settings.GameListOffset);
		gameBrowser->SetPosition(thInt("0 - game carousel layout pos x"), thInt("-20 - game carousel layout pos y"));
		gameBrowser->SetAlignment(ALIGN_LEFT, ALIGN_CENTER);

		//! Setup optional background image
		if(!carouselBackground)
			carouselBackground = App.Resources.GetImageData("carouselBackground.png");
		if(carouselBackground)
			bgImg->SetImage(carouselBackground);
		else
			bgImg->SetImage(background);
	}
	else if(App.Settings.gameDisplay == BANNERGRID_MODE)
	{
		DownloadBtn->SetSize(0, 0);
		UpdateGameInfoText(NULL);
		bannerGridBtn->SetImage(bannerGridBtnImg);
		bannerGridBtn->SetImageOver(bannerGridBtnImg);
		listBtn->SetImage(listBtnImg_g);
		listBtn->SetImageOver(listBtnImg_g);
		gridBtn->SetImage(gridBtnImg_g);
		gridBtn->SetImageOver(gridBtnImg_g);
		carouselBtn->SetImage(carouselBtnImg_g);
		carouselBtn->SetImageOver(carouselBtnImg_g);

		favoriteBtn->SetPosition(App.Settings.widescreen ? thInt("144 - bannergrid layout favorite btn pos x widescreen") : thInt("100 - bannergrid layout favorite btn pos x"),
								thInt("13 - bannergrid layout favorite btn pos y"));
		searchBtn->SetPosition(App.Settings.widescreen ? thInt("176 - bannergrid layout search btn pos x widescreen") : thInt("140 - bannergrid layout search btn pos x"),
								thInt("13 - bannergrid layout search btn pos y"));
		sortBtn->SetPosition(App.Settings.widescreen ? thInt("208 - bannergrid layout abc/sort btn pos x widescreen") : thInt("180 - bannergrid layout abc/sort btn pos x"),
								thInt("13 - bannergrid layout abc/sort btn pos y"));
		loaderModeBtn->SetPosition(App.Settings.widescreen ? thInt("240 - bannergrid layout loadermode btn pos x widescreen") : thInt("220 - bannergrid layout loadermode btn pos x"),
								thInt("13 - bannergrid layout loadermode btn pos y"));
		categBtn->SetPosition(App.Settings.widescreen ? thInt("272 - bannergrid layout category btn pos x widescreen") : thInt("260 - bannergrid layout category btn pos x"),
								thInt("13 - bannergrid layout category btn pos y"));
		listBtn->SetPosition(App.Settings.widescreen ? thInt("304 - bannergrid layout list btn pos x widescreen") : thInt("300 - bannergrid layout list btn pos x"),
								thInt("13 - bannergrid layout list btn pos y"));
		gridBtn->SetPosition(App.Settings.widescreen ? thInt("336 - bannergrid layout grid btn pos x widescreen") : thInt("340 - bannergrid layout grid btn pos x"),
								thInt("13 - bannergrid layout grid btn pos y"));
		carouselBtn->SetPosition(App.Settings.widescreen ? thInt("368 - bannergrid layout carousel btn pos x widescreen") : thInt("380 - bannergrid layout carousel btn pos x"),
								thInt("13 - bannergrid layout carousel btn pos y"));
		bannerGridBtn->SetPosition(App.Settings.widescreen ? thInt("400 - bannergrid layout bannergrid btn pos x widescreen") : thInt("420 - bannergrid layout bannergrid btn pos x"),
								thInt("13 - bannergrid layout bannergrid btn pos y"));
		lockBtn->SetPosition(App.Settings.widescreen ? thInt("432 - bannergrid layout lock btn pos x widescreen") : thInt("460 - bannergrid layout lock btn pos x"),
								thInt("13 - bannergrid layout lock btn pos y"));
		dvdBtn->SetPosition(App.Settings.widescreen ? thInt("464 - bannergrid layout dvd btn pos x widescreen") : thInt("500 - bannergrid layout dvd btn pos x"),
								thInt("13 - bannergrid layout dvd btn pos y"));

		gameBrowser = new GuiBannerGrid(App.Settings.GameListOffset + App.Settings.SelectedGame);
	}

	if (thInt("1 - show hdd info: 1 for on and 0 for off") == 1) //force show hdd info
		Append(usedSpaceTxt);
	if (thInt("1 - show game count: 1 for on and 0 for off") == 1) //force show game cnt info
		Append(gamecntBtn);
	if (App.Settings.godmode || !(App.Settings.ParentalBlocks & BLOCK_SD_RELOAD_BUTTON))
		Append(sdcardBtn);
	Append(poweroffBtn);
	Append(gameInfo);
	Append(homeBtn);
	Append(settingsBtn);

	if (App.Settings.godmode || !(App.Settings.ParentalBlocks & BLOCK_HBC_MENU))
		Append(homebrewBtn);

	if (App.Settings.godmode || !(App.Settings.ParentalBlocks & BLOCK_GAME_INSTALL))
		Append(installBtn);

	if (App.Settings.godmode || !(App.Settings.ParentalBlocks & BLOCK_COVER_DOWNLOADS))
		Append(DownloadBtn);

	if ((App.Settings.gameDisplay == LIST_MODE) && (App.Settings.godmode || !(App.Settings.ParentalBlocks & BLOCK_GAMEID_CHANGE)))
		Append(idBtn);

	Append(favoriteBtn);
	Append(searchBtn);
	Append(sortBtn);
	Append(categBtn);
	Append(listBtn);
	Append(gridBtn);
	Append(loaderModeBtn);
	Append(carouselBtn);
	Append(bannerGridBtn);
	Append(lockBtn);
	Append(dvdBtn);

	if ((App.Settings.hddinfo == CLOCK_HR12) || (App.Settings.hddinfo == CLOCK_HR24))
	{
		Append(clockTimeBack);
		Append(clockTime);
	}

	if (gameBrowser)
		Append(gameBrowser);

	if (show_searchwindow)
	{
		searchBar = new GuiSearchBar;
		App.MainWindow->Append(searchBar);
	}

	SetEffect(EFFECT_FADE, 40);
	ResumeGui();

	while(parentElement && this->GetEffect() > 0) usleep(100);
}

int GameBrowseMenu::MainLoop()
{
	UpdateClock();
	CheckDiscSlotUpdate();

	if (updateavailable == true)
	{
		gprintf("\tUpdate Available\n");
		SetState(STATE_DISABLED);
		UpdateApp();
		updateavailable = false;
		SetState(STATE_DEFAULT);
	}

	else if (poweroffBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tpoweroffBtn clicked\n");
		int choice = 0;
		if(isWiiU())
			choice = WindowPrompt(tr( "How to Shutdown?" ), 0, tr( "Full shutdown" ), tr("Cancel"));
		else
			choice = WindowPrompt(tr( "How to Shutdown?" ), 0, tr( "Full shutdown" ), tr( "Standby" ), tr( "Cancel" ));
		
		if (choice == 2)
			Sys_ShutdownToIdle();
		else if (choice == 1)
			Sys_ShutdownToStandby();

		poweroffBtn->ResetState();
	}
	else if (gamecntBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tgameCntBtn clicked\n");
		gamecntBtn->ResetState();

		int choice = WindowPrompt(0, fmt("%s %sGameList ?", tr( "Save Game List to" ), App.Settings.update_path), "TXT", "CSV", tr( "Back" ));
		if (choice)
		{
			if (save_gamelist(choice == 2))
				WindowPrompt(0, tr( "Saved" ), tr( "OK" ));
			else
				WindowPrompt(tr( "Error" ), tr( "Could not save." ), tr( "OK" ));
		}
	}
	else if (homeBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\thomeBtn clicked\n");
		WindowExitPrompt();

		homeBtn->ResetState();
	}
	else if (installBtn->GetState() == STATE_CLICKED)
	{
		if(!App.Settings.godmode && (App.Settings.ParentalBlocks & BLOCK_GAME_INSTALL))
		{
			WindowPrompt(tr( "Permission denied." ), tr( "Console must be unlocked for this option." ), tr( "OK" ));
			installBtn->ResetState();
			return MENU_NONE;
		}

		int choice = WindowPrompt(tr( "Install a game" ), 0, tr( "Yes" ), tr( "No" ));
		if (choice == 1)
		{
			this->SetState(STATE_DISABLED);
			if(!(App.Settings.LoaderMode & MODE_WIIGAMES) && (App.Library.Games.GameCount() == 0))
			{
				if(WBFS_ReInit(WBFS_DEVICE_USB) < 0)
					ShowError(tr("Failed to initialize the USB storage device."));
				else
				{
					App.Library.Games.ReadGameList();
					App.Library.DisplayNames.LoadTitlesFromGameTDB(App.Settings.titlestxt_path, false);
					if(App.Settings.ShowFreeSpace)
					{
						ThreadedTask::Instance()->AddCallback(&HDDSizeCallback);
						ThreadedTask::Instance()->Execute();
					}
					return MenuInstall();
				}
			}
			else
				return MenuInstall();

			this->SetState(STATE_DEFAULT);
		}
		installBtn->ResetState();
	}
	else if (sdcardBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tsdCardBtn Clicked\n");
		if(WindowPrompt(tr("Are you sure you want to remount SD?"), tr("The application might crash if there is currently a read/write access to the SD card!"), tr("Yes"), tr("Cancel")))
		{
			HaltGui();
			BannerAsync::HaltThread();
			bgMusic->Pause();
			App.Settings.Save();
			DeviceHandler::Instance()->UnMountSD();
			DeviceHandler::Instance()->MountSD();
		
			App.Settings.Load();
			gprintf("\tLoading language...%s\n", App.Settings.LoadLanguage(App.Settings.language_path, CONSOLE_DEFAULT) ? "done" : "failed");
			
			gprintf("\tLoading game settings...%s\n", App.Library.Settings.Load(App.Settings.ConfigPath) ? "done" : "failed");
			gprintf("\tLoading game statistics...%s\n", App.Library.Statistics.Load(App.Settings.ConfigPath) ? "done" : "failed");
			
			App.Theme.LoadFont(App.Settings.theme_path);
			gprintf("\tLoading theme...%s\n", App.Theme.Load(App.Settings.theme) ? "done" : "failed (using default)");
			
			bgMusic->Resume();
			App.Library.Games.FilterList();
			ReloadBrowser();
			BannerAsync::ResumeThread();
			ResumeGui();
		}

		sdcardBtn->ResetState();
	}

	else if (DownloadBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tDownloadBtn Clicked\n");
		ImageDownloader::DownloadImages();
		ReloadBrowser();
		DownloadBtn->ResetState();
	}

	else if (settingsBtn->GetState() == STATE_CLICKED)
	{
		if (!App.Settings.godmode && (App.Settings.ParentalBlocks & BLOCK_GLOBAL_SETTINGS))
		{
			WindowPrompt(tr( "Permission denied." ), tr( "Console must be unlocked for this option." ), tr( "OK" ));
			settingsBtn->ResetState();
			return MENU_NONE;
		}

		return MENU_SETTINGS;
	}

	else if (favoriteBtn->GetState() == STATE_CLICKED)
	{
		favoriteBtn->ResetState();
		gprintf("\tfavoriteBtn Clicked\n");

		if(App.Settings.GameSort & SORT_FAVORITE)
			App.Settings.GameSort &= ~SORT_FAVORITE;
		else
			App.Settings.GameSort |= SORT_FAVORITE;

		App.Library.Games.FilterList();

		if((App.Settings.GameSort & SORT_FAVORITE) && App.Library.Games.size() == 0)
		{
			App.Settings.GameSort &= ~SORT_FAVORITE;
			App.Library.Games.FilterList();
			ShowError(tr("No favorites selected."));
		}
		else
			ReloadBrowser();
	}

	else if (searchBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tsearchBtn Clicked\n");
		show_searchwindow = !show_searchwindow;
		App.Library.Games.FilterList();
		ReloadBrowser();
		searchBtn->ResetState();
		if(show_searchwindow && wcslen(App.Library.Games.GetCurrentFilter()) == 0)
			GridRowsPreSearch = App.Settings.gridRows; //! store old rows amount
	}

	else if (searchBar && (searchChar = searchBar->GetClicked()))
	{
		if (searchChar > 27) //! Character clicked
		{
			int len = App.Library.Games.GetCurrentFilter() ? wcslen(App.Library.Games.GetCurrentFilter()) : 0;
			wchar_t newFilter[len + 2];
			if (App.Library.Games.GetCurrentFilter()) wcscpy(newFilter, App.Library.Games.GetCurrentFilter());
			newFilter[len] = searchChar;
			newFilter[len + 1] = 0;

			App.Library.Games.FilterList(newFilter);
		}
		else if (searchChar == 27) //! Close
		{
			show_searchwindow = false;
			searchBtn->StopEffect();
		}
		else if (searchChar == 7) //! Clear
		{
			App.Library.Games.FilterList(L"");
			App.Settings.gridRows = GridRowsPreSearch; //! restore old rows amount so we don't stay on one row
		}
		else if (searchChar == 8) //! Backspace
		{
			int len = wcslen(App.Library.Games.GetCurrentFilter());
			wchar_t newFilter[len + 1];
			if (App.Library.Games.GetCurrentFilter()) wcscpy(newFilter, App.Library.Games.GetCurrentFilter());
			newFilter[len > 0 ? len - 1 : 0] = 0;
			App.Library.Games.FilterList(newFilter);
			if(len == 1)
				App.Settings.gridRows = GridRowsPreSearch; //! restore old rows amount so we don't stay on one row
		}
		else if (searchChar == 6)
		{
			App.Settings.SearchMode = App.Settings.SearchMode == SEARCH_BEGINNING ? SEARCH_CONTENT : SEARCH_BEGINNING;
			App.Library.Games.FilterList();
		}
		ReloadBrowser();
		return MENU_NONE;
	}

	else if (sortBtn->GetState() == STATE_CLICKED)
	{
		sortBtn->ResetState();
		gprintf("\tsortBtn clicked\n");
		if(App.Settings.GameSort & SORT_ABC)
		{
			App.Settings.GameSort &= ~SORT_ABC;
			App.Settings.GameSort |= SORT_RANKING;
		}
		else if(App.Settings.GameSort & SORT_RANKING)
		{
			App.Settings.GameSort &= ~SORT_RANKING;
			App.Settings.GameSort |= SORT_PLAYCOUNT;
		}
		else if(App.Settings.GameSort & SORT_PLAYCOUNT)
		{
			App.Settings.GameSort &= ~SORT_PLAYCOUNT;
			App.Settings.GameSort |= SORT_PLAYERS;
		}
		else if(App.Settings.GameSort & SORT_PLAYERS)
		{
			App.Settings.GameSort &= ~SORT_PLAYERS;
			App.Settings.GameSort |= SORT_ABC;
		}

		App.Library.Games.FilterList();
		ReloadBrowser();
	}

	else if (listBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tlistBtn Clicked\n");
		if (App.Settings.gameDisplay != LIST_MODE)
		{
			App.Settings.gameDisplay = LIST_MODE;
			ReloadBrowser();
		}
		listBtn->ResetState();
	}

	else if (gridBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tgridBtn Clicked\n");
		if (App.Settings.gameDisplay != GRID_MODE)
		{
			App.Settings.gameDisplay = GRID_MODE;
			ReloadBrowser();
		}
		gridBtn->ResetState();
	}

	else if (carouselBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tcarouselBtn Clicked\n");
		if (App.Settings.gameDisplay != CAROUSEL_MODE)
		{
			App.Settings.gameDisplay = CAROUSEL_MODE;
			ReloadBrowser();
		}
		carouselBtn->ResetState();
	}

	else if (bannerGridBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tbannerGridBtn Clicked\n");
		if(!SystemMenuResources::Instance()->IsLoaded()) {
			WindowPrompt(tr( "Error:" ), tr( "Banner grid layout is only available with AHBPROT! Please consider installing new HBC version." ), tr( "OK" ));
			bannerGridBtn->ResetState();
			return MENU_NONE;
		}
		if (App.Settings.gameDisplay != BANNERGRID_MODE)
		{
			App.Settings.gameDisplay = BANNERGRID_MODE;
			ReloadBrowser();
		}
		bannerGridBtn->ResetState();
	}

	else if (homebrewBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\thomebrewBtn Clicked\n");
		return MENU_HOMEBREWBROWSE;
	}

	else if (gameInfo->GetState() == STATE_CLICKED)
	{
		gprintf("\tgameinfo Clicked\n");
		int SelectedGame = GetSelectedGame();
		gameInfo->ResetState();
		if (SelectedGame >= 0 && SelectedGame < (s32)App.Library.Games.size())
		{
			SetState(STATE_DISABLED);
			int choice = showGameInfo(SelectedGame, 0);
			SetState(STATE_DEFAULT);
			if (choice == 2)
				homeBtn->SetState(STATE_CLICKED);
		}
	}
	else if (lockBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tlockBtn clicked\n");
		lockBtn->ResetState();
		if (App.Settings.godmode)
		{
			if(WindowPrompt(tr( "Parental Control" ), tr( "Are you sure you want to lock USB Loader GX?" ), tr( "Yes" ), tr( "No" )) == 1)
			{
				App.Settings.godmode = 0;
				App.Library.Games.FilterList();
				ReloadBrowser();
			}
		}
		else
		{
			//password check to unlock Install,Delete and Format
			SetState(STATE_DISABLED);
			int result = PasswordCheck(App.Settings.unlockCode);
			SetState(STATE_DEFAULT);
			if (result > 0)
			{
				if(result == 1)
					WindowPrompt( tr( "Correct Password" ), tr( "All the features of USB Loader GX are unlocked." ), tr( "OK" ));
				App.Settings.godmode = 1;
				App.Library.Games.FilterList();
				ReloadBrowser();
			}
			else if(result < 0)
				WindowPrompt(tr( "Wrong Password" ), tr( "USB Loader GX is protected" ), tr( "OK" ));
		}
	}

	else if(categBtn->GetState() == STATE_CLICKED)
	{
		if (!App.Settings.godmode && (App.Settings.ParentalBlocks & BLOCK_CATEGORIES_MENU))
		{
			WindowPrompt(tr( "Permission denied." ), tr( "Console must be unlocked for this option." ), tr( "OK" ));
			categBtn->ResetState();
			return returnMenu;
		}

		App.MainWindow->SetState(STATE_DISABLED);
		CategorySwitchPrompt promptMenu;
		promptMenu.SetAlignment(thAlign("center - category switch prompt align hor"), thAlign("middle - category switch prompt align ver"));
		promptMenu.SetPosition(thInt("0 - category switch prompt pos x"), thInt("0 - category switch prompt pos y"));
		promptMenu.SetEffect(EFFECT_FADE, 20);
		App.MainWindow->Append(&promptMenu);

		promptMenu.Show();

		promptMenu.SetEffect(EFFECT_FADE, -20);
		while(promptMenu.GetEffect() > 0) usleep(100);
		App.MainWindow->Remove(&promptMenu);
		categBtn->ResetState();
		App.MainWindow->SetState(STATE_DEFAULT);
		if(promptMenu.categoriesChanged())
		{
			App.Library.Games.FilterList();
			ReloadBrowser();
		}
	}

	else if(loaderModeBtn->GetState() == STATE_CLICKED)
	{
		if (!App.Settings.godmode && (App.Settings.ParentalBlocks & BLOCK_LOADER_MODE_BUTTON))
		{
			WindowPrompt(tr( "Permission denied." ), tr( "Console must be unlocked for this option." ), tr( "OK" ));
			loaderModeBtn->ResetState();
			return returnMenu;
		}

		int choice = CheckboxWindow(tr( "Select titles sources." ), 0, tr( "Wii Games" ), tr( "Nand Channels" ), tr("EmuNand Channels"), tr("GC Games"), 0, 0, App.Settings.LoaderMode);
		if(choice != CheckedNone && choice != App.Settings.LoaderMode)
		{
			App.Settings.LoaderMode = choice;

			if((App.Settings.LoaderMode & MODE_WIIGAMES) && (App.Library.Games.GameCount() == 0))
			{
				s32 wbfsinit = WBFS_Init(WBFS_DEVICE_USB);
				if (wbfsinit < 0)
				{
					ShowError("%s %s", tr( "USB Device not initialized." ), tr("Switching to channel list mode."));
					App.Settings.LoaderMode &= ~MODE_WIIGAMES;
					App.Settings.LoaderMode |= MODE_NANDCHANNELS;
				}
				else
				{
					WBFS_ReInit(WBFS_DEVICE_USB);
				}
				App.Library.Games.ReadGameList();

				if(App.Settings.ShowFreeSpace)
				{
					ThreadedTask::Instance()->AddCallback(&HDDSizeCallback);
					ThreadedTask::Instance()->Execute();
				}
			}

			wString oldFilter(App.Library.Games.GetCurrentFilter());
			App.Library.DisplayNames.LoadTitlesFromGameTDB(App.Settings.titlestxt_path, false);
			App.Library.Games.FilterList(oldFilter.c_str());
			ReloadBrowser();
		}
		loaderModeBtn->ResetState();
	}

	else if (App.Settings.gameDisplay == LIST_MODE && idBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tidBtn Clicked\n");
		struct discHdr * header = App.Library.Games[GetSelectedGame()];
		//enter new game ID
		char entered[7];
		snprintf(entered, sizeof(entered), "%s", (char *) header->id);
		int result = OnScreenKeyboard(entered, sizeof(entered), 0);
		if (result == 1)
		{
			WBFS_ReIDGame(header->id, entered);
			wString oldFilter(App.Library.Games.GetCurrentFilter());
			App.Library.Games.ReadGameList();
			App.Library.Games.FilterList(oldFilter.c_str());
			ReloadBrowser();
		}
		idBtn->ResetState();
	}

	else if (App.Settings.gameDisplay == LIST_MODE && GetSelectedGame() != gameSelectedOld)
	{
		gameSelectedOld = GetSelectedGame();
		int gameSelected = gameSelectedOld;
		if(gameSelected >= 0 && gameSelected < (s32)App.Library.Games.size())
		{
			struct discHdr *header = App.Library.Games[gameSelected];
			LoadCover(header);
			UpdateGameInfoText(header->id);
		}
	}

	if(gameBrowser)
	{
		//! This is bad, but for saving pupose it will be in main loop
		App.Settings.GameListOffset = gameBrowser->getListOffset();
		App.Settings.SelectedGame = gameBrowser->GetSelectedOption()-App.Settings.GameListOffset;
	}

	gameClicked = gameBrowser ? gameBrowser->GetClickedOption() : -1;

	if(gameClicked >= 0 && gameClicked < (s32)App.Library.Games.size())
		OpenClickedGame(App.Library.Games[gameClicked]);

	return returnMenu;
}

void GameBrowseMenu::CheckDiscSlotUpdate()
{
	// No need to update every 1 ms
	static u32 delayCounter = 0;
	if(++delayCounter < 100)
		return;

	delayCounter = 0;
	u32 DiscDriveCover = 0;
	WDVD_GetCoverStatus(&DiscDriveCover);//for detecting if i disc has been inserted

	if ((DiscDriveCover & 0x02) && (DiscDriveCover != DiscDriveCoverOld))
	{
		int choice = WindowPrompt(tr( "Disc Insert Detected" ), 0, tr( "Install" ), tr( "Mount DVD drive" ), tr( "Cancel" ));
		if (choice == 1)
			installBtn->SetState(STATE_CLICKED);
		else if (choice == 2)
			dvdBtn->SetState(STATE_CLICKED);
	}
	else if (dvdBtn->GetState() == STATE_CLICKED)
	{
		gprintf("\tdvdBtn Clicked\n");
		if(DiscDriveCover & 0x02)
		{
			if(!dvdheader)
				dvdheader = new struct discHdr;

			if(Disc_Mount(dvdheader) < 0)
			{
				delete dvdheader;
				dvdheader = NULL;
				ShowError(tr("Can't mount or unknown disc format."));
			}
			else
				OpenClickedGame(dvdheader);
		}
		else
			WindowPrompt(tr( "No disc inserted." ), 0, tr( "OK" ));

		dvdBtn->ResetState();
	}

	if(DiscDriveCoverOld != DiscDriveCover)
	{
		if(DiscDriveCover & 0x02)
			dvdBtn->SetImage(dvdBtnImg);
		else
			dvdBtn->SetImage(dvdBtnImg_g);

		DiscDriveCoverOld = DiscDriveCover;
	}
}

void GameBrowseMenu::UpdateClock()
{
	if(App.Settings.hddinfo != CLOCK_HR12 && App.Settings.hddinfo != CLOCK_HR24)
		return;

	time_t rawtime = time(0);
	if(rawtime == lastrawtime) //! Only update every 1 second
		return;

	lastrawtime = rawtime;

	char theTime[50];
	theTime[0] = 0;

	struct tm * timeinfo = localtime(&rawtime);
	if (App.Settings.hddinfo == CLOCK_HR12)
	{
		if (rawtime & 1)
			strftime(theTime, sizeof(theTime), "%I:%M", timeinfo);
		else
			strftime(theTime, sizeof(theTime), "%I %M", timeinfo);
	}
	if (App.Settings.hddinfo == CLOCK_HR24)
	{
		if (rawtime & 1)
			strftime(theTime, sizeof(theTime), "%H:%M", timeinfo);
		else
			strftime(theTime, sizeof(theTime), "%H %M", timeinfo);
	}
	clockTime->SetText(theTime);

	if (App.Settings.screensaver != 0 && Input::ControlActivityTimeout())
	{
		WindowScreensaver();
	}
}

void GameBrowseMenu::UpdateGameInfoText(const u8 * gameId)
{
	if(!gameId)
	{
		Remove(GameRegionTxt);
		delete GameRegionTxt;
		GameRegionTxt = NULL;
		Remove(GameIDTxt);
		delete GameIDTxt;
		GameIDTxt = NULL;
		return;
	}

	char gameregion[10];
	char IDfull[7];
	snprintf(IDfull, sizeof(IDfull), (char *) gameId);

	switch (IDfull[3])
	{
		case 'A':
		case 'B':
		case 'U':
		case 'X':
			strcpy(gameregion, tr("Region Free"));
			break;
		case 'E':
		case 'N':
			strcpy(gameregion, "NTSC U");
			break;
		case 'J':
			strcpy(gameregion, "NTSC J");
			break;
		case 'K':
		case 'Q':
		case 'T':
			strcpy(gameregion, "NTSC K");
			break; 
		case 'W':
			strcpy(gameregion, "NTSC T");
			break;
		default:
			strcpy(gameregion, "  PAL ");
	}

	HaltGui();
	if (App.Settings.sinfo == GAMEINFO_ID || App.Settings.sinfo == GAMEINFO_BOTH)
	{
		Remove(GameIDTxt);
		delete GameIDTxt;
		GameIDTxt = new GuiText(IDfull, 22, thColor("r=55 g=190 b=237 a=255 - game id text color"));
		GameIDTxt->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		GameIDTxt->SetPosition(thInt("68 - gameID btn pos x"), thInt("305 - gameID btn pos y"));
		GameIDTxt->SetEffect(EFFECT_FADE, 20);
		Append(GameIDTxt);
	}
	//don't try to show region for channels because all the custom channels wont follow the rules
	if ((App.Settings.sinfo == GAMEINFO_REGION) || (App.Settings.sinfo == GAMEINFO_BOTH))
	{
		Remove(GameRegionTxt);
		delete GameRegionTxt;
		GameRegionTxt = new GuiText(gameregion, 22, thColor("r=55 g=190 b=237 a=255 - region info text color"));
		GameRegionTxt->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		GameRegionTxt->SetPosition(thInt("68 - region info text pos x"), thInt("30 - region info text pos y"));
		GameRegionTxt->SetEffect(EFFECT_FADE, 20);
		Append(GameRegionTxt);
	}
	ResumeGui();
}

int GameBrowseMenu::OpenClickedGame(struct discHdr *header)
{
	int choice = -1;

	if (searchBar)
	{
		HaltGui();
		App.MainWindow->Remove(searchBar);
		ResumeGui();
	}

	SetAllowDim(false);
	SetState(STATE_DISABLED);
	if(gameBrowser)
		gameBrowser->SetState(STATE_DISABLED);

	if (App.Settings.wiilight == ON)
		wiilight(1);

	if (App.Settings.quickboot) { //quickboot game
		GameWindow::BootGame(header);
	}
	else if((App.Settings.GameWindowMode == GAMEWINDOW_BANNER) ||
			(App.Settings.GameWindowMode == GAMEWINDOW_BOTH && App.Settings.gameDisplay == BANNERGRID_MODE))
	{
		BannerWindow GamePrompt(this, header);
		App.MainWindow->Append(&GamePrompt);

		choice = GamePrompt.Run();
	}
	else if((App.Settings.GameWindowMode == GAMEWINDOW_DISC) ||
			(App.Settings.GameWindowMode == GAMEWINDOW_BOTH && App.Settings.gameDisplay != BANNERGRID_MODE))
	{
		SetAllowDim(true);
		GameWindow GamePrompt(this, header);
		App.MainWindow->Append(&GamePrompt);

		choice = GamePrompt.Run();
	}

	if (choice == 1)
	{
		App.Library.Games.FilterList();
		ReloadBrowser();
		if(App.Settings.ShowFreeSpace)
		{
			ThreadedTask::Instance()->AddCallback(&HDDSizeCallback);
			ThreadedTask::Instance()->Execute();
		}
	}

	wiilight(0);
	
	SetState(STATE_DEFAULT);
	SetAllowDim(true);

	if(gameBrowser)
		gameBrowser->SetState(STATE_DEFAULT);

	if (searchBar)
	{
		HaltGui();
		App.MainWindow->Append(searchBar);
		ResumeGui();
	}

	return 0;
}

void GameBrowseMenu::LoadCover(struct discHdr *header)
{
	gameCoverImg->SetImage(NULL);

	delete gameCover;
	gameCover = LoadCoverImage(header);

	gameCoverImg->SetImage(gameCover);// put the new image on the download button
}

void GameBrowseMenu::UpdateCallback(void * e)
{
	//! Draw the selected Icon allways on top
	GameBrowseMenu * w = (GameBrowseMenu *) e;

	for(u32 i = 0; i < w->ToolBar.size(); ++i)
	{
		if(w->ToolBar[i]->GetState() == STATE_SELECTED)
		{
			w->Remove(w->ToolBar[i]);
			w->Append(w->ToolBar[i]);
			break;
		}
	}
}

void GameBrowseMenu::UpdateFreeSpace(void * arg)
{
	char spaceinfo[30];
	spaceinfo[0] = 0;

	if(App.Settings.ShowFreeSpace)
	{
		float freespace = 0.0, used = 0.0;
		int ret = WBFS_DiskSpace(&used, &freespace);
		if(ret >= 0)
		{
			if (strcmp(App.Settings.db_language, "JA") == 0)
			{
				// needs to be "total...used" for Japanese
				snprintf(spaceinfo, sizeof(spaceinfo), "%.2fGB %s %.2fGB %s", (freespace + used), tr( "of" ), freespace, tr( "free" ));
			}
			else
			{
				snprintf(spaceinfo, sizeof(spaceinfo), "%.2fGB %s %.2fGB %s", freespace, tr( "of" ), (freespace + used), tr( "free" ));
			}
		}
	}

	if(Exiting)
		return;

	usedSpaceTxt->SetText(spaceinfo);
}

