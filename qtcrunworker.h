#ifndef QTCRUNWORKER_H
#define QTCRUNWORKER_H

#include <projectexplorer/runcontrol.h>

#include <utils/qtcprocess.h>

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief The QtcRunWorker instances represent run workers for Qt Creator plugins.
 *
 * Instances of this class ensure that the old plugins will not be loaded
 * when starting the run configuration by renaming them with `.del` extension.
 * After the program stops, it restores the old plugins version.
 *
 * \sa QtcRunWorkerFactory
 */
class QtcRunWorker : public ProjectExplorer::RunWorker // FIXME would be interesting to inherit from ProjectExplorer::ProcessRunner
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run worker instance for Qt Creator plugins
     * run configurations.
     * \param runControl The associated run control.
     */
    QtcRunWorker(ProjectExplorer::RunControl* runControl);
protected:
    /*!
     * \brief Start the run worker
     *
     * This function is called when the run worker needs to be started.
     * It will rename the old plugin versions so that they are not loaded.
     *
     * \sa stop()
     */
    virtual void start() override;
    /*!
     * \brief Stop the run worker
     *
     * This function is called when the run worker needs to be stopped.
     * It will restore the old plugin versions so that they can be loaded later.
     *
     * \sa start()
     */
    virtual void stop() override;
private slots:
    /*!
     * \brief Slot to be called on process start
     *
     * This slot should be called when the underlying process is started.
     * It gets the process PID and report process startup.
     */
    void handleStarted(void);
    /*!
     * \brief Slot to be called on process termination
     *
     * This slot should be called when the underlying process terminates.
     * It reports the process termination and restores the plugin file.
     */
    void handleDone(void);
    /*!
     * \brief Slot to be called on process message
     *
     * This slot should be called when data is available on the underlying
     * process standard output.
     * \sa handleStdErr()
     */
    void handleStdOut(void);
    /*!
     * \brief Slot to be called on process error message
     *
     * This slot should be called when data is available on the underlying
     * process standard error.
     * \sa handleStdOut()
     */
    void handleStdErr(void);
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

    Utils::Process mProcess;    /*!< Handle to underlying process */
    bool mStopReported;         /*!< Process termination has been reported */
    bool mStopForced;           /*!< Process termination was externally forced */
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNWORKER_H
