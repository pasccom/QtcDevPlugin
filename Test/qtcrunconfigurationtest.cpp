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

#include "qtcrunconfigurationtest.h"
#include "testhelper.h"

#include "../pathaspect.h"
#include "../qtcrunconfiguration.h"
#include "../qtctestrunconfiguration.h"
#include "../qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/toolchain.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/icore.h>

#include <utils/processinterface.h>
#include <utils/theme/theme.h>

#include <QtTest>

namespace QtcDevPlugin {
namespace Test {

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

    return themes;
}

QStringList splitArgs(const QString& args)
{
    QStringList argList = args.split(QLatin1Char(' '));

    for (int a = 0; a < argList.size(); a++) {
        if (argList[a].startsWith(QLatin1Char('\''))) {
            while (!argList[a].endsWith(QLatin1Char('\'')) && (a + 1 < argList.size()))
                argList[a] = argList[a] + " " + argList.takeAt(a + 1);
            if (argList[a].endsWith(QLatin1Char('\'')))
                argList[a] = argList[a].mid(1).chopped(1);
        }
    }

    return argList;
}

void QtcRunConfigurationTest::initTestCase(void)
{
    QVERIFY(removeProjectUserFiles(Utils::FilePath(TESTS_DIR "/qMake/QtcPluginTest")));
}

void QtcRunConfigurationTest::cleanup(void)
{
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
}

void QtcRunConfigurationTest::testRestoreSettings_data(void)
{
    QTest::addColumn<Utils::FilePath>("projectPath");

    QTest::newRow("qMake project") << Utils::FilePath::fromString(TESTS_DIR "/qMake/QtcPluginTest/QtcPluginTest.pro");
    QTest::newRow("CMake project") << Utils::FilePath::fromString(TESTS_DIR "/cMake/QtcPluginTest/CMakeLists.txt");
}

void QtcRunConfigurationTest::testRestoreSettings(void)
{
    QFETCH(Utils::FilePath, projectPath);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);

    QStringList themes = availableThemes();
    QString theme = themes.first();
    if (Utils::creatorTheme()->displayName() == theme)
        theme = themes.last();

    for (ProjectExplorer::Target* target: mProject->targets()) {
        for (ProjectExplorer::RunConfiguration* runConfig: target->runConfigurations()) {
            if ((runConfig->id() != QtcDevPlugin::Constants::QtcRunConfigurationId) &&
                (runConfig->id() != QtcDevPlugin::Constants::QtcTestRunConfigurationId))
                continue;

            qDebug() << "Created run configuration" << runConfig->displayName()
                     << "for target" << target->displayName();

            QVERIFY(runConfig->aspect(QtcDevPlugin::Constants::ThemeId) != nullptr);
            QVERIFY(qobject_cast<Utils::SelectionAspect*>(runConfig->aspect(QtcDevPlugin::Constants::ThemeId)) != nullptr);
            static_cast<Utils::SelectionAspect*>(runConfig->aspect(QtcDevPlugin::Constants::ThemeId))->setValue(1);

            QVERIFY(runConfig->aspect(QtcDevPlugin::Constants::SettingsPathId) != nullptr);
            QVERIFY(qobject_cast<QtcDevPlugin::Internal::PathAspect*>(runConfig->aspect(QtcDevPlugin::Constants::SettingsPathId)) != nullptr);
            static_cast<QtcDevPlugin::Internal::PathAspect*>(runConfig->aspect(QtcDevPlugin::Constants::SettingsPathId))->setValue(Utils::FilePath::fromString("."));

            QVERIFY(runConfig->aspect(QtcDevPlugin::Constants::WorkingDirectoryId) != nullptr);
            QVERIFY(qobject_cast<QtcDevPlugin::Internal::PathAspect*>(runConfig->aspect(QtcDevPlugin::Constants::WorkingDirectoryId)) != nullptr);
            static_cast<QtcDevPlugin::Internal::PathAspect*>(runConfig->aspect(QtcDevPlugin::Constants::WorkingDirectoryId))->setValue(Utils::FilePath::fromString("."));

            Utils::ProcessRunData runnable = runConfig->runnable();
            QCOMPARE(runnable.workingDirectory, Utils::FilePath("."));

            QStringList args = splitArgs(runnable.command.arguments());
            qDebug() << "Command line arguments:" << args;

            int themeIndex = args.indexOf(QLatin1String("-theme"));
            QVERIFY(themeIndex != -1);
            QVERIFY(themeIndex + 1 < args.size());
            QCOMPARE(args.at(themeIndex + 1), theme);

            int settingsIndex = args.indexOf(QLatin1String("-settingspath"));
            QVERIFY(settingsIndex != -1);
            QVERIFY(settingsIndex + 1 < args.size());
            QCOMPARE(args.at(settingsIndex + 1), QLatin1String("."));
        }
    }

    closeProject(mProject);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);

    for (ProjectExplorer::Target* target: mProject->targets()) {
        for (ProjectExplorer::RunConfiguration* runConfig: target->runConfigurations()) {
            if ((runConfig->id() != QtcDevPlugin::Constants::QtcRunConfigurationId) &&
                (runConfig->id() != QtcDevPlugin::Constants::QtcTestRunConfigurationId))
                continue;

            qDebug() << "Restored run configuration" << runConfig->displayName()
                     << "for target" << target->displayName();

            Utils::ProcessRunData runnable = runConfig->runnable();
            QCOMPARE(runnable.workingDirectory, Utils::FilePath("."));

            QStringList args = splitArgs(runnable.command.arguments());
            qDebug() << "Command line arguments:" << args;

            int themeIndex = args.indexOf(QLatin1String("-theme"));
            QVERIFY(themeIndex != -1);
            QVERIFY(themeIndex + 1 < args.size());
            QCOMPARE(args.at(themeIndex + 1), theme);

            int settingsIndex = args.indexOf(QLatin1String("-settingspath"));
            QVERIFY(settingsIndex != -1);
            QVERIFY(settingsIndex + 1 < args.size());
            QCOMPARE(args.at(settingsIndex + 1), QLatin1String("."));
        }
    }
}

} // Test
} // QtcDevPlugin
