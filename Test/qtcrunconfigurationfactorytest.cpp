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

#include "qtcrunconfigurationfactorytest.h"
#include "testhelper.h"

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

#include <utils/processinterface.h>
#include <utils/theme/theme.h>

#include <QLinkedList> // WARNING obsolete
#include <QtTest>

namespace QtcDevPlugin {
namespace Test {

typedef struct {
    QString name;
    QString buildKey;
} ExpectedPluginInfo;

void QtcRunConfigurationFactoryTest::initTestCase(void)
{
    QList<Utils::FilePath> projectPathes;

    // NOTE _data() function is not available for initTestCase()
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/ProjectTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/QtcPluginTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/OneSubTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/TwoSubTests");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/TestAndPlugin");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/PluginAndTest");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/OneSubPlugin");
    projectPathes << Utils::FilePath::fromString(TESTS_DIR "/qMake/TwoSubPlugins");

    for (Utils::FilePath projectPath: projectPathes)
        QVERIFY(removeProjectUserFiles(projectPath));
}

void QtcRunConfigurationFactoryTest::cleanup(void)
{
    if (mProject != nullptr)
        QVERIFY(closeProject(mProject));
}

void QtcRunConfigurationFactoryTest::testOpenProject_data(void)
{
    QTest::addColumn<Utils::FilePath>("projectPath");
    QTest::addColumn< QList<ExpectedPluginInfo> >("qtcPlugins");

    QList<ExpectedPluginInfo> plugins;

    plugins.clear();
    QTest::newRow("ProjectTest - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/ProjectTest/ProjectTest.pro") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest"), QLatin1String(TESTS_DIR "/qMake/QtcPluginTest/QtcPluginTest.pro")};
    QTest::newRow("QtcPluginTest - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/QtcPluginTest/QtcPluginTest.pro") << plugins;
    plugins.clear();
    QTest::newRow("OneSubTest - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/OneSubTest/OneSubTest.pro") << plugins;
    plugins.clear();
    QTest::newRow("TwoSubTests - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/TwoSubTests/TwoSubTests.pro") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest2"), QLatin1String(TESTS_DIR "/qMake/QtcPluginTest2/QtcPluginTest2.pro")};
    QTest::newRow("TestAndPlugin - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/TestAndPlugin/TestAndPlugin.pro") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String(TESTS_DIR "/qMake/QtcPluginTest1/QtcPluginTest1.pro")};
    QTest::newRow("PluginAndTest - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/PluginAndTest/PluginAndTest.pro") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String(TESTS_DIR "/qMake/QtcPluginTest1/QtcPluginTest1.pro")};
    QTest::newRow("OneSubPlugin - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/OneSubPlugin/OneSubPlugin.pro") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String(TESTS_DIR "/qMake/QtcPluginTest1/QtcPluginTest1.pro")};
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest2"), QLatin1String(TESTS_DIR "/qMake/QtcPluginTest2/QtcPluginTest2.pro")};
    QTest::newRow("TwoSubPlugins - qMake") << Utils::FilePath::fromString(TESTS_DIR "/qMake/TwoSubPlugins/TwoSubPlugins.pro") << plugins;

    plugins.clear();
    QTest::newRow("ProjectTest - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/ProjectTest/ProjectTest.pro") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest"), QLatin1String("QtcPluginTest")};
    QTest::newRow("QtcPluginTest - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/QtcPluginTest/CMakeLists.txt") << plugins;
    plugins.clear();
    QTest::newRow("OneSubTest - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/OneSubTest/CMakeLists.txt") << plugins;
    plugins.clear();
    QTest::newRow("TwoSubTests - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/TwoSubTests/CMakeLists.txt") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest2"), QLatin1String("QtcPluginTest2")};
    QTest::newRow("TestAndPlugin - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/TestAndPlugin/CMakeLists.txt") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String("QtcPluginTest1")};
    QTest::newRow("PluginAndTest - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/PluginAndTest/CMakeLists.txt") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String("QtcPluginTest1")};
    QTest::newRow("OneSubPlugin - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/OneSubPlugin/CMakeLists.txt") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String("QtcPluginTest1")};
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest2"), QLatin1String("QtcPluginTest2")};
    QTest::newRow("TwoSubPlugins - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/TwoSubPlugins/CMakeLists.txt") << plugins;
    plugins.clear();
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest1"), QLatin1String("QtcPluginTest1")};
    plugins << ExpectedPluginInfo{QLatin1String("QtcPluginTest2"), QLatin1String("QtcPluginTest2")};
    QTest::newRow("TwoQtcPlugins - CMake") << Utils::FilePath::fromString(TESTS_DIR "/cMake/TwoQtcPlugins/CMakeLists.txt") << plugins;

}

