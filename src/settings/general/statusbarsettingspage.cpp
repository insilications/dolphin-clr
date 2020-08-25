/*
 * SPDX-FileCopyrightText: 2009 Peter Penz <peter.penz19@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "statusbarsettingspage.h"

#include "dolphin_generalsettings.h"

#include <KLocalizedString>

#include <QCheckBox>
#include <QVBoxLayout>

StatusBarSettingsPage::StatusBarSettingsPage(QWidget* parent) :
    SettingsPageBase(parent),
    m_showZoomSlider(nullptr),
    m_showSpaceInfo(nullptr)
{
    m_showZoomSlider = new QCheckBox(i18nc("@option:check", "Show zoom slider"), this);
    m_showSpaceInfo = new QCheckBox(i18nc("@option:check", "Show space information"), this);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(m_showZoomSlider);
    topLayout->addWidget(m_showSpaceInfo);
    topLayout->addStretch();

    loadSettings();

    connect(m_showZoomSlider, &QCheckBox::toggled, this, &StatusBarSettingsPage::changed);
    connect(m_showSpaceInfo, &QCheckBox::toggled, this, &StatusBarSettingsPage::changed);
}

StatusBarSettingsPage::~StatusBarSettingsPage()
{
}

void StatusBarSettingsPage::applySettings()
{
    GeneralSettings* settings = GeneralSettings::self();
    settings->setShowZoomSlider(m_showZoomSlider->isChecked());
    settings->setShowSpaceInfo(m_showSpaceInfo->isChecked());
    settings->save();
}

void StatusBarSettingsPage::restoreDefaults()
{
    GeneralSettings* settings = GeneralSettings::self();
    settings->useDefaults(true);
    loadSettings();
    settings->useDefaults(false);
}

void StatusBarSettingsPage::loadSettings()
{
    m_showZoomSlider->setChecked(GeneralSettings::showZoomSlider());
    m_showSpaceInfo->setChecked(GeneralSettings::showSpaceInfo());
}

