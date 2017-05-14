/****************************************************************************
 * Copyright (C) 2011
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#include "CategorySelectPrompt.hpp"
#include "App.h"
#include "language/gettext.h"
#include "utils/StringTools.h"

CategorySelectPrompt::CategorySelectPrompt(struct discHdr * header)
	: CategoryPrompt(fmt("%s - %s", (char *)header->id, tr("Categories"))),
	gameHeader(header)
{
	browser->checkBoxClicked.connect(this, &CategorySelectPrompt::OnCheckboxClick);
	browserRefresh.connect(this, &CategorySelectPrompt::onBrowserRefresh);
	resetChanges.connect(this, &CategorySelectPrompt::onResetChanges);

	browserRefresh();
}

void CategorySelectPrompt::onResetChanges()
{
	App.Library.GameCategories.Load(App.Settings.ConfigPath);
}

void CategorySelectPrompt::onBrowserRefresh()
{
	browser->Clear();
	App.Library.GameCategories.CategoryList.goToFirst();
	do
	{
		bool checked = false;
		const vector<unsigned int> gameCat = App.Library.GameCategories[gameHeader->id];

		for (u32 i = 0; i < gameCat.size(); ++i)
		{
			if (gameCat[i] == App.Library.GameCategories.CategoryList.getCurrentID())
			{
				checked = true;
				break;
			}
		}

		browser->AddEntrie(tr(App.Library.GameCategories.CategoryList.getCurrentName().c_str()), checked);
	} while (App.Library.GameCategories.CategoryList.goToNext());

	App.Library.GameCategories.CategoryList.goToFirst();
	browser->RefreshList();
}

void CategorySelectPrompt::OnCheckboxClick(GuiCheckbox *checkBox, int index)
{
	App.Library.GameCategories.CategoryList.goToFirst();
	for (int i = 0; i < index; ++i)
		App.Library.GameCategories.CategoryList.goToNext();

	if (App.Library.GameCategories.CategoryList.getCurrentID() == 0)
	{
		checkBox->SetChecked(true);
		return;
	}

	const vector<unsigned int> gameCat = App.Library.GameCategories[gameHeader->id];

	u32 i;
	for (i = 0; i < gameCat.size(); ++i)
	{
		if (gameCat[i] == App.Library.GameCategories.CategoryList.getCurrentID())
		{
			if (!checkBox->IsChecked())
			{
				App.Library.GameCategories.RemoveCategory((const char *)gameHeader->id, gameCat[i]);
				markChanged();
			}
			break;
		}
	}

	if (i == gameCat.size() && checkBox->IsChecked())
	{
		App.Library.GameCategories.SetCategory(gameHeader->id, App.Library.GameCategories.CategoryList.getCurrentID());
		markChanged();
	}
}