void QtcRunConfigurationFactoryTest::testOpenProject(void)
{
    QFETCH(Utils::FilePath, projectPath);
    QFETCH(QList<ExpectedPluginInfo>, qtcPlugins);

    QVERIFY(openQMakeProject(projectPath, &mProject));
    QCOMPARE(mProject->projectFilePath(), projectPath);

    for (ProjectExplorer::Target* target: mProject->targets()) {
        QLinkedList<Internal::QtcRunConfiguration*> qtcRunConfigs;
        QLinkedList<Internal::QtcTestRunConfiguration*> qtcTestRunConfigs;
        for (ProjectExplorer::RunConfiguration* runConfig: target->runConfigurations()) {
            Internal::QtcRunConfiguration* qtcRunConfig = qobject_cast<Internal::QtcRunConfiguration*>(runConfig);
            Internal::QtcTestRunConfiguration* qtcTestRunConfig = qobject_cast<Internal::QtcTestRunConfiguration*>(runConfig);

            if (qtcTestRunConfig != nullptr)
                qtcTestRunConfigs << qtcTestRunConfig;
            else if (qtcRunConfig != nullptr)
                qtcRunConfigs << qtcRunConfig;
        }

        QStringList qtcPluginsFound;
        for (Internal::QtcRunConfiguration* qtcRunConfig: qtcRunConfigs) {
            bool ok = false;
            for (ExpectedPluginInfo pluginInfo: qtcPlugins) {
                if (QString::compare(qtcRunConfig->buildTargetInfo().buildKey, pluginInfo.buildKey, Qt::CaseSensitive) == 0) {
                    ok = true;
                    qtcPluginsFound << pluginInfo.name;
                    break;
                }
            }
            QVERIFY2(ok, qPrintable(QString(QLatin1String("Unexpected run configuration: %1")).arg(qtcRunConfig->id().suffixAfter(Utils::Id(Constants::QtcRunConfigurationId)))));

            QCOMPARE(qtcRunConfig->displayName(), QString(QLatin1String("Run Qt Creator with \"%1\"")).arg(qtcPluginsFound.last()));

            QStringList args = qtcRunConfig->commandLineArgumentsList();
            for (QStringList::Iterator it = args.begin(); it != args.end(); it++) {
                if ((*it).contains(' '))
                    *it = "'" + *it + "'";
            }
            Utils::ProcessRunData qtcRunnable = qtcRunConfig->runnable();
            QCOMPARE(qtcRunnable.command.executable(), Utils::FilePath::fromString(QCoreApplication::applicationFilePath()));
            Utils::FilePath workingDirectory = target->activeBuildConfiguration()->buildDirectory();
            // workingDirectory.replace(QLatin1Char('/'), QDir::separator());
            QCOMPARE(qtcRunnable.workingDirectory, workingDirectory);
            QCOMPARE(qtcRunnable.command.arguments(), args.join(QLatin1Char(' ')));

            int themeIndex = args.indexOf(QLatin1String("-theme"));
            QVERIFY(themeIndex != -1);
            QVERIFY(themeIndex + 1 < args.size());
            QString themeArg = args.at(themeIndex + 1);
            if (themeArg.startsWith("'") && themeArg.endsWith("'"))
                themeArg = themeArg.mid(1).chopped(1);
            QCOMPARE(themeArg, Utils::creatorTheme()->displayName());

            int pluginIndex = args.indexOf(QLatin1String("-pluginpath"));
            QVERIFY(pluginIndex != -1);
            QVERIFY(pluginIndex + 1 < args.size());
            QString pluginArg = args.at(pluginIndex + 1);
            if (pluginArg.startsWith("'") && pluginArg.endsWith("'"))
                pluginArg = pluginArg.mid(1).chopped(1);
            qDebug() << pluginArg;
            // FIXME QCOMPARE(pluginArg, QString(QLatin1String(TESTS_DIR "/%1/bin")).arg(qtcPluginsFound.last()));
        }
        QCOMPARE(qtcPlugins.size(), qtcPluginsFound.size());

        QStringList qtcTestPluginsFound;
        for (Internal::QtcTestRunConfiguration* qtcTestRunConfig: qtcTestRunConfigs) {
            bool ok = false;
            for (ExpectedPluginInfo pluginInfo: qtcPlugins) {
                if (QString::compare(qtcTestRunConfig->buildTargetInfo().buildKey, pluginInfo.buildKey, Qt::CaseSensitive) == 0) {
                    ok = true;
                    qtcTestPluginsFound << pluginInfo.name;
                    break;
                }
            }
            QVERIFY2(ok, qPrintable(QString(QLatin1String("Unexpected run configuration: %1")).arg(qtcTestRunConfig->id().suffixAfter(Utils::Id(Constants::QtcTestRunConfigurationId)))));

            QCOMPARE(qtcTestRunConfig->displayName(), QString(QLatin1String("Run Qt Creator tests for \"%1\"")).arg(qtcTestPluginsFound.last()));

            QStringList args = qtcTestRunConfig->commandLineArgumentsList();
            for (QStringList::Iterator it = args.begin(); it != args.end(); it++) {
                if ((*it).contains(' '))
                    *it = "'" + *it + "'";
            }
            Utils::ProcessRunData qtcTestRunnable = qtcTestRunConfig->runnable();
            QCOMPARE(qtcTestRunnable.command.executable(), Utils::FilePath::fromString(QCoreApplication::applicationFilePath()));
            Utils::FilePath workingDirectory = target->activeBuildConfiguration()->buildDirectory();
            // workingDirectory.replace(QLatin1Char('/'), QDir::separator());
            QCOMPARE(qtcTestRunnable.workingDirectory, workingDirectory);
            QCOMPARE(qtcTestRunnable.command.arguments(), args.join(QLatin1Char(' ')));

            int testIndex = args.indexOf(QLatin1String("-test"));
            QVERIFY(testIndex != -1);
            QVERIFY(testIndex + 1 < args.size());
            QCOMPARE(args.at(testIndex + 1), qtcTestPluginsFound.last());

            int loadIndex = args.indexOf(QLatin1String("-load"));
            QVERIFY(loadIndex != -1);
            QVERIFY(loadIndex + 1 < args.size());
            QCOMPARE(args.at(loadIndex + 1), QLatin1String("all"));

            int themeIndex = args.indexOf(QLatin1String("-theme"));
            QVERIFY(themeIndex != -1);
            QVERIFY(themeIndex + 1 < args.size());
            QString themeArg = args.at(themeIndex + 1);
            if (themeArg.startsWith("'") && themeArg.endsWith("'"))
                themeArg = themeArg.mid(1).chopped(1);
            QCOMPARE(themeArg, Utils::creatorTheme()->displayName());

            int pluginIndex = args.indexOf(QLatin1String("-pluginpath"));
            QVERIFY(pluginIndex != -1);
            QVERIFY(pluginIndex + 1 < args.size());
            QString pluginArg = args.at(pluginIndex + 1);
            if (pluginArg.startsWith("'") && pluginArg.endsWith("'"))
                pluginArg = pluginArg.mid(1).chopped(1);
            qDebug() << pluginArg;
            // FIXME QCOMPARE(pluginArg, QString(QLatin1String(TESTS_DIR "/%1/bin")).arg(qtcTestPluginsFound.last()));
        }
        QCOMPARE(qtcPlugins.size(), qtcTestPluginsFound.size());
    }
}

} // Test
} // QtcDevPlugin
