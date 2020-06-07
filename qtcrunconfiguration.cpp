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
#include "pathaspect.h"

#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/icore.h>

#include <utils/theme/theme.h>

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
    foreach (QString fileName, resourceDir.entryList()) {
        QSettings themeSettings(resourceDir.absoluteFilePath(fileName), QSettings::IniFormat);
        themes << themeSettings.value(QLatin1String("ThemeName"), QCoreApplication::tr("unnamed")).toString();
    }

    QDir userResourceDir = QDir(Core::ICore::userResourcePath());
    userResourceDir.cd(QLatin1String("themes"));
    userResourceDir.setFilter(QDir::Files);
    userResourceDir.setNameFilters(QStringList() << QLatin1String("*.creatortheme"));
    foreach (QString fileName, userResourceDir.entryList()) {
        QSettings themeSettings(userResourceDir.absoluteFilePath(fileName), QSettings::IniFormat);
        themes << themeSettings.value(QLatin1String("ThemeName"), QCoreApplication::tr("unnamed")).toString();
    }

    int currentIndex = themes.indexOf(Utils::creatorTheme()->displayName());
    if (currentIndex != -1)
        themes.prepend(themes.takeAt(currentIndex));
    else
        qWarning() << "Current theme \"" + Utils::creatorTheme()->displayName() + "\" theme not found in ressource path.";

    qDebug() << themes << resourceDir.absolutePath() << userResourceDir.absolutePath();

    return themes;
}

QtcRunConfiguration::QtcRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    ProjectExplorer::RunConfiguration(parent, id)
{
    setDefaultDisplayName(tr("Run Qt Creator"));

    auto workingDirectoryAspect = addAspect<PathAspect>();
    workingDirectoryAspect->setId(Core::Id(Constants::WorkingDirectoryId));
    workingDirectoryAspect->setSettingsKey(Constants::WorkingDirectoryKey);
    workingDirectoryAspect->setDisplayName(tr("Working directory"));
    workingDirectoryAspect->setMacroExpanderProvider([this] {return macroExpander();});
    workingDirectoryAspect->setDefaultValue(QLatin1String("%{buildDir}"));

    auto settingsPathAspect = addAspect<PathAspect>();
    settingsPathAspect->setId(Core::Id(Constants::SettingsPathId));
    settingsPathAspect->setSettingsKey(Constants::SettingsPathKey);
    settingsPathAspect->setDisplayName(tr("Alternative settings path"));
    settingsPathAspect->setCheckable(true);
    settingsPathAspect->setMacroExpanderProvider([this] {return macroExpander();});

    auto themeAspect = addAspect<ProjectExplorer::BaseSelectionAspect>();
    themeAspect->setId(Core::Id(Constants::ThemeId));
    themeAspect->setSettingsKey(Constants::ThemeKey);
    themeAspect->setDisplayName(tr("Theme:"));
    themeAspect->setDisplayStyle(ProjectExplorer::BaseSelectionAspect::DisplayStyle::ComboBox);
    foreach (QString theme, availableThemes())
        themeAspect->addOption(theme);

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
    runnable.commandLineArguments = commandLineArgumentsList().join(QLatin1Char(' '));
    runnable.workingDirectory = static_cast<PathAspect*>(aspect(Core::Id(Constants::WorkingDirectoryId)))->value().toString();
    if (macroExpander() != NULL)
        runnable.workingDirectory = macroExpander()->expand(runnable.workingDirectory);
    runnable.environment = aspect<ProjectExplorer::LocalEnvironmentAspect>()->environment();
    runnable.device = ProjectExplorer::DeviceManager::instance()->defaultDevice(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
    return runnable;
}

QStringList QtcRunConfiguration::commandLineArgumentsList(void) const
{
    QStringList cmdArgs;

    QStringList themes = availableThemes();
    int themeIndex = static_cast<ProjectExplorer::BaseSelectionAspect*>(aspect(Core::Id(Constants::ThemeId)))->value();
    if ((themeIndex >= 0) && (themeIndex < themes.size()))
        cmdArgs << QLatin1String("-theme") << themes[themeIndex];

    QString pluginsPath = buildTargetInfo().workingDirectory.toString();
    pluginsPath.replace(QLatin1Char('"'), QLatin1String("\\\""));
    if (pluginsPath.contains(QLatin1Char(' ')))
        pluginsPath.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
    cmdArgs << QLatin1String("-pluginpath") << pluginsPath;

    QString settingsPath = static_cast<PathAspect*>(aspect(Core::Id(Constants::SettingsPathId)))->value().toString();
    if (macroExpander() != NULL)
        settingsPath = macroExpander()->expand(settingsPath);
    settingsPath.replace(QLatin1Char('"'), QLatin1String("\\\""));
    if (settingsPath.contains(QLatin1Char(' ')))
        settingsPath.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
    if (!settingsPath.isEmpty())
        cmdArgs << QLatin1String("-settingspath") << settingsPath;

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs;
}

} // Internal
} // QTestLibPlugin

