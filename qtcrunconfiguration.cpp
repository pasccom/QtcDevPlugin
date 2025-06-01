/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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
#include "qtcdevpluginconstants.h"
#include "pathaspect.h"

#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/icore.h>

#include <utils/macroexpander.h>
#include <utils/processinterface.h>
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
    Utils::FileFilter fileFilter(QStringList() << QLatin1String("*.creatortheme"), QDir::Files);

    for (Utils::FilePath filePath: Core::ICore::resourcePath("themes").dirEntries(fileFilter)) {
        QSettings themeSettings(filePath.nativePath(), QSettings::IniFormat);
        themes << themeSettings.value(QLatin1String("ThemeName"), QCoreApplication::tr("unnamed")).toString();
    }

    for (Utils::FilePath filePath: Core::ICore::userResourcePath("themes").dirEntries(fileFilter)) {
        QSettings themeSettings(filePath.nativePath(), QSettings::IniFormat);
        themes << themeSettings.value(QLatin1String("ThemeName"), QCoreApplication::tr("unnamed")).toString();
    }

    int currentIndex = themes.indexOf(Utils::creatorTheme()->displayName());
    if (currentIndex != -1)
        themes.prepend(themes.takeAt(currentIndex));
    else
        qWarning() << "Current theme \"" + Utils::creatorTheme()->displayName() + "\" theme not found in ressource path.";

    qDebug() << themes << Core::ICore::resourcePath("themes") << Core::ICore::userResourcePath("themes");

    return themes;
}

QtcRunConfiguration::QtcRunConfiguration(ProjectExplorer::Target *parent, Utils::Id id):
    ProjectExplorer::RunConfiguration(parent, id)
{
    setDefaultDisplayName(tr("Run Qt Creator"));

    mWorkingDirectoryAspect.setId(Utils::Id(Constants::WorkingDirectoryId));
    mWorkingDirectoryAspect.setSettingsKey(Utils::Key(Constants::WorkingDirectoryKey));
    mWorkingDirectoryAspect.setDisplayName(tr("Working directory"));
    mWorkingDirectoryAspect.setAcceptDirectories(true);
    mWorkingDirectoryAspect.setMacroExpanderProvider([this] {return const_cast<Utils::MacroExpander*>(macroExpander());});
    mWorkingDirectoryAspect.setDefaultValue(Utils::FilePath::fromString("%{buildDir}"));

    mSettingsPathAspect.setId(Utils::Id(Constants::SettingsPathId));
    mSettingsPathAspect.setSettingsKey(Utils::Key(Constants::SettingsPathKey));
    mSettingsPathAspect.setDisplayName(tr("Alternative settings path"));
    mSettingsPathAspect.setAcceptDirectories(true);
    mSettingsPathAspect.setRequireWritable(true);
    mSettingsPathAspect.setCheckable(true);
    mSettingsPathAspect.setMacroExpanderProvider([this] {return const_cast<Utils::MacroExpander*>(macroExpander());});

    mThemeAspect.setId(Utils::Id(Constants::ThemeId));
    mThemeAspect.setSettingsKey(Utils::Key(Constants::ThemeKey));
    mThemeAspect.setDisplayName(tr("Theme:"));
    mThemeAspect.setDisplayStyle(Utils::SelectionAspect::DisplayStyle::ComboBox);
    for (QString theme: availableThemes())
        mThemeAspect.addOption(theme);

    /* TODO ensure this run configuration cannot be run with valgrind...
     * To do this, the code of the Valgrind plugin should be altered:
     * 1.ValgrindRunControlFactory should check the type of the given RunConfiguration (e.g. in canRun())
     * and addAspects() should only add aspects provided bu runnable RunControl factories.
     * 2.Alternatively, ValgrindPlugin, should ensure the extra aspects are added to
     * sensible RunConfiguration and RunConfiguration::addExtraAspects() should be removed. */
}

QString QtcRunConfiguration::pluginName(void) const
{
    return buildTargetInfo().projectFilePath.toFileInfo().baseName();
}

Utils::ProcessRunData QtcRunConfiguration::runnable(void) const
{
    Utils::ProcessRunData runnable;
    runnable.command = Utils::CommandLine(
        Utils::FilePath::fromString(QCoreApplication::applicationFilePath()),
        commandLineArgumentsList()
    );
    runnable.workingDirectory = mWorkingDirectoryAspect.value();
    if (macroExpander() != NULL)
        runnable.workingDirectory = macroExpander()->expand(runnable.workingDirectory);
    runnable.environment = mEnvironmentAspect.environment();
    return runnable;
}

QStringList QtcRunConfiguration::commandLineArgumentsList(void) const
{
    QStringList cmdArgs;

    QStringList themes = availableThemes();
    int themeIndex = static_cast<Utils::SelectionAspect*>(aspect(Utils::Id(Constants::ThemeId)))->value();
    if ((themeIndex >= 0) && (themeIndex < themes.size()))
        cmdArgs << QLatin1String("-theme") << themes[themeIndex];

    QString pluginsPath = buildTargetInfo().workingDirectory.nativePath();
    pluginsPath.replace(QLatin1Char('"'), QLatin1String("\\\""));
    if (pluginsPath.contains(QLatin1Char(' ')))
        pluginsPath.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
    cmdArgs << QLatin1String("-pluginpath") << pluginsPath;

    QString settingsPath = static_cast<PathAspect*>(aspect(Utils::Id(Constants::SettingsPathId)))->value().nativePath();
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

Utils::Id QtcRunConfiguration::id(void)
{
    return Utils::Id(Constants::QtcRunConfigurationId);
}

QString QtcRunConfiguration::displayNamePattern(void)
{
    return QCoreApplication::translate("QtcDevPlugin::Internal::QtcRunConfiguration", "Run Qt Creator with \"%1\"");
}

} // Internal
} // QtcDevPlugin

