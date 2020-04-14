/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QtcDevPlugin.
 * 
 * QtcDevPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QtcDevPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QtcDevPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#include "qtcrunconfiguration.h"
#include "qtcrunconfigurationfactory.h"
#include "qtcdevpluginconstants.h"
#include "Widgets/filetypevalidatinglineedit.h"

#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/variablechooser.h>
#include <coreplugin/icore.h>

#include <utils/theme/theme.h>

#include <QtWidgets>
#include <QtDebug>

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief Available Qt Creator themes
 *
 * Returns the list of the available themes in Qt Creator.
 * \return The list of Qt Creator themes
 */
// TODO this should be available from Qt Creator util library
QStringList availableThemes(void)
{
    QStringList themes;

    QDir resourceDir = QDir(Core::ICore::resourcePath());
    resourceDir.cd(QLatin1String("themes"));
    resourceDir.setFilter(QDir::Files);
    resourceDir.setNameFilters(QStringList() << QLatin1String("*.creatortheme"));
    foreach (QFileInfo info, resourceDir.entryInfoList()) {
        themes << info.completeBaseName();
    }

    int defaultIndex = themes.indexOf(QLatin1String("default"));
    if (defaultIndex != -1)
        themes.prepend(themes.takeAt(defaultIndex));
    else
        qWarning() << "\"default\" theme not found in ressource path.";

    QDir userResourceDir = QDir(Core::ICore::userResourcePath());
    userResourceDir.cd(QLatin1String("themes"));
    userResourceDir.setFilter(QDir::Files);
    userResourceDir.setNameFilters(QStringList() << QLatin1String("*.creatortheme"));
    foreach (QFileInfo info, userResourceDir.entryInfoList()) {
        themes << info.completeBaseName();
    }

    qDebug() << themes << resourceDir.absolutePath() << userResourceDir.absolutePath();

    return themes;
}

QtcRunConfiguration::QtcRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::RunConfiguration(parent, id)
{
    mWorkingDirectory = Utils::FilePath::fromString(QLatin1String("%{buildDir}"));
    mThemeName = Utils::creatorTheme()->displayName();

    setDefaultDisplayName(tr("Run Qt Creator"));

    /* TODO ensure this run configuration cannot be run with valgrind...
     * To do this, the code of the Valgrind plugin should be altered:
     * 1.ValgrindRunControlFactory should check the type of the given RunConfiguration (e.g. in canRun())
     * and addAspects() should only add aspects provided bu runnable RunControl factories.
     * 2.Alternatively, ValgrindPlugin, should ensure the extra aspects are added to
     * sensible RunConfiguration and RunConfiguration::addExtraAspects() should be removed. */
    addAspect<ProjectExplorer::LocalEnvironmentAspect>(parent);
}

QString QtcRunConfiguration::pluginName(void) const
{
    return buildTargetInfo().projectFilePath.toFileInfo().baseName();
}

