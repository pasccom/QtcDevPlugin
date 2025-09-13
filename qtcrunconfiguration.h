/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef QTCRUNCONFIGURATION_H
#define QTCRUNCONFIGURATION_H

#include "qtcdevpluginconstants.h"
#include "pathaspect.h"

#include <projectexplorer/environmentaspect.h>
#include <projectexplorer/projectconfiguration.h>
#include <projectexplorer/runconfiguration.h>

#include <utils/fileutils.h>
#include <utils/aspects.h>

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief The QtcRunConfiguration class stores information required to start an instance of Qt Creator
 *
 * When an instance of this class is added to a target, Qt Creator will propose to the user to
 * start Qt Creator with the plugin being developped loaded. It also tries to ensure that all other
 * versions of the plugin are hidden (by moving them to alternative pathes) when Qt Creator stats,
 * so that the current version is the only loaded in the current Qt Creator instance.
 *
 * This run configuration can be easily edited using QtcRunConfigurationWidget, which
 * defines a suitable form wigdet to ease this process.
 *
 * \sa QtcRunConfigurationWidget
 */
class QtcRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Creates a new instance with parent target.
     * \param parent The parent build configuration
     * \param id The id for the run configuration
     * \sa initialize()
     */
    QtcRunConfiguration(ProjectExplorer::BuildConfiguration* parent, Utils::Id id = Utils::Id(Constants::QtcRunConfigurationId));

    /*!
     * \brief Command-line arguments list
     *
     * Returns the list of the command-line arguments to be passed to Qt Creator executable.
     * \return The command-line arguments to be passed to the Qt Creator instance
     */
    virtual QStringList commandLineArgumentsList(void) const;

    /*!
     * \brief The runnable for this runconfiguration
     *
     * Returns the runnable for this runconfiguration, which contains
     *  - The executable
     *  - The command line arguments
     *  - The environment
     *  - The working directory
     *
     * \return The runnable for this runconfiguration
     */
    virtual Utils::ProcessRunData runnable(void) const override;

    /*!
     * \brief The path of the plugin when installed
     *
     * Returns the path of plugin library file when it has been installed,
     * which is the path to the installed target.
     * \return The path where of plugin library file when installed
     * \sa pluginName()
     */
    inline Utils::FilePath targetFilePath(void) const {return buildTargetInfo().targetFilePath;}

    /*!
     * \brief The name of the plugin
     *
     * Returns the name of the plugin: This is the project file name without the *.pro extension.
     * \return  The name of the plugin.
     * \sa targetFilePath()
     */
    QString pluginName(void) const;

    /*!
     * \brief The run configuration id
     *
     * This function returns the id for instances of this class.
     * \return The run configuration id
     */
    static Utils::Id id(void);

    /*!
     * \brief The run configuration pattern for display name
     *
     * This function returns the pattern used to generate the display name for the run configuration.
     * If %1 is used, it will be replaced by the plugin name.
     * \return The pattern for the display name of the run configuration.
     */
    static QString displayNamePattern(void);
private:
    PathAspect mWorkingDirectoryAspect{this};
    PathAspect mSettingsPathAspect{this};
    Utils::SelectionAspect mThemeAspect{this};
    ProjectExplorer::EnvironmentAspect mEnvironmentAspect{this};
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATION_H
