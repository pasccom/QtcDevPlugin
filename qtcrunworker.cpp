#include "qtcrunworker.h"

#include <extensionsystem/pluginmanager.h>

namespace QtcDevPlugin {
namespace Internal {

void QtcRunWorker::start()
{
    qDebug() << "Starting" << runControl()->displayName();

    movePluginFile(runControl()->targetFilePath(), QString(), QLatin1String(".del"));
    for (Utils::FilePath pluginFilePath: pluginPaths(runControl()->targetFilePath().fileName()))
        movePluginFile(pluginFilePath, QString(), QLatin1String(".del"));

    ProjectExplorer::RunWorker::start();
}

void QtcRunWorker::stop()
{
    ProjectExplorer::RunWorker::stop();

    qDebug() << "End of" << runControl()->displayName();

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
