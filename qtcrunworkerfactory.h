#ifndef QTCRUNWORKERFACTORY_H
#define QTCRUNWORKERFACTORY_H

#include "qtcdevpluginconstants.h"

#include <projectexplorer/runcontrol.h>
#include <projectexplorer/projectexplorerconstants.h>

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief The QtcRunWorkerFactory class creates QtcRunWorker for run configurations
 * associated with Qt Creator plugins.
 *
 * This class support normal and debug run modes on desktop devices.
 *
 * \sa QtcRunConfiguration, QtcTestRunConfiguration
 */
class QtcRunWorkerFactory : public ProjectExplorer::RunWorkerFactory
{
public:
    /*!
     * \brief Functor specifying the tasks to execute a run control.
     *
     * This typedef describes a function specifying how to execute a run control.
     */
    typedef std::function<Tasking::Group(ProjectExplorer::RunControl*)> ReceipeProducer;
    /*!
     * \brief Constructor
     *
     * Constructs an new run worker factory instance for run configurations
     * associated with Qt Creator plugins.
     * \param runMode Run mode
     * \param baseReceipe Describes the base tasks to execute the run control.
     * This class simply adds setup and teardown actions.
     */
    QtcRunWorkerFactory(Utils::Id runMode, const ReceipeProducer& baseReceipe);
private:
    /*!
     * \brief Moves the plugin file
     *
     * Moves the plugin file (obtained by the target install path of the run configuration)
     * from one suffixed path to the other.
     * This allows to delete and undelete easily the plugin from Qt Creator plugin path.
     * \param targetPath The path to the target to rename.
     * \param oldSuffix The current suffix of the target.
     * \param newSuffix The desired suffix of the target.
     * \sa handleRunControlStarted(), handleRunControlStopped()
     */
    void movePluginFile(const Utils::FilePath& targetPath, const QString& oldSuffix, const QString& newSuffix);
    /*!
     * \brief List of plugin paths
     *
     * This function returns the list of paths to plugins named accordingly.
     * \param fileName The name of the plugin file.
     * \return The list of paths to plugins.
     */
    std::list<Utils::FilePath> pluginPaths(const QString& fileName);
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNWORKERFACTORY_H
