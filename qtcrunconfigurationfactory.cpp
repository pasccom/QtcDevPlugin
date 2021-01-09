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

#include "qtcrunconfigurationfactory.h"

#include "qtcrunconfiguration.h"
#include "qtctestrunconfiguration.h"
#include "qtcdevpluginconstants.h"

#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/kitinformation.h>

#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakeprojectmanagerconstants.h>

#include <QtDebug>

namespace QtcDevPlugin {
namespace Internal {

BaseQtcRunConfigurationFactory::BaseQtcRunConfigurationFactory(void) :
    ProjectExplorer::RunConfigurationFactory()
{
    addSupportedProjectType(QmakeProjectManager::Constants::QMAKEPROJECT_ID);
    addSupportedTargetDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
}

QtcRunConfigurationFactory::QtcRunConfigurationFactory(void) :
    BaseQtcRunConfigurationFactory()
{
    setDisplayNamePattern(QCoreApplication::translate("QtcDevPlugin::Internal::QtcRunConfiguration", "Run Qt Creator with \"%1\""));
    registerRunConfiguration<QtcRunConfiguration>(Utils::Id(Constants::QtcRunConfigurationId));
}

QtcTestRunConfigurationFactory::QtcTestRunConfigurationFactory(void) :
    BaseQtcRunConfigurationFactory()
{
    setDisplayNamePattern(QCoreApplication::translate("QtcDevPlugin::Internal::QtcTestRunConfiguration", "Run Qt Creator tests for \"%1\""));
    registerRunConfiguration<QtcTestRunConfiguration>(Utils::Id(Constants::QtcTestRunConfigurationId));
}

QList<ProjectExplorer::RunConfigurationCreationInfo> BaseQtcRunConfigurationFactory::availableCreators(ProjectExplorer::Target *target) const
{
    QList<ProjectExplorer::RunConfigurationCreationInfo> creators;

    if (!isReady(target->project()) || !isUseful(target->project()))
        return creators;

    QList<ProjectExplorer::BuildTargetInfo> buildInfos = target->applicationTargets();
    qDebug() << "BuildTargetInfoList size:" << buildInfos.size();
    foreach (ProjectExplorer::ProjectNode *node, qtCreatorPlugins(target->project()->rootProjectNode())) {
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
        creator.id = runConfigurationBaseId();
        creator.buildKey = info.buildKey;
        creator.displayName = mDisplayNamePattern.arg(info.displayName);

        creators << creator;
    }
    if (target->buildSystem() != nullptr)
        target->buildSystem()->setApplicationTargets(buildInfos);

    return creators;
}

Utils::FilePath BaseQtcRunConfigurationFactory::targetBuildPath(QmakeProjectManager::QmakeProFile* proFile)
{
    QDir targetPath(proFile->targetInformation().destDir.toString());
    if (!targetPath.isAbsolute())
        targetPath.setPath(proFile->targetInformation().buildDir.pathAppended(targetPath.path()).toString());
    return  Utils::FilePath::fromString(targetPath.absolutePath());
}

Utils::FilePath BaseQtcRunConfigurationFactory::targetInstallPath(QmakeProjectManager::QmakeProFile* proFile)
{
    QDir targetPath(proFile->installsList().targetPath);
    if (!targetPath.isAbsolute())
        targetPath.setPath(proFile->targetInformation().buildDir.pathAppended(targetPath.path()).toString());

    QString targetName = "lib" + proFile->targetInformation().target + '.' + proFile->singleVariableValue(QmakeProjectManager::Variable::ShLibExtension);
    return Utils::FilePath::fromString(targetPath.absoluteFilePath(targetName));
}

bool BaseQtcRunConfigurationFactory::isReady(ProjectExplorer::Project* project)
{
    if(project->rootProjectNode() == nullptr)
        return false;
    return project->rootProjectNode()->validParse();
}

bool BaseQtcRunConfigurationFactory::isUseful(ProjectExplorer::Project* project)
{
    return hasQtCreatorPlugin(project->rootProjectNode());
}

bool BaseQtcRunConfigurationFactory::findQtcPluginPri(ProjectExplorer::ProjectNode* node)
{
    foreach(ProjectExplorer::FileNode* subNode, node->fileNodes()) {
        if (QString::compare(subNode->filePath().fileName(), Constants::QtCreatorPluginPriName, Utils::HostOsInfo::fileNameCaseSensitivity()) == 0)
            return true;
    }

    foreach (ProjectExplorer::FolderNode* subNode, node->folderNodes()) {
        ProjectExplorer::ProjectNode* subProjectNode = subNode->asProjectNode();
        if (subProjectNode != NULL) {
            if (dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(subProjectNode) != NULL)
                continue;
            if (findQtcPluginPri(subProjectNode))
                return true;
        }
    }

    return false;
}

bool BaseQtcRunConfigurationFactory::isQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    QmakeProjectManager::QmakeProFileNode* qMakeNode = dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(node);
    if (qMakeNode == NULL)
        return false;
    if (qMakeNode->projectType() != QmakeProjectManager::ProjectType::SharedLibraryTemplate)
        return false;

    return findQtcPluginPri(node);
}

bool BaseQtcRunConfigurationFactory::hasQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    if (isQtCreatorPlugin(node))
        return true;

    foreach (ProjectExplorer::FolderNode* subNode, node->folderNodes()) {
        ProjectExplorer::ProjectNode* subProjectNode = subNode->asProjectNode();
        if (subProjectNode != NULL)
            if (hasQtCreatorPlugin(subProjectNode))
                return true;
    }

    return false;
}

QList<ProjectExplorer::ProjectNode*> BaseQtcRunConfigurationFactory::qtCreatorPlugins(ProjectExplorer::ProjectNode* node)
{
    QList<ProjectExplorer::ProjectNode*> qtcPlugins;

    if (isQtCreatorPlugin(node))
        qtcPlugins.append(node);

    foreach (ProjectExplorer::FolderNode* subNode, node->folderNodes()) {
        ProjectExplorer::ProjectNode* subProjectNode = subNode->asProjectNode();
        if (subProjectNode != NULL)
            qtcPlugins.append(qtCreatorPlugins(subProjectNode));
    }

    return qtcPlugins;
}

} // Internal
} // QTestLibPlugin