ProjectExplorer::Runnable QtcRunConfiguration::runnable(void) const
{
    ProjectExplorer::Runnable runnable;
    runnable.executable = Utils::FilePath::fromString(QCoreApplication::applicationFilePath());
    runnable.commandLineArguments = commandLineArgumentsList().join(QLatin1Char(' '));;
    if (macroExpander() != NULL)
        runnable.workingDirectory = macroExpander()->expand(mWorkingDirectory.toString());
    else
        runnable.workingDirectory = mWorkingDirectory.toString();
    runnable.environment = aspect<ProjectExplorer::LocalEnvironmentAspect>()->environment();
    runnable.device = ProjectExplorer::DeviceManager::instance()->defaultDevice(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
    return runnable;
}

QVariantMap QtcRunConfiguration::toMap(void) const
{
    QVariantMap map(ProjectExplorer::RunConfiguration::toMap());
    if (QString::compare(mWorkingDirectory.toString(), QLatin1String("%{buildDir}"), Utils::HostOsInfo::fileNameCaseSensitivity()) != 0)
        map.insert(Constants::WorkingDirectoryKey, mWorkingDirectory.toString());
    if (!mSettingsPath.isEmpty())
        map.insert(Constants::SettingsPathKey, mSettingsPath.toString());
    map.insert(Constants::ThemeKey, mThemeName);

    return map;
}

bool QtcRunConfiguration::fromMap(const QVariantMap& map)
{
    if (!ProjectExplorer::RunConfiguration::fromMap(map))
        return false;

    mWorkingDirectory = Utils::FilePath::fromString(map.value(Constants::WorkingDirectoryKey, QLatin1String("%{buildDir}")).toString());
    mSettingsPath = Utils::FilePath::fromString(map.value(Constants::SettingsPathKey, QString()).toString());

    QStringList themes = availableThemes();
    QString theme = map.value(Constants::ThemeKey, QString()).toString();
    if (themes.contains(theme))
        mThemeName = theme;

    if (!pluginName().isEmpty())
        setDisplayName(tr("Run Qt Creator with \"%1\"").arg(pluginName()));

    return true;
}

QStringList QtcRunConfiguration::commandLineArgumentsList(void) const
{
    QStringList cmdArgs;

    cmdArgs << QLatin1String("-theme") << mThemeName;

    QString pluginsPath = buildTargetInfo().workingDirectory.toString();
    pluginsPath.replace(QLatin1Char('"'), QLatin1String("\\\""));
    if (pluginsPath.contains(QLatin1Char(' ')))
        pluginsPath.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
    cmdArgs << QLatin1String("-pluginpath") << pluginsPath;

    QString settingsPath = mSettingsPath.toString();
    if (macroExpander() != NULL)
        settingsPath = macroExpander()->expand(settingsPath);
    settingsPath.replace(QLatin1Char('"'), QLatin1String("\\\""));
    if (settingsPath.contains(QLatin1Char(' ')))
        settingsPath.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
    if (!mSettingsPath.isEmpty())
        cmdArgs << QLatin1String("-settingspath") << settingsPath;

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs;
}

QtcRunConfigurationWidget::QtcRunConfigurationWidget(QtcRunConfiguration* runConfig, QWidget* parent)
    : QWidget(parent), mRunConfig(runConfig)
{
    mWorkingDirectoryEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mWorkingDirectoryEdit->setMacroExpander(runConfig->macroExpander());
    mWorkingDirectoryEdit->setAcceptDirectories(true);
    mWorkingDirectoryLabel = new QLabel(tr("Working directory:"), this);
    mWorkingDirectoryLabel->setBuddy(mWorkingDirectoryEdit);
    mWorkingDirectoryButton = new QPushButton(tr("Browse..."), this);

    mSettingsPathEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mSettingsPathEdit->setMacroExpander(runConfig->macroExpander());
    mSettingsPathEdit->setAcceptDirectories(true);
    mSettingsPathEdit->setEnabled(false);
    mSettingsPathCheck = new QCheckBox(tr("Use alternative settings path"));
    mSettingsPathCheck->setChecked(false);
    mSettingsPathButton = new QPushButton(tr("Browse..."), this);
    mSettingsPathButton->setEnabled(false);

    mThemeCombo = new QComboBox(this);
    mThemeCombo->setEditable(false);
    mThemeLabel = new QLabel(tr("Theme:"), this);
    mThemeLabel->setBuddy(mThemeCombo);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mWorkingDirectoryLabel, 0, 0, Qt::AlignLeft);
    mainLayout->addWidget(mWorkingDirectoryEdit, 0, 1);
    mainLayout->addWidget(mWorkingDirectoryButton, 0, 2, Qt::AlignCenter);
    mainLayout->addWidget(mSettingsPathCheck, 1, 0, Qt::AlignLeft);
    mainLayout->addWidget(mSettingsPathEdit, 1, 1);
    mainLayout->addWidget(mSettingsPathButton, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(mThemeLabel, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(mThemeCombo, 2, 1, 1, 2, Qt::AlignRight);
    mainLayout->setColumnStretch(1, 1);

    QStringList themes = availableThemes();
    mThemeCombo->addItems(themes);

    Core::VariableChooser::addSupportForChildWidgets(this, runConfig->macroExpander());

    setLayout(mainLayout);

    connect(mWorkingDirectoryEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateWorkingDirectory(bool)));
    connect(mWorkingDirectoryEdit, SIGNAL(editingFinished()),
            this, SLOT(updateWorkingDirectory()));
    connect(mWorkingDirectoryButton, SIGNAL(released()),
            this, SLOT(browseWorkingDirectory()));

    connect(mSettingsPathCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateSettingsPathState(bool)));
    connect(mSettingsPathEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateSettingsPath(bool)));
    connect(mSettingsPathEdit, SIGNAL(editingFinished()),
            this, SLOT(updateSettingsPath()));
    connect(mSettingsPathButton, SIGNAL(released()),
            this, SLOT(browseSettingsPath()));

    connect(mThemeCombo, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(updateTheme(const QString&)));
}

