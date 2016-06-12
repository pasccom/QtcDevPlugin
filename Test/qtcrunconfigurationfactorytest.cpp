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

#include "qtcrunconfigurationfactorytest.h"
#include "testhelper.h"

#include "../qtcrunconfiguration.h"
#include "../qtctestrunconfiguration.h"
#include "../qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/runnables.h>
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

QtcRunConfigurationFactoryTest::QtcRunConfigurationFactoryTest(void):
    QObject(NULL), mProject(NULL)
{
}

void QtcRunConfigurationFactoryTest::initTestCase(void)
{
    QStringList projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << QLatin1String(TESTS_DIR "/ProjectTest");
    projectPathes << QLatin1String(TESTS_DIR "/QtcPluginTest");
    projectPathes << QLatin1String(TESTS_DIR "/OneSubTest");
    projectPathes << QLatin1String(TESTS_DIR "/TwoSubTests");
    projectPathes << QLatin1String(TESTS_DIR "/TestAndPlugin");
    projectPathes << QLatin1String(TESTS_DIR "/PluginAndTest");
    projectPathes << QLatin1String(TESTS_DIR "/OneSubPlugin");
    projectPathes << QLatin1String(TESTS_DIR "/TwoSubPlugins");

    foreach (QString projectPath, projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));
}

void QtcRunConfigurationFactoryTest::init(void)
{
    mProject = NULL;
}

void QtcRunConfigurationFactoryTest::cleanup(void)
{
    if (mProject != NULL)
        ProjectExplorer::SessionManager::removeProject(mProject);
}

void QtcRunConfigurationFactoryTest::testOpenProject_data(void)
{
    QTest::addColumn<QString>("projectPath");
    QTest::addColumn<QStringList>("qtcPlugins");

    QStringList plugins;

    plugins.clear();
    QTest::newRow("ProjectTest") << TESTS_DIR "/ProjectTest/ProjectTest.pro" << plugins;
    plugins.clear();
    plugins << QLatin1String("QtcPluginTest");
    QTest::newRow("QtcPluginTest") << TESTS_DIR "/QtcPluginTest/QtcPluginTest.pro" << plugins;
    plugins.clear();
    QTest::newRow("/OneSubTest") << TESTS_DIR "/OneSubTest/OneSubTest.pro" << plugins;
    plugins.clear();
    QTest::newRow("TwoSubTests") << TESTS_DIR "/TwoSubTests/TwoSubTests.pro" << plugins;
    plugins.clear();
    plugins << QLatin1String("QtcPluginTest2");
    QTest::newRow("TestAndPlugin") << TESTS_DIR "/TestAndPlugin/TestAndPlugin.pro" << plugins;
    plugins.clear();
    plugins << QLatin1String("QtcPluginTest1");
    QTest::newRow("PluginAndTest") << TESTS_DIR "/PluginAndTest/PluginAndTest.pro" << plugins;
    plugins.clear();
    plugins << QLatin1String("QtcPluginTest1");
    QTest::newRow("OneSubPlugin") << TESTS_DIR "/OneSubPlugin/OneSubPlugin.pro" << plugins;
    plugins.clear();
    plugins << QLatin1String("QtcPluginTest1") << QLatin1String("QtcPluginTest2");
    QTest::newRow("TwoSubPlugins") << TESTS_DIR "/TwoSubPlugins/TwoSubPlugins.pro" << plugins;
}

