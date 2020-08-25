/*
 * SPDX-FileCopyrightText: 2008-2011 Peter Penz <peter.penz19@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "viewsettingstab.h"

#include "dolphin_compactmodesettings.h"
#include "dolphin_detailsmodesettings.h"
#include "dolphin_iconsmodesettings.h"
#include "dolphinfontrequester.h"
#include "global.h"
#include "views/zoomlevelinfo.h"

#include <KLocalizedString>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QHelpEvent>
#include <QFormLayout>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

ViewSettingsTab::ViewSettingsTab(Mode mode, QWidget* parent) :
    QWidget(parent),
    m_mode(mode),
    m_defaultSizeSlider(nullptr),
    m_previewSizeSlider(nullptr),
    m_fontRequester(nullptr),
    m_widthBox(nullptr),
    m_maxLinesBox(nullptr),
    m_expandableFolders(nullptr),
    m_recursiveDirectorySizeLimit(nullptr)
{
    QFormLayout* topLayout = new QFormLayout(this);

    // Create "Icon Size" section
    const int minRange = ZoomLevelInfo::minimumLevel();
    const int maxRange = ZoomLevelInfo::maximumLevel();

    m_defaultSizeSlider = new QSlider(Qt::Horizontal);
    m_defaultSizeSlider->setPageStep(1);
    m_defaultSizeSlider->setTickPosition(QSlider::TicksBelow);
    m_defaultSizeSlider->setRange(minRange, maxRange);
    connect(m_defaultSizeSlider, &QSlider::valueChanged,
            this, &ViewSettingsTab::slotDefaultSliderMoved);
    topLayout->addRow(i18nc("@label:listbox", "Default icon size:"), m_defaultSizeSlider);

    m_previewSizeSlider = new QSlider(Qt::Horizontal);
    m_previewSizeSlider->setPageStep(1);
    m_previewSizeSlider->setTickPosition(QSlider::TicksBelow);
    m_previewSizeSlider->setRange(minRange, maxRange);
    connect(m_previewSizeSlider, &QSlider::valueChanged,
            this, &ViewSettingsTab::slotPreviewSliderMoved);
    topLayout->addRow(i18nc("@label:listbox", "Preview icon size:"), m_previewSizeSlider);


    topLayout->addItem(new QSpacerItem(0, Dolphin::VERTICAL_SPACER_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed));


    // Create "Label" section
    m_fontRequester = new DolphinFontRequester(this);
    topLayout->addRow(i18nc("@label:listbox", "Label font:"), m_fontRequester);

    switch (m_mode) {
    case IconsMode: {
        m_widthBox = new QComboBox();
        m_widthBox->addItem(i18nc("@item:inlistbox Label width", "Small"));
        m_widthBox->addItem(i18nc("@item:inlistbox Label width", "Medium"));
        m_widthBox->addItem(i18nc("@item:inlistbox Label width", "Large"));
        m_widthBox->addItem(i18nc("@item:inlistbox Label width", "Huge"));
        topLayout->addRow(i18nc("@label:listbox", "Label width:"), m_widthBox);

        m_maxLinesBox = new QComboBox();
        m_maxLinesBox->addItem(i18nc("@item:inlistbox Maximum lines", "Unlimited"));
        m_maxLinesBox->addItem(i18nc("@item:inlistbox Maximum lines", "1"));
        m_maxLinesBox->addItem(i18nc("@item:inlistbox Maximum lines", "2"));
        m_maxLinesBox->addItem(i18nc("@item:inlistbox Maximum lines", "3"));
        m_maxLinesBox->addItem(i18nc("@item:inlistbox Maximum lines", "4"));
        m_maxLinesBox->addItem(i18nc("@item:inlistbox Maximum lines", "5"));
        topLayout->addRow(i18nc("@label:listbox", "Maximum lines:"), m_maxLinesBox);
        break;
    }
    case CompactMode: {
        m_widthBox = new QComboBox();
        m_widthBox->addItem(i18nc("@item:inlistbox Maximum width", "Unlimited"));
        m_widthBox->addItem(i18nc("@item:inlistbox Maximum width", "Small"));
        m_widthBox->addItem(i18nc("@item:inlistbox Maximum width", "Medium"));
        m_widthBox->addItem(i18nc("@item:inlistbox Maximum width", "Large"));
        topLayout->addRow(i18nc("@label:listbox", "Maximum width:"), m_widthBox);
        break;
    }
    case DetailsMode:
        m_expandableFolders = new QCheckBox(i18nc("@option:check", "Expandable"));
        topLayout->addRow(i18nc("@label:checkbox", "Folders:"), m_expandableFolders);

#ifndef Q_OS_WIN
        // Sorting properties
        m_numberOfItems = new QRadioButton(i18nc("option:radio", "Number of items"));
        m_sizeOfContents = new QRadioButton(i18nc("option:radio", "Size of contents, up to "));

        QButtonGroup* sortingModeGroup = new QButtonGroup(this);
        sortingModeGroup->addButton(m_numberOfItems);
        sortingModeGroup->addButton(m_sizeOfContents);

        m_recursiveDirectorySizeLimit = new QSpinBox();
        connect(m_recursiveDirectorySizeLimit, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
            m_recursiveDirectorySizeLimit->setSuffix(i18np(" level deep", " levels deep", value));
        });
        m_recursiveDirectorySizeLimit->setRange(1, 20);
        m_recursiveDirectorySizeLimit->setSingleStep(1);

        QHBoxLayout *contentsSizeLayout = new QHBoxLayout();
        contentsSizeLayout->addWidget(m_sizeOfContents);
        contentsSizeLayout->addWidget(m_recursiveDirectorySizeLimit);

        topLayout->addRow(i18nc("@title:group", "Folder size displays:"), m_numberOfItems);
        topLayout->addRow(QString(), contentsSizeLayout);
#endif
        break;
    }

    loadSettings();

    connect(m_defaultSizeSlider, &QSlider::valueChanged, this, &ViewSettingsTab::changed);
    connect(m_previewSizeSlider, &QSlider::valueChanged, this, &ViewSettingsTab::changed);
    connect(m_fontRequester, &DolphinFontRequester::changed, this, &ViewSettingsTab::changed);

    switch (m_mode) {
    case IconsMode:
        connect(m_widthBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ViewSettingsTab::changed);
        connect(m_maxLinesBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ViewSettingsTab::changed);
        break;
    case CompactMode:
        connect(m_widthBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ViewSettingsTab::changed);
        break;
    case DetailsMode:
        connect(m_expandableFolders, &QCheckBox::toggled, this, &ViewSettingsTab::changed);
#ifndef Q_OS_WIN
        connect(m_recursiveDirectorySizeLimit, QOverload<int>::of(&QSpinBox::valueChanged), this, &ViewSettingsTab::changed);
        connect(m_numberOfItems, &QRadioButton::toggled, this, &ViewSettingsTab::changed);
        connect(m_sizeOfContents, &QRadioButton::toggled, this, [=]() {
            m_recursiveDirectorySizeLimit->setEnabled(m_sizeOfContents->isChecked());
        });
#endif
        break;
    default:
        break;
    }
}

ViewSettingsTab::~ViewSettingsTab()
{
}

void ViewSettingsTab::applySettings()
{
    const QFont font = m_fontRequester->currentFont();
    const bool useSystemFont = (m_fontRequester->mode() == DolphinFontRequester::SystemFont);

    switch (m_mode) {
    case IconsMode:
        IconsModeSettings::setTextWidthIndex(m_widthBox->currentIndex());
        IconsModeSettings::setMaximumTextLines(m_maxLinesBox->currentIndex());
        break;
    case CompactMode:
        CompactModeSettings::setMaximumTextWidthIndex(m_widthBox->currentIndex());
        break;
    case DetailsMode:
        DetailsModeSettings::setExpandableFolders(m_expandableFolders->isChecked());
#ifndef Q_OS_WIN
        DetailsModeSettings::setDirectorySizeCount(m_numberOfItems->isChecked());
        DetailsModeSettings::setRecursiveDirectorySizeLimit(m_recursiveDirectorySizeLimit->value());
#endif
        break;
    default:
        break;
    }

    ViewModeSettings settings(viewMode());

    const int iconSize = ZoomLevelInfo::iconSizeForZoomLevel(m_defaultSizeSlider->value());
    const int previewSize = ZoomLevelInfo::iconSizeForZoomLevel(m_previewSizeSlider->value());
    settings.setIconSize(iconSize);
    settings.setPreviewSize(previewSize);

    settings.setUseSystemFont(useSystemFont);
    settings.setFontFamily(font.family());
    settings.setFontSize(font.pointSizeF());
    settings.setItalicFont(font.italic());
    settings.setFontWeight(font.weight());

    settings.save();
}

void ViewSettingsTab::restoreDefaultSettings()
{
    KConfigSkeleton* settings = nullptr;
    switch (m_mode) {
    case IconsMode:   settings = IconsModeSettings::self(); break;
    case CompactMode: settings = CompactModeSettings::self(); break;
    case DetailsMode: settings = DetailsModeSettings::self(); break;
    default: Q_ASSERT(false); break;
    }

    settings->useDefaults(true);
    loadSettings();
    settings->useDefaults(false);
}

void ViewSettingsTab::loadSettings()
{
    switch (m_mode) {
    case IconsMode:
        m_widthBox->setCurrentIndex(IconsModeSettings::textWidthIndex());
        m_maxLinesBox->setCurrentIndex(IconsModeSettings::maximumTextLines());
        break;
    case CompactMode:
        m_widthBox->setCurrentIndex(CompactModeSettings::maximumTextWidthIndex());
        break;
    case DetailsMode:
        m_expandableFolders->setChecked(DetailsModeSettings::expandableFolders());
        if (DetailsModeSettings::directorySizeCount()) {
            m_numberOfItems->setChecked(true);
            m_recursiveDirectorySizeLimit->setEnabled(false);
        } else {
            m_sizeOfContents->setChecked(true);
            m_recursiveDirectorySizeLimit->setEnabled(true);
        }
        m_recursiveDirectorySizeLimit->setValue(DetailsModeSettings::recursiveDirectorySizeLimit());
        break;
    default:
        break;
    }

    const ViewModeSettings settings(viewMode());

    const QSize iconSize(settings.iconSize(), settings.iconSize());
    m_defaultSizeSlider->setValue(ZoomLevelInfo::zoomLevelForIconSize(iconSize));

    const QSize previewSize(settings.previewSize(), settings.previewSize());
    m_previewSizeSlider->setValue(ZoomLevelInfo::zoomLevelForIconSize(previewSize));

    m_fontRequester->setMode(settings.useSystemFont()
                             ? DolphinFontRequester::SystemFont
                             : DolphinFontRequester::CustomFont);

    QFont font(settings.fontFamily(), qRound(settings.fontSize()));
    font.setItalic(settings.italicFont());
    font.setWeight(settings.fontWeight());
    font.setPointSizeF(settings.fontSize());
    m_fontRequester->setCustomFont(font);
}

ViewModeSettings::ViewMode ViewSettingsTab::viewMode() const
{
    ViewModeSettings::ViewMode mode;

    switch (m_mode) {
    case ViewSettingsTab::IconsMode:   mode = ViewModeSettings::IconsMode; break;
    case ViewSettingsTab::CompactMode: mode = ViewModeSettings::CompactMode; break;
    case ViewSettingsTab::DetailsMode: mode = ViewModeSettings::DetailsMode; break;
    default:                           mode = ViewModeSettings::IconsMode;
                                       Q_ASSERT(false);
                                       break;
    }

    return mode;
}


void ViewSettingsTab::slotDefaultSliderMoved(int value)
{
    showToolTip(m_defaultSizeSlider, value);
}

void ViewSettingsTab::slotPreviewSliderMoved(int value)
{
    showToolTip(m_previewSizeSlider, value);
}

void ViewSettingsTab::showToolTip(QSlider* slider, int value)
{
    const int size = ZoomLevelInfo::iconSizeForZoomLevel(value);
    slider->setToolTip(i18ncp("@info:tooltip", "Size: 1 pixel", "Size: %1 pixels", size));
    if (!slider->isVisible()) {
        return;
    }
    QPoint global = slider->rect().topLeft();
    global.ry() += slider->height() / 2;
    QHelpEvent toolTipEvent(QEvent::ToolTip, QPoint(0, 0), slider->mapToGlobal(global));
    QApplication::sendEvent(slider, &toolTipEvent);
}

