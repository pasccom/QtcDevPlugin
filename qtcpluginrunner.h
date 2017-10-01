#ifndef QTCPLUGINRUNNER_H
#define QTCPLUGINRUNNER_H

#include <projectexplorer/runconfiguration.h>

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief The QtcPluginRunner class is a worker for running Qt Creator with the right plugins.
 *
 * This class implements a worker in charge of removing the old plugins
 * from Qt Creator plugin directories and putting them there back when
 * it stops.
 */

class QtcPluginRunner : public ProjectExplorer::SimpleTargetRunner
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Does not do anything presently.
     */
    inline QtcPluginRunner(ProjectExplorer::RunControl* runControl) :
        ProjectExplorer::SimpleTargetRunner(runControl) {}
protected:
    /*!
     * \brief Starts the worker
     *
     * This function is in charge of starting the runner.
     * It first calls handleRunControlStarted(), connects the stopped() signal
     * of the associated run control to handleRunControlStopped() and let
     * the worker start.
     */
    void start(void) override;
private slots:
    /*!
     * \brief Slot called on run control start.
     *
     * This slot is called whenever a run control is started.
     * It checks if the associated run configuration is a QtcRunConfiguration
     * of a QtcTestRunConfiguration. If this is the case, it ensures that old
     * versions of the plugin are removed from Qt Creator plugin path
     * before startin Qt Creator with the right command line arguments.
     * \param runControl The run control which is starting.
     * \sa handleRunControlStopped()
     */
    void handleRunControlStarted(void);
    /*!
     * \brief Slot called on run control end.
     *
     * This slot is called whenever a run control enters stopped state.
     * It restores old versions of the plugin (which have been removed by
     * handleRunControlStarted()).
     * \param runControl The run control which is finishing.
     * \sa handleRunControlStarted()
     */
    void handleRunControlStopped(void);
};

} // Internal
} // QtcDevPlugin

#endif // QTCPLUGINRUNNER_H
