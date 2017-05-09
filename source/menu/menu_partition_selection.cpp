#include <unistd.h>
#include "video.h"
#include "menus.h"
#include "usbloader/usbstorage2.h"
#include "usbloader/wbfs.h"
#include "GUI/gui_optionbrowser.h"
#include "Controls/DeviceHandler.hpp"
#include "themes/CTheme.h"
#include "utils/tools.h"

/****************************************************************************
 * SelectPartitionMenu
 ***************************************************************************/
int SelectPartitionMenu()
{
	bool ExitSelect = false;
	OptionList options;

	u32 counter = 0;
	int choice = -1;
	int ret = -1;

	//create the partitionlist
	for (int cnt = 0; cnt < DeviceHandler::GetUSBPartitionCount(); cnt++)
	{
		PartitionHandle * usbHandle = DeviceHandler::Instance()->GetUSBHandleFromPartition(cnt);
		int portPart = DeviceHandler::PartitionToPortPartition(cnt);
		/* Calculate size in gigabytes */
		f32 size = usbHandle->GetSize(portPart) / GB_SIZE;

		if (size)
		{
			options.SetName(counter, "%s %d %s: ", tr( "Partition" ), cnt + 1, usbHandle->GetFSName(portPart));
			options.SetValue(counter, "%.2fGB", size);
		}
		else
		{
			options.SetName(counter, "%s %d:", tr( "Partition" ), cnt + 1);
			options.SetValue(counter, tr( "Can't be formatted" ));
		}
		counter++;
	}

	GuiImageData btnpwroff(App.Resources.GetFile("wiimote_poweroff.png"), App.Resources.GetFileSize("wiimote_poweroff.png"));
	GuiImageData btnpwroffOver(App.Resources.GetFile("wiimote_poweroff_over.png"), App.Resources.GetFileSize("wiimote_poweroff_over.png"));
	GuiImageData btnhome(App.Resources.GetFile("menu_button.png"), App.Resources.GetFileSize("menu_button.png"));
	GuiImageData btnhomeOver(App.Resources.GetFile("menu_button_over.png"), App.Resources.GetFileSize("menu_button_over.png"));
	GuiImageData battery(App.Resources.GetFile("battery.png"), App.Resources.GetFileSize("battery.png"));
	GuiImageData batteryBar(App.Resources.GetFile("battery_bar.png"), App.Resources.GetFileSize("battery_bar.png"));
	GuiImageData batteryRed(App.Resources.GetFile("battery_red.png"), App.Resources.GetFileSize("battery_red.png"));
	GuiImageData batteryBarRed(App.Resources.GetFile("battery_bar_red.png"), App.Resources.GetFileSize("battery_bar_red.png"));

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0);

	GuiImage poweroffBtnImg(&btnpwroff);
	GuiImage poweroffBtnImgOver(&btnpwroffOver);
	poweroffBtnImg.SetWidescreen(App.Settings.widescreen);
	poweroffBtnImgOver.SetWidescreen(App.Settings.widescreen);
	GuiButton poweroffBtn(&poweroffBtnImg, &poweroffBtnImgOver, 0, 3,
							thInt("576 - power off btn pos x"), thInt("355 - power off btn pos y"),
							&trigA, btnSoundOver, btnSoundClick2, 1);
	GuiImage exitBtnImg(&btnhome);
	GuiImage exitBtnImgOver(&btnhomeOver);
	exitBtnImg.SetWidescreen(App.Settings.widescreen);
	exitBtnImgOver.SetWidescreen(App.Settings.widescreen);
	GuiButton exitBtn(&exitBtnImg, &exitBtnImgOver, 0, 3,
						thInt("489 - home menu btn pos x"), thInt("371 - home menu btn pos y"),
						&trigA, btnSoundOver, btnSoundClick2, 1);
	exitBtn.SetTrigger(&trigHome);

	GuiOptionBrowser optionBrowser(396, 280, &options, "bg_options_settings.png");
	optionBrowser.SetPosition(0, 40);
	optionBrowser.SetAlignment(ALIGN_CENTER, ALIGN_TOP);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&poweroffBtn);
	w.Append(&exitBtn);

	App.MainWindow->Append(&w);
	App.MainWindow->Append(&optionBrowser);

	ResumeGui();

	while (!ExitSelect)
	{
		VIDEO_WaitVSync();

		if (shutdown)
			Sys_Shutdown();
		if (reset)
			Sys_Reboot();

		ret = optionBrowser.GetClickedOption();

		if (ret >= 0)
		{
			if (strcmp(options.GetValue(ret), tr( "Can't be formatted" )) != 0)
			{
				choice = ret;
				ExitSelect = true;
			}
		}

		if (poweroffBtn.GetState() == STATE_CLICKED)
		{
			choice = WindowPrompt(tr( "Shutdown System" ), tr( "Are you sure?" ), tr( "Yes" ), tr( "No" ));
			if (choice == 1)
				Sys_Shutdown();

		}
		else if (exitBtn.GetState() == STATE_CLICKED)
		{
			choice = WindowPrompt(tr( "Return to Wii Menu" ), tr( "Are you sure?" ), tr( "Yes" ), tr( "No" ));
			if (choice == 1)
				Sys_LoadMenu();
		}
	}

	HaltGui();

	App.MainWindow->Remove(&optionBrowser);
	App.MainWindow->Remove(&w);
	ResumeGui();

	return choice;
}