void QtcRunConfigurationWidget::showEvent(QShowEvent *se)
{
    mWorkingDirectoryEdit->setText(mRunConfig->mWorkingDirectory.toString());

    mSettingsPathEdit->setText(mRunConfig->mSettingsPath.toString());
    mSettingsPathCheck->setChecked(!mRunConfig->mSettingsPath.isEmpty());

    int currentThemeIndex = 0;
    while ((currentThemeIndex < mThemeCombo->count()) &&
           (QString::compare(mThemeCombo->itemText(currentThemeIndex), mRunConfig->mThemeName, Qt::CaseInsensitive) != 0))
        currentThemeIndex++;
    QTC_ASSERT(currentThemeIndex != mThemeCombo->count(), );
    if (currentThemeIndex != mThemeCombo->count())
        mThemeCombo->setCurrentIndex(currentThemeIndex);

    QWidget::showEvent(se);
}

void QtcRunConfigurationWidget::updateWorkingDirectory(bool valid)
{
    if (valid)
        mRunConfig->mWorkingDirectory = Utils::FilePath::fromUserInput(mWorkingDirectoryEdit->text());
}

void QtcRunConfigurationWidget::updateWorkingDirectory(void)
{
    if (mWorkingDirectoryEdit->isValid())
        mRunConfig->mWorkingDirectory = Utils::FilePath::fromUserInput(mWorkingDirectoryEdit->text());
    mWorkingDirectoryEdit->setText(mRunConfig->mWorkingDirectory.toString());
}

void QtcRunConfigurationWidget::browseWorkingDirectory(void)
{
    QString wd = QFileDialog::getExistingDirectory(this, tr("Choose working directory"), mRunConfig->mWorkingDirectory.toString());

    if (!wd.isNull())
        mRunConfig->mWorkingDirectory = Utils::FilePath::fromString(wd);
    mWorkingDirectoryEdit->setText(mRunConfig->mWorkingDirectory.toString());
}

void QtcRunConfigurationWidget::updateSettingsPathState(bool checked)
{
    mSettingsPathEdit->setEnabled(checked);
    mSettingsPathButton->setEnabled(checked);
    mRunConfig->mSettingsPath = checked ? Utils::FilePath::fromUserInput(mSettingsPathEdit->text()) : Utils::FilePath();
}

void QtcRunConfigurationWidget::updateSettingsPath(bool valid)
{
    if (valid)
        mRunConfig->mSettingsPath = Utils::FilePath::fromUserInput(mSettingsPathEdit->text());
}

void QtcRunConfigurationWidget::updateSettingsPath(void)
{
    if (mSettingsPathEdit->isValid())
        mRunConfig->mSettingsPath = Utils::FilePath::fromUserInput(mSettingsPathEdit->text());
    mSettingsPathEdit->setText(mRunConfig->mSettingsPath.toString());
}

void QtcRunConfigurationWidget::browseSettingsPath(void)
{
    QString wd = QFileDialog::getExistingDirectory(this, tr("Choose alternative settings path"), mRunConfig->mSettingsPath.toString());

    if (!wd.isNull())
        mRunConfig->mSettingsPath = Utils::FilePath::fromString(wd);
    mSettingsPathEdit->setText(mRunConfig->mSettingsPath.toString());
}

void QtcRunConfigurationWidget::updateTheme(const QString& theme)
{
    mRunConfig->mThemeName = theme;
}

} // Internal
} // QTestLibPlugin

