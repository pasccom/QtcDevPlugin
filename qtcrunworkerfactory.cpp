#include "qtcrunworkerfactory.h"

#include "qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorerconstants.h>

#include <extensionsystem/pluginmanager.h>

namespace QtcDevPlugin {
namespace Internal {

QtcRunWorkerFactory::QtcRunWorkerFactory(Utils::Id runMode, const ReceipeProducer& baseReceipe)
    : ProjectExplorer::RunWorkerFactory()
{
    addSupportedRunMode(runMode);

    addSupportedDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);

    addSupportedRunConfig(Utils::Id(Constants::QtcRunConfigurationId));
    addSupportedRunConfig(Utils::Id(Constants::QtcTestRunConfigurationId));

    setProducer([this, baseReceipe] (ProjectExplorer::RunControl* runControl) {
        return new ProjectExplorer::RunWorker(runControl, Tasking::Group {
            Tasking::onGroupSetup([this, runControl] () {
                movePluginFile(runControl->targetFilePath(), QString(), QLatin1String(".del"));
                for (Utils::FilePath pluginFilePath: pluginPaths(runControl->targetFilePath().fileName()))
                    movePluginFile(pluginFilePath, QString(), QLatin1String(".del"));
            }),
            Tasking::onGroupDone([this, runControl] () {
                movePluginFile(runControl->targetFilePath(), QLatin1String(".del"), QString());
                for (Utils::FilePath pluginFilePath: pluginPaths(runControl->targetFilePath().fileName()))
                    movePluginFile(pluginFilePath, QLatin1String(".del"), QString());
            }),
            baseReceipe(runControl)
        });
    });
}

std::list<Utils::FilePath> QtcRunWorkerFactory::pluginPaths(const QString& fileName)
{
    std::list<Utils::FilePath> ans;

    for (Utils::FilePath pluginPath: ExtensionSystem::PluginManager::pluginPaths())
        ans.push_back(pluginPath / fileName);

    return ans;
}

void QtcRunWorkerFactory::movePluginFile(const Utils::FilePath& targetPath, const QString& oldSuffix, const QString& newSuffix)
{
    Utils::FilePath oldTargetPath = Utils::FilePath(targetPath).stringAppended(oldSuffix);
    Utils::FilePath newTargetPath = Utils::FilePath(targetPath).stringAppended(newSuffix);

    qDebug() << "Before:" << oldTargetPath << oldTargetPath.exists()
                          << newTargetPath << newTargetPath.exists();

    if (oldTargetPath.exists()) {
        QTC_CHECK(QFile::rename(oldTargetPath.nativePath(), newTargetPath.nativePath()));
        qDebug() << "After: " << oldTargetPath << oldTargetPath.exists()
                              << newTargetPath << newTargetPath.exists();
    }
    QTC_CHECK(!oldTargetPath.exists());
}

} // Internal
} // QtcDevPlugin