void QtcRunConfigurationFactoryTest::testOpenProject(void)
{
    QFETCH(QString, projectPath);
    QFETCH(QStringList, qtcPlugins);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath);

    foreach (ProjectExplorer::Target* target, mProject->targets()) {
        QLinkedList<Internal::QtcRunConfiguration*> qtcRunConfigs;
        QLinkedList<Internal::QtcTestRunConfiguration*> qtcTestRunConfigs;
        foreach (ProjectExplorer::RunConfiguration* runConfig, target->runConfigurations()) {
            Internal::QtcRunConfiguration* qtcRunConfig = qobject_cast<Internal::QtcRunConfiguration*>(runConfig);
            Internal::QtcTestRunConfiguration* qtcTestRunConfig = qobject_cast<Internal::QtcTestRunConfiguration*>(runConfig);

            if (qtcTestRunConfig != NULL)
                qtcTestRunConfigs << qtcTestRunConfig;
            else if (qtcRunConfig != NULL)
                qtcRunConfigs << qtcRunConfig;
        }

        QStringList qtcPluginsFound;
        foreach (Internal::QtcRunConfiguration* qtcRunConfig, qtcRunConfigs) {
            QVERIFY(!qtcRunConfig->id().suffixAfter(Core::Id(Constants::QtcRunConfigurationId)).isNull());

            bool ok = false;
            QString pluginProFilePath = qtcRunConfig->id().suffixAfter(Core::Id(Constants::QtcRunConfigurationId));
            foreach (QString qtcPlugin, qtcPlugins) {
                QDir expectedProFilePath(QLatin1String(TESTS_DIR));
                expectedProFilePath.cd(qtcPlugin);

                if (QString::compare(pluginProFilePath, expectedProFilePath.absoluteFilePath(qtcPlugin + QLatin1String(".pro")), Qt::CaseSensitive) == 0) {
                    ok = true;
                    qtcPluginsFound << qtcPlugin;
                    break;
                }
            }
            QVERIFY2(ok, qPrintable(QString(QLatin1String("Unexpected run configuration: %1")).arg(qtcRunConfig->id().suffixAfter(Core::Id(Constants::QtcRunConfigurationId)))));

            QCOMPARE(qtcRunConfig->displayName(), QString(QLatin1String("Run Qt Creator with \"%1\"")).arg(qtcPluginsFound.last()));
            QVERIFY(qtcRunConfig->runnable().is<ProjectExplorer::StandardRunnable>());

            ProjectExplorer::StandardRunnable qtcRunnable = qtcRunConfig->runnable().as<ProjectExplorer::StandardRunnable>();
            QCOMPARE(qtcRunnable.runMode, ProjectExplorer::ApplicationLauncher::Gui);
            QCOMPARE(qtcRunnable.executable, QCoreApplication::applicationFilePath());
            QString workingDirectory = target->activeBuildConfiguration()->buildDirectory().toString();
            workingDirectory.replace(QLatin1Char('/'), QDir::separator());
            QCOMPARE(qtcRunnable.workingDirectory, workingDirectory);
            QCOMPARE(qtcRunnable.commandLineArguments, qtcRunConfig->commandLineArgumentsList().join(QLatin1Char(' ')));

            int themeIndex = qtcRunConfig->commandLineArgumentsList().indexOf(QLatin1String("-theme"));
            QVERIFY(themeIndex != -1);
            QVERIFY(themeIndex + 1 < qtcRunConfig->commandLineArgumentsList().size());
            QCOMPARE(qtcRunConfig->commandLineArgumentsList().at(themeIndex + 1), Utils::creatorTheme()->displayName());

            int pluginIndex = qtcRunConfig->commandLineArgumentsList().indexOf(QLatin1String("-pluginpath"));
            QVERIFY(pluginIndex != -1);
            QVERIFY(pluginIndex + 1 < qtcRunConfig->commandLineArgumentsList().size());
            qDebug() << qtcRunConfig->commandLineArgumentsList().at(pluginIndex + 1);
            QCOMPARE(qtcRunConfig->commandLineArgumentsList().at(pluginIndex + 1), QString(QLatin1String(TESTS_DIR "/%1/./bin")).arg(qtcPluginsFound.last()));
        }
        QCOMPARE(qtcPlugins.size(), qtcPluginsFound.size());

        QStringList qtcTestPluginsFound;
        foreach (Internal::QtcTestRunConfiguration* qtcTestRunConfig, qtcTestRunConfigs) {
            QVERIFY(!qtcTestRunConfig->id().suffixAfter(Core::Id(Constants::QtcTestRunConfigurationId)).isNull());

            bool ok = false;
            QString pluginProFilePath = qtcTestRunConfig->id().suffixAfter(Core::Id(Constants::QtcTestRunConfigurationId));
            foreach (QString qtcPlugin, qtcPlugins) {
                QDir expectedProFilePath(QLatin1String(TESTS_DIR));
                expectedProFilePath.cd(qtcPlugin);

                if (QString::compare(pluginProFilePath, expectedProFilePath.absoluteFilePath(qtcPlugin + QLatin1String(".pro")), Qt::CaseSensitive) == 0) {
                    ok = true;
                    qtcTestPluginsFound << qtcPlugin;
                    break;
                }
            }
            QVERIFY2(ok, qPrintable(QString(QLatin1String("Unexpected run configuration: %1")).arg(qtcTestRunConfig->id().suffixAfter(Core::Id(Constants::QtcTestRunConfigurationId)))));

            QCOMPARE(qtcTestRunConfig->displayName(), QString(QLatin1String("Run Qt Creator tests for \"%1\"")).arg(qtcTestPluginsFound.last()));
            QVERIFY(qtcTestRunConfig->runnable().is<ProjectExplorer::StandardRunnable>());

            ProjectExplorer::StandardRunnable qtcTestRunnable = qtcTestRunConfig->runnable().as<ProjectExplorer::StandardRunnable>();
            QCOMPARE(qtcTestRunnable.runMode, ProjectExplorer::ApplicationLauncher::Gui);
            QCOMPARE(qtcTestRunnable.executable, QCoreApplication::applicationFilePath());
            QString workingDirectory = target->activeBuildConfiguration()->buildDirectory().toString();
            workingDirectory.replace(QLatin1Char('/'), QDir::separator());
            QCOMPARE(qtcTestRunnable.workingDirectory, workingDirectory);
            QCOMPARE(qtcTestRunnable.commandLineArguments, qtcTestRunConfig->commandLineArgumentsList().join(QLatin1Char(' ')));

            int themeIndex = qtcTestRunConfig->commandLineArgumentsList().indexOf(QLatin1String("-theme"));
            QVERIFY(themeIndex != -1);
            QVERIFY(themeIndex + 1 < qtcTestRunConfig->commandLineArgumentsList().size());
            QCOMPARE(qtcTestRunConfig->commandLineArgumentsList().at(themeIndex + 1), Utils::creatorTheme()->displayName());

            int pluginIndex = qtcTestRunConfig->commandLineArgumentsList().indexOf(QLatin1String("-pluginpath"));
            QVERIFY(pluginIndex != -1);
            QVERIFY(pluginIndex + 1 < qtcTestRunConfig->commandLineArgumentsList().size());
            qDebug() << qtcTestRunConfig->commandLineArgumentsList().at(pluginIndex + 1);
            QCOMPARE(qtcTestRunConfig->commandLineArgumentsList().at(pluginIndex + 1), QString(QLatin1String(TESTS_DIR "/%1/./bin")).arg(qtcTestPluginsFound.last()));

        }
        QCOMPARE(qtcPlugins.size(), qtcTestPluginsFound.size());
    }
}

} // Test
} // QtcDevPlugin
