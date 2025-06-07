#include "qtcrunworker.h"

#include <extensionsystem/pluginmanager.h>

#include <utils/processinterface.h>

namespace QtcDevPlugin {
namespace Internal {


QtcRunWorker::QtcRunWorker(ProjectExplorer::RunControl* runControl) :
    ProjectExplorer::RunWorker(runControl), mStopReported(false), mStopForced(false)
{
    connect(&mProcess, &Utils::Process::started, this, &QtcRunWorker::handleStarted);
    connect(&mProcess, &Utils::Process::readyReadStandardOutput, this, &QtcRunWorker::handleStdOut);
    connect(&mProcess, &Utils::Process::readyReadStandardError, this, &QtcRunWorker::handleStdErr);
    connect(&mProcess, &Utils::Process::done, this, &QtcRunWorker::handleDone);
}

void QtcRunWorker::start(void)
{
    qDebug() << "Starting" << runControl()->displayName();

    movePluginFile(runControl()->targetFilePath(), QString(), QLatin1String(".del"));
    for (Utils::FilePath pluginFilePath: pluginPaths(runControl()->targetFilePath().fileName()))
        movePluginFile(pluginFilePath, QString(), QLatin1String(".del"));

    const QString msg = tr("Starting %1...").arg(runControl()->commandLine().displayName());
    appendMessage(msg, Utils::NormalMessageFormat);

    mProcess.setCommand(runControl()->commandLine());
    mProcess.setEnvironment(runControl()->environment());

    //m_state = Run;
    mProcess.setWorkingDirectory(runControl()->workingDirectory());
    mProcess.setForceDefaultErrorModeOnWindows(true);
    mProcess.start();
}

void QtcRunWorker::handleStarted(void)
{
    Utils::ProcessHandle pid{mProcess.processId()};
    runControl()->setApplicationProcessHandle(pid);
    pid.activate();

    reportStarted();
}

void QtcRunWorker::handleStdOut(void)
{
    appendMessage(mProcess.readAllStandardOutput(), Utils::StdOutFormat, false);
}

void QtcRunWorker::handleStdErr(void)
{
    appendMessage(mProcess.readAllStandardError(), Utils::StdErrFormat, false);
}

void QtcRunWorker::stop(void)
{
    mStopForced = true;
    mProcess.stop();
}

void QtcRunWorker::handleDone(void)
{
    Utils::ProcessResultData resultData = mProcess.resultData();

    if (mStopReported)
        return;

    //m_state = Inactive;
    //m_waitForDoneTimer.stop();
    const QString executable = runControl()->commandLine().displayName();
    QString msg = tr("%1 exited with code %2").arg(executable).arg(resultData.m_exitCode);
    if (resultData.m_exitStatus == QProcess::CrashExit) {
        if (mStopForced)
            msg = tr("The process was ended forcefully.");
        else
            msg = tr("The process crashed.");
    } else if (resultData.m_error != QProcess::UnknownError) {
        msg = RunWorker::userMessageForProcessError(resultData.m_error, runControl()->commandLine().executable());
    }
    appendMessage(msg, Utils::NormalMessageFormat);

    mStopReported = true;
    reportStopped();

    movePluginFile(runControl()->targetFilePath(), QLatin1String(".del"), QString());
    for (Utils::FilePath pluginFilePath: pluginPaths(runControl()->targetFilePath().fileName()))
        movePluginFile(pluginFilePath, QLatin1String(".del"), QString());
}

std::list<Utils::FilePath> QtcRunWorker::pluginPaths(const QString& fileName)
{
    std::list<Utils::FilePath> ans;

    for (Utils::FilePath pluginPath: ExtensionSystem::PluginManager::pluginPaths())
        ans.push_back(pluginPath / fileName);

    return ans;
}

void QtcRunWorker::movePluginFile(const Utils::FilePath& targetPath, const QString& oldSuffix, const QString& newSuffix)
{
    Utils::FilePath oldTargetPath = Utils::FilePath(targetPath).stringAppended(oldSuffix);
    Utils::FilePath newTargetPath = Utils::FilePath(targetPath).stringAppended(newSuffix);

    qDebug() << oldTargetPath << oldTargetPath.exists() << newTargetPath << newTargetPath.exists();

    if (oldTargetPath.exists()) {
        QTC_CHECK(QFile::rename(oldTargetPath.nativePath(), newTargetPath.nativePath()));
    }
    QTC_CHECK(!oldTargetPath.exists());
}

} // Internal
} // QtcDevPlugin
