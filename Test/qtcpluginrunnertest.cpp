#include "qtcpluginrunnertest.h"
#include "testhelper.h"

#include "../qtcrunconfiguration.h"
#include "../qtctestrunconfiguration.h"

#include <qtsupport/qtkitinformation.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>

#include <coreplugin/id.h>

#include <QtTest>

namespace QtcDevPlugin {
namespace Test {


void QtcPluginRunnerTest::initTestCase(void)
{
    QDir projectPath(TESTS_DIR "/QtcPluginTest");

    QVERIFY(removeProjectUserFiles(projectPath.absolutePath()));
    QVERIFY(openQMakeProject(projectPath.absoluteFilePath("QtcPluginTest.pro"), &mProject));
    QCOMPARE(mProject->projectFilePath().toString(), projectPath.absoluteFilePath("QtcPluginTest.pro"));

    QList<ProjectExplorer::Target*> targets = mProject->targets();
    foreach (ProjectExplorer::Target* target, targets) {
        QtSupport::BaseQtVersion *qtVersion = QtSupport::QtKitInformation::qtVersion(target->kit());
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
    QTest::newRow("Debug test") << Core::Id(ProjectExplorer::Constants::DEBUG_RUN_MODE) << Core::Id(QtcDevPlugin::Constants::QtcTestRunConfigurationId) << "Run Qt Creator tests for \"QtcPluginTest\"";
}

void QtcPluginRunnerTest::testRunner(void)
{
    QFETCH(Core::Id, runModeId);
    QFETCH(Core::Id, runConfigId);
    QFETCH(QString, runControlDisplayName);

    ProjectExplorer::RunConfiguration* runConfig = Utils::findOrDefault(mProject->activeTarget()->runConfigurations(), [runConfigId](ProjectExplorer::RunConfiguration* rc) {
        return rc->id() == runConfigId;
    });
    QVERIFY(runConfig != nullptr);
    QVERIFY(runConfig->id() == runConfigId);
    mProject->activeTarget()->setActiveRunConfiguration(runConfig);
    QCOMPARE(mProject->activeTarget()->activeRunConfiguration(), runConfig);

    QtcDevPlugin::Internal::QtcRunConfiguration* qtcRunConfig = static_cast<QtcDevPlugin::Internal::QtcRunConfiguration*>(runConfig);
    QString targetInstallPath = qtcRunConfig->installPath().appendPath(qtcRunConfig->targetName().toString()).toString();
    qDebug() << targetInstallPath;
    QVERIFY2(QFile(targetInstallPath).exists(), "Please install QtcPluginTest before running tests");

    QSignalSpy runControlAboutToStartSpy(ProjectExplorer::ProjectExplorerPlugin::instance(),
                                    SIGNAL(aboutToExecuteRunControl(ProjectExplorer::RunControl*, Core::Id)));
    ProjectExplorer::ProjectExplorerPlugin::runProject(mProject, runModeId);
    QVERIFY2(runControlAboutToStartSpy.wait(10000), "Project takes too long to start");
    QCOMPARE(runControlAboutToStartSpy.size(), 1);
    QCOMPARE(runControlAboutToStartSpy.at(0).size(), 2);
    QCOMPARE(runControlAboutToStartSpy.at(0).at(1).value<Core::Id>(), runModeId);
    mRunControl = runControlAboutToStartSpy.at(0).at(0).value<ProjectExplorer::RunControl*>();
    QCOMPARE(mRunControl->displayName(), runControlDisplayName);

    QVERIFY(!QFile(targetInstallPath).exists());

    if (!mRunControl->isRunning()) {
        QSignalSpy runControlStartedSpy(mRunControl, SIGNAL(started()));
        QVERIFY2(runControlStartedSpy.wait(15000), "Run control takes too long to start");
    }

    QSignalSpy runControlStoppedSpy(mRunControl, SIGNAL(stopped()));
    mRunControl->initiateStop();
    QVERIFY2(runControlStoppedSpy.wait(15000), "Run control takes too long to stop");

    QVERIFY(QFile(targetInstallPath).exists());
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
