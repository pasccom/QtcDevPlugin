#include "qtcrunworkerfactory.h"

#include "qtcrunworker.h"
#include "qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorerconstants.h>

namespace QtcDevPlugin {
namespace Internal {

QtcRunWorkerFactory::QtcRunWorkerFactory()
    : ProjectExplorer::RunWorkerFactory()
{
    addSupportedRunMode(ProjectExplorer::Constants::NORMAL_RUN_MODE);
    addSupportedRunMode(ProjectExplorer::Constants::DEBUG_RUN_MODE);

    addSupportedDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);

    addSupportedRunConfig(Utils::Id(Constants::QtcRunConfigurationId));
    addSupportedRunConfig(Utils::Id(Constants::QtcTestRunConfigurationId));

    setProduct<QtcRunWorker>();
}

} // Internal
} // QtcDevPlugin
