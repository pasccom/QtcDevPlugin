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

namespace ProjectExplorer {
    class RunControl;
}

namespace QtcDevPlugin {
namespace Internal {

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
 *
 * A \c Y means that QtcDevPlugin was tested against this Qt Creator version
 * under the given operating system. A \c N means that the version of the plugin
 * cannot work properly for the given operating system. A \c ? means that I
 * currently do not know whether it works.
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
    bool initialize(const QStringList &arguments, QString *errorString);
    /*!
     * \brief Listener for dependant plugin initialisation.
     *
     * This function is currently unused (as this plugin cannot have dependencies).
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \sa initialize()
     */
    void extensionsInitialized();
    /*!
     * \brief Listener for shutdown
     *
     * This function is currently unused
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \return \c SynchronousShutdown in all cases.
     */
    ShutdownFlag aboutToShutdown();
#ifdef BUILD_TESTS
    /*!
     * \brief Creates tests instances
     *
     * This method creates instances of test classes.
     * \note This function of this method is extensively described in Qt Creator developper documentation.
     * \return A list of instances of test classes.
     */
    QList<QObject *> createTestObjects(void) const;
#endif
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
     * \sa handleRunControlEnd()
     */
    void handleRunControlStart(ProjectExplorer::RunControl* runControl);
    /*!
     * \brief Slot called on run control end.
     *
     * This slot is called whenever a run control finishes.
     * It restores old versions of the plugin (which have been removed by
     * handleRunControlStart()).
     * \param runControl The run control which is finishing.
     * \sa handleRunControlStart()
     */
    void handleRunControlEnd(ProjectExplorer::RunControl* runControl);
};

} // namespace Internal
} // namespace QtcDevPlugin

#endif // QTCDEVPLUGIN_H
