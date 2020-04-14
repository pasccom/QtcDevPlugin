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

#ifndef QTCRUNCONFIGURATIONFACTORY_H
#define QTCRUNCONFIGURATIONFACTORY_H

#include <projectexplorer/runconfiguration.h>

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QmakeProjectManager {
    class QmakeProFileNode;
    class QmakeProFile;
}

namespace QtcDevPlugin {
namespace Internal {

class QtcRunConfiguration;

/*!
 * \brief The QtcRunConfigurationFactory class creates QtcRunConfiguration for supported \c qMake projects.
 *
 * This class is in charge of find out whether a project is supported
 * by QtcRunConfiguration. For this is uses the standard methods provided by
 * ProjectExplorer::IRunConfigurationFactory (i.e. project type id and
 * target device id.
 *
 * It handles only desktop targets and is usefull only for \c qMake projects
 * including the project include for Qt Crator plugins. When this file is included
 * by multiple subprojects multiple run configurations can be created (one for each
 * sub project including Qt Creator plugin.
 *
 * \sa QtcRunConfiguration
 */
class QtcRunConfigurationFactory : public ProjectExplorer::RunConfigurationFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run configuration factory instance
     * for QMake projects with desktop target.
     * \param registerRunConfig Whether to register run configuration (to allow inheritence)
     */
    QtcRunConfigurationFactory(bool registerRunConfig = true);
    /*!
     * \brief The build targets this factory can create
     *
     * This function returns a list of the build targets this factory can create.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent The target of the future run configuration.
     * \return A list of the build targets this factory can create.
     * \sa canCreate()
     */
    virtual QList<ProjectExplorer::RunConfigurationCreationInfo> availableCreators(ProjectExplorer::Target *parent) const override;

    /*!
     * \brief Whether the project is ready for examination
     *
     * Returns \c true when the given project is ready for examination by isUseful().
     * \param project A project
     * \return \c true if the given project is ready for examination, \c false otherwise.
     */
    static bool isReady(ProjectExplorer::Project* project);
    /*!
     * \brief Whether QtcRunConfiguration or QtcTestRunConfiguration is useful for the given project
     *
     * Returns \c true when a QtcRunConfiguration or a QtcTestRunConfiguration is useful for the given project,
     * i.e when it canHandle() the associated target,
     * the project is a \c qMake project including at least one time
     * the project include for Qt Creator plugins.
     * \param project A project
     * \return \c true when a QtcRunConfiguration or a QtcTestRunConfiguration is useful for the given project.
     * \sa canHandle(), canCreate(), canRestore(), canClone()
     */
    static bool isUseful(ProjectExplorer::Project* project);

    /*!
     * \brief Set display name pattern
     *
     * Sets the pattern used for generating a display name for this run configuration.
     * If %1 is used, it will be replaced by the plugin name.
     * \param pattern The display name pattern
     */
    inline void setDisplayNamePattern(const QString& pattern) {mDisplayNamePattern = pattern;}
private:

    /*!
     * \brief Find Qt Creator plugin project include
     *
     * Finds recursively the project include file for Qt Creator plugins
     * in the given project node and it files and folder children.
     * \param node A project node
     * \return \c true if the project include for Qt Creator plugins can be found and \c false otherwise
     * \sa isQtCreatorPlugin(), hasQtCreatorPlugin(), qtCreatorPlugins()
     */
    static bool findQtcPluginPri(ProjectExplorer::ProjectNode* node);
    /*!
     * \brief Whether the project is a Qt Creator plugin
     *
     * Tells whether the project is a Qt Creator plugin.
     * It first chesk whether this is a \c qMake sahred library project
     * and then tries to findQtcPluginPri().
     * \param node A project node
     * \return \c true if the given project node is a Qt Creator plugin project and \c false otherwise
     * \sa findQtcPluginPri(), hasQtCreatorPlugin(), qtCreatorPlugins()
     */
    static bool isQtCreatorPlugin(ProjectExplorer::ProjectNode* node);
    /*!
     * \brief Whether the project contains a Qt Creator plugins.
     *
     * Search the given project node and its project children to find
     * a Qt Creator plugin.
     * \param node A project node
     * \return \c true if the given project node or one of its project children
     * is a Qt Creator plugin and \c false otherwise
     * \sa findQtcPluginPri(), isQtCreatorPlugin(), qtCreatorPlugins()
     */
    static bool hasQtCreatorPlugin(ProjectExplorer::ProjectNode* node);
    /*!
     * \brief Fild all Qt Creator plugin sub projects.
     *
     * Search the given project node and its project children for Qt Creator plugins
     * and return the list of the found Qt Creator plugin projects.
     * \param node A project node
     * \return The list of the Qt Creator plugin sub projects.
     * \sa findQtcPluginPri(), isQtCreatorPlugin(), hasQtCreatorPlugin()
     */
    static QList<ProjectExplorer::ProjectNode*> qtCreatorPlugins(ProjectExplorer::ProjectNode* node);
    /*!
     * \brief Path where the target is built
     *
     * Returns the path where the target of the given project file is built.
     * \param proFile A QMake project file.
     * \return The path where the target of the given project file is built.
     */
    static Utils::FilePath targetBuildPath(QmakeProjectManager::QmakeProFile* proFile);
    /*!
     * \brief Path where the target is installed
     *
     * Returns the path where the target of the given project file is installed.
     * \param proFile A QMake project file.
     * \return The path where the target of the given project file is installed.
     */
    static Utils::FilePath targetInstallPath(QmakeProjectManager::QmakeProFile* proFile);

    QString mDisplayNamePattern; /*!< The display name pattern */
};

/*!
 * \brief The QtcTestRunConfigurationFactory class creates QtcTestRunConfiguration for supported \c qMake projects.
 *
 * This class is in charge of finding out whether a project is supported
 * by QtcTestRunConfiguration. For this is uses the standard methods provided by
 * ProjectExplorer::IRunConfigurationFactory (i.e. project type id and
 * target device id.
 *
 * It handles only desktop targets and is usefull only for \c qMake projects
 * including the project include for Qt Crator plugins. When this file is included
 * by multiple subprojects multiple run configurations can be created (one for each
 * sub project including Qt Creator plugin.
 *
 * \sa QtcTestRunConfiguration
 */
class QtcTestRunConfigurationFactory : public QtcRunConfigurationFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run configuration factory instance
     * for QMake projects with desktop target.
     * \param registerRunConfig Whether to register run configuration (to allow inheritence)
     */
    QtcTestRunConfigurationFactory(bool registerRunConfig = true);
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATIONFACTORY_H
