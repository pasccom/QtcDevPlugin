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

#include "qtcrunconfigurationtest.h"
#include "testhelper.h"

#include "../qtcrunconfiguration.h"
#include "../qtctestrunconfiguration.h"
#include "../qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/toolchain.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <QtTest>

namespace QtcDevPlugin {
namespace Test {

void QtcRunConfigurationTest::initTestCase(void)
{
    QVERIFY(removeProjectUserFiles(QLatin1String(TESTS_DIR "/QtcPluginTest")));
}

void QtcRunConfigurationTest::cleanup(void)
{
    if (mProject != NULL)
        closeProject(mProject);
}

void QtcRunConfigurationTest::testRestoreSettings(void)
{
    QString projectPath(TESTS_DIR "/QtcPluginTest/QtcPluginTest.pro");

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    QString theme = "default";
    if (Utils::creatorTheme()->displayName() == theme)
        theme = "dark";

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
            if ((runConfig->id() != QtcDevPlugin::Constants::QtcRunConfigurationId) &&
                (runConfig->id() != QtcDevPlugin::Constants::QtcTestRunConfigurationId))
                continue;

            qDebug() << "Created run configuration" << runConfig->displayName()
                     << "for target" << target->displayName();

            QVariantMap map = runConfig->toMap();
            map.insert(QtcDevPlugin::Constants::ThemeKey, theme);
            map.insert(QtcDevPlugin::Constants::SettingsPathKey, QLatin1String("."));
            map.insert(QtcDevPlugin::Constants::WorkingDirectoryKey, QLatin1String("."));
            runConfig->fromMap(map);

            ProjectExplorer::Runnable runnable = runConfig->runnable();
            QCOMPARE(runnable.workingDirectory, QLatin1String("."));

            QStringList args = runnable.commandLineArguments.split(QLatin1Char(' '));

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
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
            if ((runConfig->id() != QtcDevPlugin::Constants::QtcRunConfigurationId) &&
                (runConfig->id() != QtcDevPlugin::Constants::QtcTestRunConfigurationId))
                continue;

            qDebug() << "Restored run configuration" << runConfig->displayName()
                     << "for target" << target->displayName();

            ProjectExplorer::Runnable runnable = runConfig->runnable();
            QCOMPARE(runnable.workingDirectory, QLatin1String("."));

            QStringList args = runnable.commandLineArguments.split(QLatin1Char(' '));
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
