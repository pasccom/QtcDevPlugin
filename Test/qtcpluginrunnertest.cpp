/* Copyright 2020 Pascal COMBES <pascom@orange.fr>
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

#include "qtcpluginrunnertest.h"
#include "testhelper.h"

#include "../qtcrunconfiguration.h"
#include "../qtctestrunconfiguration.h"

#include <qtsupport/qtkitinformation.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>

#include <QtTest>

namespace QtcDevPlugin {
namespace Test {


void QtcPluginRunnerTest::initTestCase(void)
{
    QDir projectPath(TESTS_DIR "/QtcPluginTest");

    // Install the project:
    QProcess makeProcess(this);
    makeProcess.setWorkingDirectory(projectPath.absolutePath());
    makeProcess.start("make", QStringList() << "install");
    QVERIFY2(makeProcess.waitForFinished(), "Failed to execute \"make install\" for QtcPluginTest. Please do it manually");

    QVERIFY(removeProjectUserFiles(projectPath.absolutePath()));
    QVERIFY(openQMakeProject(projectPath.absoluteFilePath("QtcPluginTest.pro"), &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath.absoluteFilePath("QtcPluginTest.pro"));

    QList<ProjectExplorer::Target*> targets = mProject->targets();
    foreach (ProjectExplorer::Target* target, targets) {
        QtSupport::BaseQtVersion *qtVersion = QtSupport::QtKitAspect::qtVersion(target->kit());
        if (qtVersion->qtVersion().majorVersion < 5)
            mProject->removeTarget(target);
    }
}

void QtcPluginRunnerTest::testRunner_data(void)
{
    QTest::addColumn<Core::Id>("runModeId");
    QTest::addColumn<Core::Id>("runConfigId");
    QTest::addColumn<QString>("runControlDisplayName");

    QTest::newRow("Normal run") << Core::Id(ProjectExplorer::Constants::NORMAL_RUN_MODE) << Core::Id(QtcDevPlugin::Constants::QtcRunConfigurationId) << "Run Qt Creator with \"QtcPluginTest\"";
    //QTest::newRow("Debug run") << Core::Id(ProjectExplorer::Constants::DEBUG_RUN_MODE) << Core::Id(QtcDevPlugin::Constants::QtcRunConfigurationId) << "Run Qt Creator with \"QtcPluginTest\"";
    QTest::newRow("Normal test") << Core::Id(ProjectExplorer::Constants::NORMAL_RUN_MODE) << Core::Id(QtcDevPlugin::Constants::QtcTestRunConfigurationId) << "Run Qt Creator tests for \"QtcPluginTest\"";
    //QTest::newRow("Debug test") << Core::Id(ProjectExplorer::Constants::DEBUG_RUN_MODE) << Core::Id(QtcDevPlugin::Constants::QtcTestRunConfigurationId) << "Run Qt Creator tests for \"QtcPluginTest\"";
}

void QtcPluginRunnerTest::qtcRunConfiguration(const Core::Id& runConfigId, QtcDevPlugin::Internal::QtcRunConfiguration** qtcRunConfig)
{
    BEGIN_SUB_TEST_FUNCTION

    ProjectExplorer::RunConfiguration* runConfig = Utils::findOrDefault(mProject->activeTarget()->runConfigurations(), [runConfigId](ProjectExplorer::RunConfiguration* rc) {
        return rc->id() == runConfigId;
    });
    QVERIFY(runConfig != nullptr);
    QVERIFY(runConfig->id() == runConfigId);

    QVERIFY(qtcRunConfig != nullptr);
    *qtcRunConfig = static_cast<QtcDevPlugin::Internal::QtcRunConfiguration*>(runConfig);

    END_SUB_TEST_FUNCTION
}

void QtcPluginRunnerTest::testRunner(void)
{
    QFETCH(Core::Id, runModeId);
    QFETCH(Core::Id, runConfigId);
    QFETCH(QString, runControlDisplayName);

    QtcDevPlugin::Internal::QtcRunConfiguration* qtcRunConfig;
    SUB_TEST_FUNCTION(qtcRunConfiguration(runConfigId, &qtcRunConfig));
    mProject->activeTarget()->setActiveRunConfiguration(qtcRunConfig);
    QCOMPARE(mProject->activeTarget()->activeRunConfiguration(), qtcRunConfig);

    Utils::FilePath targetInstallPath = qtcRunConfig->targetFilePath();
    qDebug() << targetInstallPath;
    QVERIFY2(targetInstallPath.toFileInfo().exists(), "Please ensure QtcPluginTest is installed before running tests");

    QSignalSpy runControlAboutToStartSpy(ProjectExplorer::ProjectExplorerPlugin::instance(),
                                    SIGNAL(aboutToExecuteRunControl(ProjectExplorer::RunControl*, Core::Id)));
    ProjectExplorer::ProjectExplorerPlugin::runProject(mProject, runModeId);
    QVERIFY2(runControlAboutToStartSpy.wait(10000), "Project takes too long to start");
    QCOMPARE(runControlAboutToStartSpy.size(), 1);
    QCOMPARE(runControlAboutToStartSpy.at(0).size(), 2);
    QCOMPARE(runControlAboutToStartSpy.at(0).at(1).value<Core::Id>(), runModeId);
    mRunControl = runControlAboutToStartSpy.at(0).at(0).value<ProjectExplorer::RunControl*>();
    QCOMPARE(mRunControl->displayName(), runControlDisplayName);

    QVERIFY(!targetInstallPath.toFileInfo().exists());

    if (!mRunControl->isRunning()) {
        QSignalSpy runControlStartedSpy(mRunControl, SIGNAL(started()));
        QVERIFY2(runControlStartedSpy.wait(15000), "Run control takes too long to start");
    }

    QSignalSpy runControlStoppedSpy(mRunControl, SIGNAL(stopped()));
    mRunControl->initiateStop();
    QVERIFY2(runControlStoppedSpy.wait(15000), "Run control takes too long to stop");

    QVERIFY(targetInstallPath.toFileInfo().exists());
}

void QtcPluginRunnerTest::cleanup(void)
{
    if ((mRunControl != nullptr) && !mRunControl->isStopped()) {
        QSignalSpy runControlStoppedSpy(mRunControl, SIGNAL(stopped()));
        mRunControl->initiateStop();
        if (!runControlStoppedSpy.wait()) {
            mRunControl->forceStop();
            QVERIFY(runControlStoppedSpy.wait());
        }
    }

    mRunControl = nullptr;
}

void QtcPluginRunnerTest::cleanupTestCase(void)
{
    if (mProject != NULL)
        closeProject(mProject);
}

} // Test
} // QtcDevPlugin
