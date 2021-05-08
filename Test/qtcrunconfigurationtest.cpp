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
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>
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
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
}

void QtcRunConfigurationTest::testRestoreSettings(void)
{
    QString projectPath(TESTS_DIR "/QtcPluginTest/QtcPluginTest.pro");

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    QString theme = "Classic";
    if (Utils::creatorTheme()->displayName() == theme)
        theme = "Dark";

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
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
