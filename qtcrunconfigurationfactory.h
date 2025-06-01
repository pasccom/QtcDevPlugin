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

#ifndef QTCRUNCONFIGURATIONFACTORY_H
#define QTCRUNCONFIGURATIONFACTORY_H

#include "qtcdevpluginconstants.h"

#include "qtcrunconfiguration.h"
#include "qtctestrunconfiguration.h"
#include "qtcdevpluginconstants.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/target.h>

#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakeprojectmanagerconstants.h>

#include <QtDebug>

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
 * \brief The BaseQtcRunConfigurationFactory class implements basic common functionnality for QtcRunConfigurationFactory and QtcTestRunConfigurationFactory.
 *
 * This class is in charge of finding out whether a project is supported
 * by QtcRunConfiguration. For this is uses the standard methods provided by
 * ProjectExplorer::IRunConfigurationFactory (i.e. project type id and
 * target device id.
 *
 * It handles only desktop targets and is usefull only for \c qMake projects
 * including the project include for Qt Crator plugins. When this file is included
 * by multiple subprojects multiple run configurations can be created (one for each
 * sub project including Qt Creator plugin.
 *
 * \tparam QtcRunConfiguration The run configuration type which is produced by the factory.
 * The type must have the following static functions:
 *   - id(): Returns the run configuration id;
 *   - displayNamePattern(): Returns the pattern used for the run configuration display name.
 *
 * \sa QtcRunConfiguration
 */
template <class RunConfiguration>
class QtcRunConfigurationFactory : public ProjectExplorer::RunConfigurationFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run configuration factory instance
     * for QMake projects with desktop target.
     */
    QtcRunConfigurationFactory(void);

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
};

template <class RunConfiguration>
QtcRunConfigurationFactory<RunConfiguration>::QtcRunConfigurationFactory(void) :
    ProjectExplorer::RunConfigurationFactory()
{
    addSupportedProjectType(QmakeProjectManager::Constants::QMAKEPROJECT_ID);
    addSupportedTargetDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
    registerRunConfiguration<RunConfiguration>(RunConfiguration::id());
}

template <class RunConfiguration>
QList<ProjectExplorer::RunConfigurationCreationInfo> QtcRunConfigurationFactory<RunConfiguration>::availableCreators(ProjectExplorer::Target *target) const
{
    QList<ProjectExplorer::RunConfigurationCreationInfo> creators;
    qDebug() << "availableCreators()" << isReady(target->project()) << isUseful(target->project());

    if (!isReady(target->project()) || !isUseful(target->project()))
        return creators;

    QList<ProjectExplorer::BuildTargetInfo> buildInfos = target->buildSystem()->applicationTargets();
    qDebug() << "BuildTargetInfoList size:" << buildInfos.size();
    for (ProjectExplorer::ProjectNode* node: qtCreatorPlugins(target->project()->rootProjectNode())) {
        QmakeProjectManager::QmakeProFileNode* qMakeNode = static_cast<QmakeProjectManager::QmakeProFileNode*>(node);

        QFileInfo proFileInfo = qMakeNode->filePath().toFileInfo();
        ProjectExplorer::BuildTargetInfo info = target->buildTarget(proFileInfo.canonicalFilePath());

        if (info.buildKey != proFileInfo.canonicalFilePath()) {
            qDebug() << __func__ << "Creating:" << proFileInfo.canonicalFilePath() << proFileInfo.baseName() << targetBuildPath(qMakeNode->proFile()) << targetInstallPath(qMakeNode->proFile());

            info.displayName = proFileInfo.baseName();
            info.buildKey = proFileInfo.canonicalFilePath();
            info.projectFilePath = qMakeNode->filePath();
            info.workingDirectory = targetBuildPath(qMakeNode->proFile());
            info.targetFilePath = targetInstallPath(qMakeNode->proFile());
            buildInfos << info;
        }

        ProjectExplorer::RunConfigurationCreationInfo creator;
        creator.factory = this;
        creator.buildKey = info.buildKey;
        creator.displayName = RunConfiguration::displayNamePattern().arg(info.displayName);

        creators << creator;
    }
    if (target->buildSystem() != nullptr)
        target->buildSystem()->setApplicationTargets(buildInfos);

    return creators;
}

template <class RunConfiguration>
Utils::FilePath QtcRunConfigurationFactory<RunConfiguration>::targetBuildPath(QmakeProjectManager::QmakeProFile* proFile)
{
    if (proFile->targetInformation().destDir.isAbsolutePath())
        return proFile->targetInformation().destDir;
    return proFile->targetInformation().buildDir / proFile->targetInformation().destDir.nativePath();
}

template <class RunConfiguration>
Utils::FilePath QtcRunConfigurationFactory<RunConfiguration>::targetInstallPath(QmakeProjectManager::QmakeProFile* proFile)
{
    Utils::FilePath targetPath = Utils::FilePath::fromString(proFile->installsList().targetPath);
    if (!targetPath.isAbsolutePath())
        targetPath = proFile->targetInformation().buildDir / proFile->installsList().targetPath;

    QString targetName = "lib" + proFile->targetInformation().target + '.' + proFile->singleVariableValue(QmakeProjectManager::Variable::ShLibExtension);
    return targetPath / targetName;
}

template <class RunConfiguration>
bool QtcRunConfigurationFactory<RunConfiguration>::isReady(ProjectExplorer::Project* project)
{
    if(project->rootProjectNode() == nullptr)
        return false;
    return project->rootProjectNode()->validParse();
}

template <class RunConfiguration>
bool QtcRunConfigurationFactory<RunConfiguration>::isUseful(ProjectExplorer::Project* project)
{
    return hasQtCreatorPlugin(project->rootProjectNode());
}

template <class RunConfiguration>
bool QtcRunConfigurationFactory<RunConfiguration>::findQtcPluginPri(ProjectExplorer::ProjectNode* node)
{
    bool ret = false;

    node->forEachFileNode([&ret] (ProjectExplorer::FileNode* subNode) {
        if (QString::compare(subNode->filePath().fileName(), Constants::QtCreatorPluginPriName, Utils::HostOsInfo::fileNameCaseSensitivity()) == 0)
            ret = true;
    });

    node->forEachFolderNode([&ret] (ProjectExplorer::FolderNode* subNode) {
        ProjectExplorer::ProjectNode* subProjectNode = subNode->asProjectNode();
        if (subProjectNode == NULL)
            return;
        if (dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(subProjectNode) != NULL)
            return;
        if (findQtcPluginPri(subProjectNode))
            ret = true;
    });

    return ret;
}

template <class RunConfiguration>
bool QtcRunConfigurationFactory<RunConfiguration>::isQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    QmakeProjectManager::QmakeProFileNode* qMakeNode = dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(node);
    if (qMakeNode == NULL)
        return false;
    if (qMakeNode->projectType() != QmakeProjectManager::ProjectType::SharedLibraryTemplate)
        return false;

    return findQtcPluginPri(node);
}

