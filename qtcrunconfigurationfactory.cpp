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

#include "qtcrunconfigurationfactory.h"

#include "qtcrunconfiguration.h"
#include "qtctestrunconfiguration.h"
#include "qtcdevpluginconstants.h"

#include <projectexplorer/target.h>

#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakeprojectmanagerconstants.h>

#include <QtDebug>

namespace QtcDevPlugin {
namespace Internal {

QtcRunConfigurationFactory::QtcRunConfigurationFactory(QObject *parent) :
    IRunConfigurationFactory(parent)
{
    addSupportedProjectType(QmakeProjectManager::Constants::QMAKEPROJECT_ID);
    setSupportedTargetDeviceTypes({ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE});

    setDisplayNamePattern(QCoreApplication::translate("QtcDevPlugin::Internal::QtcRunConfiguration", "Run Qt Creator with \"%1\""));
    registerRunConfiguration<QtcRunConfiguration>(Core::Id(Constants::QtcRunConfigurationId));
}

QtcTestRunConfigurationFactory::QtcTestRunConfigurationFactory(QObject *parent) :
    QtcRunConfigurationFactory(parent)
{
    setDisplayNamePattern(QCoreApplication::translate("QtcDevPlugin::Internal::QtcTestRunConfiguration", "Run Qt Creator tests for \"%1\""));
    registerRunConfiguration<QtcTestRunConfiguration>(Core::Id(Constants::QtcTestRunConfigurationId));
}

QList<ProjectExplorer::BuildTargetInfo> QtcRunConfigurationFactory::availableBuildTargets(ProjectExplorer::Target *target, CreationMode mode) const
{
    Q_UNUSED(mode);

    QList<ProjectExplorer::BuildTargetInfo> buildTargets;
    ProjectExplorer::BuildTargetInfoList appBuildTargets = target->applicationTargets();

    if (!canHandle(target) || !isReady(target->project()) || !isUseful(target->project()))
        return buildTargets;

    foreach (ProjectExplorer::ProjectNode* node, qtCreatorPlugins(target->project()->rootProjectNode())) {
        QmakeProjectManager::QmakeProFileNode* qMakeNode = static_cast<QmakeProjectManager::QmakeProFileNode*>(node);

        ProjectExplorer::BuildTargetInfo buildTarget;
        buildTarget.projectFilePath = qMakeNode->filePath();

        QString name = buildTarget.projectFilePath.fileName();
        if (name.endsWith(QLatin1String(".pro")))
                name.chop(4);
        buildTarget.targetName = name;

        buildTargets << buildTarget;
        if (!appBuildTargets.list.contains(buildTarget))
            appBuildTargets.list.append(buildTarget);
    }

    target->setApplicationTargets(appBuildTargets);
    return buildTargets;
}

bool QtcRunConfigurationFactory::isReady(ProjectExplorer::Project* project)
{
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);
    if (qMakeProject == NULL)
        return false;
    return qMakeProject->rootProFile()->validParse();
}

bool QtcRunConfigurationFactory::isUseful(ProjectExplorer::Project* project)
{
    QmakeProjectManager::QmakeProject* qMakeProject = static_cast<QmakeProjectManager::QmakeProject*>(project);

    QTC_ASSERT(qMakeProject != NULL, return false);
    QTC_ASSERT(qMakeProject->rootProFile() != NULL, return false);
    QTC_ASSERT(qMakeProject->rootProFile()->validParse(), return false);

    return hasQtCreatorPlugin(project->rootProjectNode());
}

bool QtcRunConfigurationFactory::findQtcPluginPri(ProjectExplorer::ProjectNode* node)
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

bool QtcRunConfigurationFactory::isQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    QmakeProjectManager::QmakeProFileNode* qMakeNode = dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(node);
    if (qMakeNode == NULL)
        return false;
    if (qMakeNode->projectType() != QmakeProjectManager::ProjectType::SharedLibraryTemplate)
        return false;

    return findQtcPluginPri(node);
}

bool QtcRunConfigurationFactory::hasQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
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

QList<ProjectExplorer::ProjectNode*> QtcRunConfigurationFactory::qtCreatorPlugins(ProjectExplorer::ProjectNode* node)
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

