/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
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

#ifndef QTCDEVPLUGIN_H
#define QTCDEVPLUGIN_H

#include "qtcdevplugin_global.h"

#include <extensionsystem/iplugin.h>
#include <utils/fileutils.h>

namespace ProjectExplorer {
    class RunConfigurationFactory;
    class RunControl;
}

namespace QtcDevPlugin {
namespace Internal {
    class QtcRunConfiguration;

/*!
 * \mainpage QtcDev plugin: Qt Creator plugin to develop plugins.
 *
 * \section features Features
 * Here is a list of the current features of the plugin (see also \ref future for further devopments):
 *  \li Allows to start Qt Creator with the current version of the plugin loaded
 *  \li Allows to test the current version of the plugin
 *  \li Auto-detection of plugin build output dir (\c DESTDIR) and install dir
 *  \li Tuning theme settings path and working directory of test instance
 *
 * \section future Planned developments
 * Here are some ideas I plan to implement later:
 *  \li Select test(s) to run
 *  \li Full configuration of the test Qt Creator instance
 *  \li Selection of the loaded plugin for the test Qt Creator instance
 *
 * If you have any other feature you will be interested in, please let me know.
 * I will be pleased to develop it if I think it is a must have.
 *
 * If you want to implement extension, also tell me please. Admittedly you
 * can do what you desire with the code (under the constraints stated in the
 * section \ref license below), but this will avoid double work.
 *
 * \section compat Compatibity table
 *
 *  | QtcDevPlugin | Qt Creator | Linux | Windows | Mac OS |
 *  |:-------------|:-----------|:-----:|:-------:|:------:|
 *  | 0.0.1        | 3.6.0      |  `Y`  |   `N`   |  `?`   |
 *  | 0.0.2        | 3.6.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.3        | 4.0.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.4        | 4.2.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.5        | 4.3.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.6        | 4.4.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.6        | 4.4.1      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.7        | 4.5.0      |  `Y`  |   `Y`   |  `?`   |
 *  | 0.0.8        | 4.6.0      |  `P`  |   `P`   |  `?`   |
 *  | 0.0.9        | 4.7.0      |  `P`  |   `P`   |  `?`   |
 *  | 0.0.10       | 4.8.0      |  `P`  |   `P`   |  `?`   |
 *  | 0.0.11       | 4.9.0      |  `P`  |   `P`   |  `?`   |
 *  | 0.0.12       | 4.10.0     |  `P`  |   `P`   |  `?`   |
 *  | 0.0.13       | 4.11.0     |  `P`  |   `P`   |  `?`   |
 *  | 0.0.14       | 4.12.0     |  `P`  |   `P`   |  `?`   |
 *
 * A \c Y means that QtcDevPlugin was tested against this Qt Creator version
 * under the given operating system. A \c N means that the version of the plugin
 * cannot work properly for the given operating system. A `P` means that
 * the sources should be patched (see \c patches/ folder) for the plugin to work
 * under the given operating system. A \c ? means that I currently do not know
 * whether it works.
 *
 * \section license Licensing information
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
 * along with QTestLibPlugin. If not, see http://www.gnu.org/licenses/
 */

/*!
 * \brief The QtcDeveloperPlugin class is the main class of QtcDevPlugin.
 *
 * It implements the plugin functionnality
 * (see Qt Creator developper documentation for details).
 *
 * It also contains the slots which manage the available versions of the
 * plugin being developped (relying on the information contained in
 * QtcRunConfiguration and QtcTestRunConfiguration).
 *
 * It proposes tests for the functionnality of the whole plugin.
 */
class QtcDeveloperPlugin : public ExtensionSystem::IPlugin
{
#ifndef DOXYGEN
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtcDevPlugin.json")
#endif
public:
    /*!
     * \brief Constructor
     *
     * Does not do anything presently.
     */
    QtcDeveloperPlugin();
    /*!
     * \brief Destructor
     *
     * Does not do anything presently.
     */
    ~QtcDeveloperPlugin();

    /*!
     * \brief Plugin initialisation method
     *
     * This function is in charge of registering objects in Qt Creator object pool.
     * It creates:
     *  \li The run configuration factories
     *  \li Install the plugin translator.
     *
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \param arguments Arguments passed to Qt Creator through the command line.
     * \param errorString If there is an error and if the pointer is not empty,
     * then it is set to a description of the problem.
     * \return \c true if the plugin initialisation suceeded, \c false otherwise.
     * \sa extensionsInitialized()
     */
    bool initialize(const QStringList &arguments, QString *errorString) override;
    /*!
     * \brief Listener for dependant plugin initialisation.
     *
     * This function is currently unused (as this plugin cannot have dependencies).
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \sa initialize()
     */
    void extensionsInitialized() override;
    /*!
     * \brief Listener for shutdown
     *
     * This function is currently unused
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \return \c SynchronousShutdown in all cases.
     */
    ShutdownFlag aboutToShutdown() override;
#ifdef BUILD_TESTS
    /*!
     * \brief Creates tests instances
     *
     * This method creates instances of test classes.
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \return A list of instances of test classes.
     */
    QVector<QObject *> createTestObjects(void) const override;
#endif
private slots:
    /*!
     * \brief Slot called on run control start.
     *
     * This slot is called whenever a run control is started.
     * It checks if the associated run configuration is a QtcRunConfiguration
     * of a QtcTestRunConfiguration. If this is the case, it ensures that old
     * versions of the plugin are removed from Qt Creator plugin path
     * before starting Qt Creator with the right command line arguments.
     * \param runControl The run control which is starting.
     * \sa handleRunControlStopped()
     */
    void handleRunControlStarted(ProjectExplorer::RunControl* runControl);
    /*!
     * \brief Slot called on run control end.
     *
     * This slot is called whenever a run control enters stopped state.
     * It restores old versions of the plugin (which have been removed by
     * handleRunControlStarted()). The sender is expected to be
     * the run control which is finishing.
     * \sa handleRunControlStarted()
     */
    void handleRunControlStopped();
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

    QLinkedList<Utils::FilePath> pluginPaths(const QString& fileName);

    QList<ProjectExplorer::RunConfigurationFactory*> mRunConfigurationFactories; /*!< List of run configuration factories created by this plugin (for deletion) */
};

} // namespace Internal
} // namespace QtcDevPlugin

#endif // QTCDEVPLUGIN_H
