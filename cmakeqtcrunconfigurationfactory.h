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

#ifndef CMAKEQTCRUNCONFIGURATIONFACTORY_H
#define CMAKEQTCRUNCONFIGURATIONFACTORY_H

#include "qtcdevpluginconstants.h"

#include "qtcrunconfiguration.h"
#include "qtctestrunconfiguration.h"
#include "qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>

#include <cmakeprojectmanager/cmakeprojectconstants.h>

#include <utils/algorithm.h>

#include <QtCore>
#include <QtDebug>

#include <optional>

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief Run configuration factory for CMake-based Qt Creator plugins
 *
 * This class is in charge of finding out whether a project is supported
 * by QtcRunConfiguration. For this is uses the standard methods provided by
 * ProjectExplorer::IRunConfigurationFactory (i.e. project type id and
 * target device id.
 *
 * It handles only desktop targets and is usefull only for \c CMake projects
 * using the CMake function `add_qtc_plugin` to declare Qt Crator plugins. When this
 * macro is used multiple times, multiple run configurations can be created (one for each
 * declared Qt Creator plugin.
 *
 * \tparam RunConfiguration The run configuration type which is produced by the factory.
 * The type must have the following static functions:
 *   - id(): Returns the run configuration id;
 *   - displayNamePattern(): Returns the pattern used for the run configuration display name.
 *
 * \sa QtcRunConfiguration
 */
template <class RunConfiguration>
class CMakeQtcRunConfigurationFactory : public ProjectExplorer::RunConfigurationFactory
{
private:
    /*!
     * \brief Information about a Qt Creator plugin
     *
     * This structure stores information about a Qt Creator plugin
     * extracted from CMake file API.
     */
    typedef struct {
        Utils::FilePath projectFilePath;    /*!< Path to the Qt Creator plugin project file */
        Utils::FilePath targetFilePath;     /*!< Path to the built Qt Creator plugin target file */
        Utils::FilePath targetBuildPath;    /*!< CMake build path for the Qt Creator plugin target */
    } QtcPluginInfo;
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run configuration factory instance
     * for QMake projects with desktop target.
     */
    CMakeQtcRunConfigurationFactory(void);

    /*!
     * \brief The build targets this factory can create
     *
     * This function returns a list of the build targets this factory can create.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param bc The build configuration of the future run configuration.
     * \return A list of the build targets this factory can create.
     * \sa canCreate()
     */
    virtual QList<ProjectExplorer::RunConfigurationCreationInfo> availableCreators(ProjectExplorer::BuildConfiguration* bc) const override;

    /*!
     * \brief Whether the project is ready for examination
     *
     * Returns \c true when the given project is ready for examination by isUseful().
     * \param project A (CMake-based) project
     * \return \c true if the given project is ready for examination, \c false otherwise.
     */
    static bool isReady(ProjectExplorer::Project* project);

private:
    /*!
     * \brief Get Qt Creator plugin information
     *
     * Parse the given target file in CMake File API tree (queried by Qt Creator)
     * and returs information about an eventual Qt Creator plugin.
     * \param bc Build configuration associated with a CMake-based project
     * \param cMakeTargetFilePath Path to the target file in CMake file API tree
     * \return Information about the Qt Creator plugin if any, or \c std::nullopt.
     * \sa qtCreatorPlugins()
     */
    static std::optional<QtcPluginInfo> qtCreatorPlugin(ProjectExplorer::BuildConfiguration* bc, const Utils::FilePath& cMakeTargetFilePath);
    /*!
     * \brief Recursively find Qt Creator plugins.
     *
     * Search the given node for Qt Creator plugins and
     * return the list of the found Qt Creator plugin projects
     * together with information about the plugins.
     * \param bc Build configuration associated with a CMake-based project
     * \param root Node restricting the search
     * \return A map whose keys are plugins names and values are
     * information about the corresponding Qt Creator plugin.
     * \sa qtCreatorPlugin()
     */
    static QMap<QString, QtcPluginInfo> qtCreatorPlugins(ProjectExplorer::BuildConfiguration* bc, ProjectExplorer::FolderNode* root = nullptr);

    /*!
     * \brief Path to CMake file API reply tree
     *
     * This function returns the path to CMake file API reply tree for the given project.
     * This tree is queried by Qt Creator.
     * \param project A (CMake-based) project
     * \return The path to CMake file API reply tree.
     * \sa cMakeTargetFilePath()
     */
    static Utils::FilePath cMakeApiPath(ProjectExplorer::Project* project);
    /*!
     * \brief Path to CMake file API target file
     *
     * This function returns the path to CMake file API target file for the given project
     * and the given target.
     * \param project A (CMake-based) project
     * \param targetName The name of a target in the project.
     * \return
     * \sa cMakeTargetFilePath()
     */
    static Utils::FilePath cMakeTargetFilePath(ProjectExplorer::Project* project, const QString& targetName);
};

template <class RunConfiguration>
CMakeQtcRunConfigurationFactory<RunConfiguration>::CMakeQtcRunConfigurationFactory(void) :
    ProjectExplorer::RunConfigurationFactory()
{
    addSupportedProjectType(CMakeProjectManager::Constants::CMAKE_PROJECT_ID);
    addSupportedTargetDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
    registerRunConfiguration<RunConfiguration>(RunConfiguration::id());
}

template <class RunConfiguration>
QList<ProjectExplorer::RunConfigurationCreationInfo> CMakeQtcRunConfigurationFactory<RunConfiguration>::availableCreators(ProjectExplorer::BuildConfiguration* bc) const
{
    QList<ProjectExplorer::RunConfigurationCreationInfo> creators;
    qDebug() << "availableCreators()" << isReady(bc->project());

    if (bc->buildSystem() == nullptr)
        return creators;
    if (!isReady(bc->project()))
        return creators;

    auto buildInfos = bc->buildSystem()->applicationTargets();
    auto qtcPluginInfos = qtCreatorPlugins(bc);
    for (ProjectExplorer::BuildTargetInfo info : buildInfos)
        qDebug() << "BuildTargetInfo:" << info.displayName << info.buildKey << info.projectFilePath << info.workingDirectory << info.targetFilePath;
    for (QString pluginName: qtcPluginInfos.keys()) {
        ProjectExplorer::BuildTargetInfo info = bc->buildSystem()->buildTarget(pluginName);

        if (info.buildKey != pluginName) {
            qDebug() << __func__ << "Creating:" << qtcPluginInfos.value(pluginName).projectFilePath << qtcPluginInfos.value(pluginName).targetFilePath << qtcPluginInfos.value(pluginName).targetBuildPath;

            info.displayName = pluginName;
            info.buildKey = pluginName;
            info.projectFilePath = qtcPluginInfos.value(pluginName).projectFilePath;
            info.targetFilePath = qtcPluginInfos.value(pluginName).targetFilePath;
            info.workingDirectory = qtcPluginInfos.value(pluginName).targetBuildPath;
            buildInfos << info;
        }

        ProjectExplorer::RunConfigurationCreationInfo creator;
        creator.factory = this;
        creator.buildKey = info.buildKey;
        creator.displayName = RunConfiguration::displayNamePattern().arg(info.displayName);

        creators << creator;
    }
    bc->buildSystem()->setApplicationTargets(buildInfos);

    return creators;
}

template <class RunConfiguration>
Utils::FilePath CMakeQtcRunConfigurationFactory<RunConfiguration>::cMakeApiPath(ProjectExplorer::Project* project)
{
    if (project == nullptr)
        return Utils::FilePath();
    if (project->activeBuildConfiguration() == nullptr)
        return Utils::FilePath();

    Utils::FilePath apiDirectory = project->activeBuildConfiguration()->buildDirectory().pathAppended(".cmake/api/v1/reply");
    qDebug() << "CMake API dir:" << apiDirectory;
    return apiDirectory;
}

template <class RunConfiguration>
Utils::FilePath CMakeQtcRunConfigurationFactory<RunConfiguration>::cMakeTargetFilePath(ProjectExplorer::Project* project, const QString& targetName)
{
    Utils::FilePath apiPath(cMakeApiPath(project));
    if (!apiPath.isDir())
        return Utils::FilePath();

    Utils::FilePath cMakeTargetFilePath;
    Utils::FileFilter cMakeCacheFilter(QStringList() << QLatin1String("target-%1-*.json").arg(targetName), QDir::Files);
    for (Utils::FilePath file : apiPath.dirEntries(cMakeCacheFilter, QDir::Time)) {
        if (!cMakeTargetFilePath.isEmpty())
            qWarning() << "Multiple target files. Using newest one";
        cMakeTargetFilePath = file;
    }

    qDebug() << "CMake target path for" << targetName << ":" << cMakeTargetFilePath;
    return cMakeTargetFilePath;
}

template <class RunConfiguration>
bool CMakeQtcRunConfigurationFactory<RunConfiguration>::isReady(ProjectExplorer::Project* project)
{
    if (!cMakeApiPath(project).isDir())
        return false;
    if (project->rootProjectNode() == nullptr)
        return false;

    bool cMakeListsFound = false;
    bool targetFound = false;

    for (ProjectExplorer::Node* child : project->rootProjectNode()->nodes()) {
        if (child->displayName().contains(QLatin1String("_autogen"), Qt::CaseSensitive))
            continue;
        if (QString::compare(child->displayName(), QLatin1String("CMakeLists.txt"), Qt::CaseSensitive) == 0)
            cMakeListsFound = true;
        else
            targetFound = true;
    }
    return cMakeListsFound && targetFound;
}

template <class RunConfiguration>
std::optional<typename CMakeQtcRunConfigurationFactory<RunConfiguration>::QtcPluginInfo> CMakeQtcRunConfigurationFactory<RunConfiguration>::qtCreatorPlugin(ProjectExplorer::BuildConfiguration* bc, const Utils::FilePath& cMakeTargetFilePath)
{
    if (!cMakeTargetFilePath.isFile())
        return std::nullopt;

    Utils::Result<QByteArray> cMakeTargetData = cMakeTargetFilePath.fileContents();
    QTC_CHECK_RESULT(cMakeTargetData);
    QJsonObject cMakeTargetFile = QJsonDocument::fromJson(*cMakeTargetData).object();

    QJsonArray commands = cMakeTargetFile.value(QLatin1String("backtraceGraph")).toObject().value(QLatin1String("commands")).toArray();
    if (Utils::allOf(commands, [](const QJsonValue& value) {
        return QString::compare(value.toString(), QLatin1String("add_qtc_plugin"), Qt::CaseSensitive) != 0;
    }))
        return std::nullopt;

    if (cMakeTargetFile.value(QLatin1String("artifacts")).toArray().isEmpty())
        return std::nullopt;

    if (cMakeTargetFile.value(QLatin1String("artifacts")).toArray().size() > 1)
        qWarning() << "More than one artifact in " << cMakeTargetFilePath;

    Utils::FilePath projectFilePath = bc->project()->projectFilePath().parentDir();
    QString subProjectPath = cMakeTargetFile.value(QLatin1String("paths")).toObject()
                                            .value(QLatin1String("source")).toString();
    if (QString::compare(subProjectPath, QLatin1String(".")) != 0)
        projectFilePath = projectFilePath.pathAppended(subProjectPath);

    Utils::FilePath targetFilePath = Utils::FilePath::fromString(cMakeTargetFile.value(QLatin1String("artifacts")).toArray()
                                                                                .at(0).toObject()
                                                                                .value(QLatin1String("path")).toString());
    Utils::FilePath targetInstallPath = Utils::FilePath::fromString(cMakeTargetFile.value(QLatin1String("install")).toObject()
                                                                                   .value(QLatin1String("destinations")).toArray()
                                                                                   .at(0).toObject()
                                                                                   .value(QLatin1String("path")).toString());

    return std::make_optional<QtcPluginInfo>({
        .projectFilePath = projectFilePath,
        .targetFilePath = targetInstallPath.pathAppended(targetFilePath.fileName()),
        .targetBuildPath = bc->buildDirectory(),
    });
}

template <class RunConfiguration>
QMap<QString, typename CMakeQtcRunConfigurationFactory<RunConfiguration>::QtcPluginInfo> CMakeQtcRunConfigurationFactory<RunConfiguration>::qtCreatorPlugins(ProjectExplorer::BuildConfiguration* bc, ProjectExplorer::FolderNode* root)
{
    QMap<QString, typename CMakeQtcRunConfigurationFactory<RunConfiguration>::QtcPluginInfo> qtcPlugins;

    if (root == nullptr)
        root = bc->project()->rootProjectNode()->asFolderNode();
    QTC_ASSERT(root != nullptr, return qtcPlugins);

    for (ProjectExplorer::Node* child : root->nodes()) {
        if (child->displayName().contains(QLatin1String("_autogen"), Qt::CaseSensitive))
            continue;
        qDebug() << child->displayName() << child->isProjectNodeType() << child->isFolderNodeType() << child->isVirtualFolderType() << child->isGenerated();
        if (QString::compare(child->displayName(), QLatin1String("CMakeLists.txt"), Qt::CaseSensitive) == 0)
            continue;

        if (child->isFolderNodeType()) {
            auto subPlugins = qtCreatorPlugins(bc, child->asFolderNode());
            for (auto it = subPlugins.begin(); it != subPlugins.end(); it++)
                qtcPlugins.insert(it.key(), it.value());
        } else if (child->isProjectNodeType()) {
            auto qtcPluginInfo = qtCreatorPlugin(bc, cMakeTargetFilePath(bc->project(), child->displayName()));
            if (qtcPluginInfo.has_value())
                qtcPlugins.insert(child->displayName(), *qtcPluginInfo);
        }
    }

    qDebug() << "Plugin names:" << qtcPlugins.keys().join(QLatin1String(", "));

    return qtcPlugins;
}

} // Internal
} // QtcDevPlugin

#endif // CMAKEQTCRUNCONFIGURATIONFACTORY_H
