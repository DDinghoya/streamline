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

#include "CategorySwitchPrompt.hpp"
#include "App.h"
#include "language/gettext.h"

CategorySwitchPrompt::CategorySwitchPrompt()
	: CategoryPrompt(tr("Show Categories")), oldSettingEnabled(App.Settings.EnabledCategories),
	oldSettingRequired(App.Settings.RequiredCategories), oldSettingForbidden(App.Settings.ForbiddenCategories)
{
	browser->checkBoxClicked.connect(this, &CategorySwitchPrompt::OnCheckboxClick);
	browserRefresh.connect(this, &CategorySwitchPrompt::onBrowserRefresh);
	resetChanges.connect(this, &CategorySwitchPrompt::onResetChanges);

	browserRefresh();
}

void CategorySwitchPrompt::onResetChanges()
{
	App.Settings.EnabledCategories = oldSettingEnabled;
	App.Settings.RequiredCategories = oldSettingRequired;
	App.Settings.ForbiddenCategories = oldSettingForbidden;
	App.Library.Categories.Load(App.Settings.ConfigPath);
}

void CategorySwitchPrompt::onBrowserRefresh()
{
	browser->Clear();
	App.Library.Categories.CategoryList.goToFirst();
	do
	{
		bool checked = false;
		int style = GuiCheckbox::CHECKSIGN;

		// Verify the Enabled Categories [v]
		for(u32 i = 0; i < App.Settings.EnabledCategories.size(); ++i)
		{
			if(App.Settings.EnabledCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
			{
				checked = true;
				break;
			}
		}
		
		// Verify the Forbidden Categories [X]
		if(!checked)
		{
			for(u32 i = 0; i < App.Settings.ForbiddenCategories.size(); ++i)
			{
				if(App.Settings.ForbiddenCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
				{
					checked = true;
					style = GuiCheckbox::CROSS;
					break;
				}
			}
		}
		
		// Verify the Required Categories [+]
		if(!checked)
		{
			for(u32 i = 0; i < App.Settings.RequiredCategories.size(); ++i)
			{
				if(App.Settings.RequiredCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
				{
					checked = true;
					style = GuiCheckbox::PLUS;
					break;
				}
			}
		}
		
		browser->AddEntrie(tr(App.Library.Categories.CategoryList.getCurrentName().c_str()), checked, style, true);
	}
	while(App.Library.Categories.CategoryList.goToNext());

	App.Library.Categories.CategoryList.goToFirst();
	browser->RefreshList();
}

void CategorySwitchPrompt::OnCheckboxClick(GuiCheckbox *checkBox, int index)
{
	App.Library.Categories.CategoryList.goToFirst();
	for(int i = 0; i < index; ++i)
		App.Library.Categories.CategoryList.goToNext();

	u32 i;
	if(!checkBox->IsChecked())
	{
		// Remove from Required
		for(i = 0; i < App.Settings.RequiredCategories.size(); ++i)
		{
			if(App.Settings.RequiredCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
			{
				App.Settings.RequiredCategories.erase(App.Settings.RequiredCategories.begin()+i);
				markChanged();
				break;
			}
		}
	}
	else if(checkBox->GetStyle() == GuiCheckbox::CHECKSIGN)
	{
		// Add to Enabled
		App.Settings.EnabledCategories.push_back(App.Library.Categories.CategoryList.getCurrentID());
		markChanged();
	}
	else if(checkBox->GetStyle() == GuiCheckbox::CROSS)
	{
		// Remove from Enabled
		for(i = 0; i < App.Settings.EnabledCategories.size(); ++i)
		{
			if(App.Settings.EnabledCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
			{
				App.Settings.EnabledCategories.erase(App.Settings.EnabledCategories.begin()+i);
				break;
			}
		}
		// Add to Forbidden
		App.Settings.ForbiddenCategories.push_back(App.Library.Categories.CategoryList.getCurrentID());
		markChanged();
	}
	else if(checkBox->GetStyle() == GuiCheckbox::PLUS && index > 0)
	{
		// Remove from Forbidden
		for(i = 0; i < App.Settings.ForbiddenCategories.size(); ++i)
		{
			if(App.Settings.ForbiddenCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
			{
				App.Settings.ForbiddenCategories.erase(App.Settings.ForbiddenCategories.begin()+i);
				break;
			}
		}
		// Add to Required
		App.Settings.RequiredCategories.push_back(App.Library.Categories.CategoryList.getCurrentID());
		markChanged();
	}

	// Override Style cycling for category "All"
	if(index == 0 && checkBox->GetStyle() == GuiCheckbox::PLUS)
	{
		checkBox->SetStyle(GuiCheckbox::CHECKSIGN);
		checkBox->SetChecked(false);
		for(i = 0; i < App.Settings.ForbiddenCategories.size(); ++i)
		{
			if(App.Settings.ForbiddenCategories[i] == App.Library.Categories.CategoryList.getCurrentID())
			{
				App.Settings.ForbiddenCategories.erase(App.Settings.ForbiddenCategories.begin()+i);
				markChanged();
				break;
			}
		}
	}
}
