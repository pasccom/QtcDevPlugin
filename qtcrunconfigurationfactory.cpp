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
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <utils/hostosinfo.h>

#include <QtDebug>

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief Project path corresponding to the given ID
 *
 * Return the path to the project file associated with
 * the given QtcRunConfiguration or QtcTestRunConfiguration ID.
 * \param id A full QtcRunConfiguration ID or a full QtcTestRunConfiguration ID
 * \param base A prefix for a QtcRunConfiguration ID or a QtcTestRunConfiguration ID
 * or an invalid ID to test for both QtcRunConfiguration and QtcTestRunConfiguration
 * \return The path to the associated project file
 */
Utils::FileName pathFromId(Core::Id id, Core::Id base = Core::Id())
{
    QString path;
    if (path.isNull() && (!base.isValid() || (base == Core::Id(Constants::QtcRunConfigurationId))))
        path = id.suffixAfter(Constants::QtcRunConfigurationId);
    if (path.isNull() && (!base.isValid() || (base == Core::Id(Constants::QtcTestRunConfigurationId))))
        path = id.suffixAfter(Constants::QtcTestRunConfigurationId);
    return Utils::FileName::fromString(path);
}

void QtcRunConfigurationFactory::updateRunConfiguration(QtcRunConfiguration* runConfig, QmakeProjectManager::QmakeProFileNode* qMakeRoot)
{
    if (qMakeRoot == NULL)
        return;

    QString name = pathFromId(runConfig->id()).fileName();
    if (name.endsWith(QLatin1String(".pro")))
        name.chop(4);
    runConfig->mPluginName = name;

    QmakeProjectManager::QmakeProFileNode* qMakeNode = qMakeRoot->findProFileFor(pathFromId(runConfig->id()));
    QTC_ASSERT(qMakeNode != NULL, return);

    if (QDir::isAbsolutePath(qMakeNode->targetInformation().destDir))
        runConfig->mDestDir = Utils::FileName::fromString(qMakeNode->targetInformation().destDir);
    else
        runConfig->mDestDir = Utils::FileName::fromString(qMakeNode->targetInformation().buildDir).appendPath(qMakeNode->targetInformation().destDir);

    runConfig->mInstallPath = Utils::FileName::fromString(qMakeNode->installsList().targetPath);
    if (!qMakeNode->variableValue(QmakeProjectManager::ShLibExtensionVar).isEmpty()) {
        QTC_ASSERT(qMakeNode->variableValue(QmakeProjectManager::ShLibExtensionVar).size() == 1,);
        if (Utils::HostOsInfo::isWindowsHost())
            runConfig->mTargetName = Utils::FileName::fromString(qMakeNode->targetInformation().target + QLatin1Char('.') + qMakeNode->variableValue(QmakeProjectManager::ShLibExtensionVar).first());
        else
            runConfig->mTargetName = Utils::FileName::fromString(QLatin1String("lib") + qMakeNode->targetInformation().target + QLatin1Char('.') + qMakeNode->variableValue(QmakeProjectManager::ShLibExtensionVar).first());
    } else if (Utils::HostOsInfo::isWindowsHost()) {
            runConfig->mTargetName = Utils::FileName::fromString(qMakeNode->targetInformation().target + QLatin1String(".dll"));
    } else {
        runConfig->mTargetName = Utils::FileName::fromString(QLatin1String("lib") + qMakeNode->targetInformation().target + QLatin1String(".so"));
    }
}

QtcRunConfigurationFactory::QtcRunConfigurationFactory(QObject *parent) :
    IRunConfigurationFactory(parent)
{
}

QList<Core::Id> QtcRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *target, CreationMode mode) const
{
    Q_UNUSED(mode);

    QList<Core::Id> ids;

    if (!canHandle(target) || !isUseful(target->project()))
        return ids;

    foreach(ProjectExplorer::ProjectNode* node, qtCreatorPlugins(target->project()->rootProjectNode())) {
        QmakeProjectManager::QmakeProFileNode* qMakeNode = static_cast<QmakeProjectManager::QmakeProFileNode*>(node);
        ids << Core::Id(Constants::QtcRunConfigurationId).withSuffix(qMakeNode->path().toString())
            << Core::Id(Constants::QtcTestRunConfigurationId).withSuffix(qMakeNode->path().toString());
    }

    qDebug() << ids;

    return ids;
}