template <class RunConfiguration>
bool QtcRunConfigurationFactory<RunConfiguration>::hasQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    bool ret = false;

    if (node == nullptr)
        return false;
    if (isQtCreatorPlugin(node))
        return true;

    node->forEachFolderNode([&ret] (ProjectExplorer::FolderNode* subNode) {
        ProjectExplorer::ProjectNode* subProjectNode = subNode->asProjectNode();
        if (subProjectNode == NULL)
            return;
        if (hasQtCreatorPlugin(subProjectNode))
            ret = true;
    });

    return ret;
}

template <class RunConfiguration>
QList<ProjectExplorer::ProjectNode*> QtcRunConfigurationFactory<RunConfiguration>::qtCreatorPlugins(ProjectExplorer::ProjectNode* node)
{
    QList<ProjectExplorer::ProjectNode*> qtcPlugins;

    if (node == nullptr)
        return qtcPlugins;
    if (isQtCreatorPlugin(node))
        qtcPlugins.append(node);

    node->forEachFolderNode([&qtcPlugins] (ProjectExplorer::FolderNode* subNode) {
        ProjectExplorer::ProjectNode* subProjectNode = subNode->asProjectNode();
        if (subProjectNode != NULL)
            qtcPlugins.append(qtCreatorPlugins(subProjectNode));
    });

    return qtcPlugins;
}

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATIONFACTORY_H