QString QtcRunConfigurationFactory::displayNameForId(Core::Id id) const
{
    QString name = pathFromId(id).fileName();
    if (name.endsWith(QLatin1String(".pro")))
        name.chop(4);

    if (!pathFromId(id, Core::Id(Constants::QtcRunConfigurationId)).isNull())
        return QCoreApplication::translate("QtcDevPlugin::Internal::QtcRunConfiguration", "Run Qt Creator with \"%1\"").arg(name);
    if (!pathFromId(id, Core::Id(Constants::QtcTestRunConfigurationId)).isNull())
        return QCoreApplication::translate("QtcDevPlugin::Internal::QtcTestRunConfiguration", "Run Qt Creator tests for \"%1\"").arg(name);

    QTC_ASSERT(false, return QString::null);
}

bool QtcRunConfigurationFactory::canHandle(ProjectExplorer::Target* target)
{
    QTC_ASSERT((target != NULL) && (target->kit() != NULL), return false);

    if (qobject_cast<QmakeProjectManager::QmakeProject*>(target->project()) == NULL)
        return false;

    if (target->kit() == NULL)
        return false;
    if (target->kit()->hasFeatures(Core::FeatureSet(QtSupport::Constants::FEATURE_DESKTOP)))
        return true;
    return false;
}

bool QtcRunConfigurationFactory::isReady(ProjectExplorer::Project* project)
{
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(project);
    if (qMakeProject == NULL)
        return false;
    return qMakeProject->rootQmakeProjectNode()->validParse();
}

bool QtcRunConfigurationFactory::isUseful(ProjectExplorer::Project* project)
{
    QmakeProjectManager::QmakeProject* qMakeProject = static_cast<QmakeProjectManager::QmakeProject*>(project);

    QTC_ASSERT(qMakeProject != NULL, return false);
    QTC_ASSERT(qMakeProject->rootQmakeProjectNode() != NULL, return false);
    QTC_ASSERT(qMakeProject->rootQmakeProjectNode()->validParse(), return false);

    return hasQtCreatorPlugin(project->rootProjectNode());
}

bool QtcRunConfigurationFactory::findQtcPluginPri(ProjectExplorer::ProjectNode* node)
{
    foreach(ProjectExplorer::FileNode* subNode, node->fileNodes()) {
        if (QString::compare(subNode->path().fileName(), Constants::QtCreatorPluginPriName, Utils::HostOsInfo::fileNameCaseSensitivity()) == 0)
            return true;
    }

    foreach (ProjectExplorer::ProjectNode* subNode, node->subProjectNodes()) {
        if (dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(subNode) != NULL)
            continue;
        if (findQtcPluginPri(subNode))
            return true;
    }

    return false;
}


bool QtcRunConfigurationFactory::isQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    QmakeProjectManager::QmakeProFileNode* qMakeNode = dynamic_cast<QmakeProjectManager::QmakeProFileNode*>(node);
    if (qMakeNode == NULL)
        return false;
    if (qMakeNode->projectType() != QmakeProjectManager::SharedLibraryTemplate)
        return false;

    return findQtcPluginPri(node);
}

bool QtcRunConfigurationFactory::hasQtCreatorPlugin(ProjectExplorer::ProjectNode* node)
{
    if (isQtCreatorPlugin(node))
        return true;

    foreach (ProjectExplorer::ProjectNode* subNode, node->subProjectNodes()) {
        if (hasQtCreatorPlugin(subNode))
            return true;
    }

    return false;
}

QList<ProjectExplorer::ProjectNode*> QtcRunConfigurationFactory::qtCreatorPlugins(ProjectExplorer::ProjectNode* node)
{
    QList<ProjectExplorer::ProjectNode*> qtcPlugings;

    if (isQtCreatorPlugin(node))
        qtcPlugings.append(node);

    foreach (ProjectExplorer::ProjectNode* subNode, node->subProjectNodes())
        qtcPlugings.append(qtCreatorPlugins(subNode));

    return qtcPlugings;
}

bool QtcRunConfigurationFactory::canCreate(ProjectExplorer::Target *target, Core::Id id) const
{
    QTC_ASSERT(canHandle(target) && isUseful(target->project()), return false);
    if (!pathFromId(id, Core::Id(Constants::QtcRunConfigurationId)).isNull() ||
        !pathFromId(id, Core::Id(Constants::QtcTestRunConfigurationId)).isNull())
        return true;

    return false;
}

bool QtcRunConfigurationFactory::canRestore(ProjectExplorer::Target *target, const QVariantMap &map) const
{
    return canHandle(target) && (!pathFromId(ProjectExplorer::idFromMap(map), Core::Id(Constants::QtcRunConfigurationId)).isNull() ||
                                 !pathFromId(ProjectExplorer::idFromMap(map), Core::Id(Constants::QtcTestRunConfigurationId)).isNull());
}

bool QtcRunConfigurationFactory::canClone(ProjectExplorer::Target *target, ProjectExplorer::RunConfiguration *product) const
{
    return canHandle(target) && ((qobject_cast<QtcRunConfiguration*>(product) != NULL) ||
                                 (qobject_cast<QtcTestRunConfiguration*>(product) != NULL));
}

ProjectExplorer::RunConfiguration* QtcRunConfigurationFactory::clone(ProjectExplorer::Target *target, ProjectExplorer::RunConfiguration *product)
{
    QTC_ASSERT(canHandle(target), return NULL);

    QtcRunConfiguration* runConfig = NULL;

    if (qobject_cast<QtcRunConfiguration*>(product) != NULL) {
        qDebug() << "Cloning QTC run configuration for target:" << target->displayName();
        runConfig = new QtcRunConfiguration(target, Core::Id(Constants::QtcRunConfigurationId));
    }

    if (qobject_cast<QtcTestRunConfiguration*>(product) != NULL) {
        qDebug() << "Cloning QTC test run configuration for target:" << target->displayName();
        runConfig = new QtcTestRunConfiguration(target, Core::Id(Constants::QtcTestRunConfigurationId));
    }

    QTC_ASSERT(runConfig != NULL, return NULL);

    runConfig->fromMap(product->toMap());
    runConfig->setDisplayName(product->displayName());

    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(target->project());
    connect(qMakeProject, &QmakeProjectManager::QmakeProject::proFilesEvaluated,
            this, [runConfig, qMakeProject] () {
        updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());
    });
    updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());

    return runConfig;
}

ProjectExplorer::RunConfiguration* QtcRunConfigurationFactory::doCreate(ProjectExplorer::Target* target, Core::Id id)
{
    QtcRunConfiguration* runConfig = NULL;

    if (!pathFromId(id, Core::Id(Constants::QtcRunConfigurationId)).isNull()) {
        qDebug() << "Creating QTC run configuration for target:" << target->displayName();
        runConfig = new QtcRunConfiguration(target, id);
    }

    if (!pathFromId(id, Core::Id(Constants::QtcTestRunConfigurationId)).isNull()) {
        qDebug() << "Creating QTC test run configuration for target:" << target->displayName();
        runConfig = new QtcTestRunConfiguration(target, id);
    }

    QTC_ASSERT(runConfig != NULL, return NULL);

    qDebug() << displayNameForId(id);
    runConfig->setDisplayName(displayNameForId(id));

    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(target->project());
    connect(qMakeProject, &QmakeProjectManager::QmakeProject::proFilesEvaluated,
            this, [runConfig, qMakeProject] () {
        updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());
    });
    updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());

    return runConfig;
}

ProjectExplorer::RunConfiguration* QtcRunConfigurationFactory::doRestore(ProjectExplorer::Target* target, const QVariantMap& map)
{
    QtcRunConfiguration* runConfig = NULL;

    if (!pathFromId(ProjectExplorer::idFromMap(map), Core::Id(Constants::QtcRunConfigurationId)).isNull()) {
        qDebug() << "Restoring QTC run configuration for target:" << target->displayName();
        runConfig = new QtcRunConfiguration(target, ProjectExplorer::idFromMap(map));
    }

    if (!pathFromId(ProjectExplorer::idFromMap(map), Core::Id(Constants::QtcTestRunConfigurationId)).isNull()) {
        qDebug() << "Restoring QTC test run configuration for target:" << target->displayName();
        runConfig = new QtcTestRunConfiguration(target, ProjectExplorer::idFromMap(map));
    }

    QTC_ASSERT(runConfig != NULL, return NULL);

    runConfig->fromMap(map);
    qDebug() << displayNameForId(ProjectExplorer::idFromMap(map));
    runConfig->setDisplayName(displayNameForId(ProjectExplorer::idFromMap(map)));

    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(target->project());
    connect(qMakeProject, &QmakeProjectManager::QmakeProject::proFilesEvaluated,
            this, [runConfig, qMakeProject] () {
        updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());
    });
    updateRunConfiguration(runConfig, qMakeProject->rootQmakeProjectNode());

    return runConfig;
}



} // Internal
} // QTestLibPlugin

