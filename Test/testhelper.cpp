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

#include "testhelper.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/buildinfo.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <QSignalSpy>

#define QVERIFY(_cond_)                                                       \
    do {                                                                      \
        if (!(_cond_)) {                                                      \
            qDebug() << #_cond_ << "evaluated to false.";                     \
            return false;                                                     \
        }                                                                     \
    } while(false);                                                           \

namespace QtcDevPlugin {
namespace Test {

bool removeProjectUserFiles(const QString& projectPath)
{
    QDir projectDir(projectPath);
    QVERIFY(projectDir.exists());
    projectDir.setNameFilters(QStringList() << projectDir.dirName() + QLatin1String(".pro.user*"));
    QStringList projectUserPathes = projectDir.entryList();
    foreach (QString projectUserPath, projectUserPathes)
        QVERIFY(QFile::remove(projectDir.absoluteFilePath(projectUserPath)));
    return true;
}


bool openQMakeProject(const QString& projectFilePath, ProjectExplorer::Project** project)
{
    ProjectExplorer::Project* proj;

    // Open project
    ProjectExplorer::ProjectExplorerPlugin::OpenProjectResult result = ProjectExplorer::ProjectExplorerPlugin::openProject(projectFilePath);
    QVERIFY((bool) result);
    proj = result.project();

    // Initialize targets if required
    if (proj->activeTarget() == NULL) {
        foreach (ProjectExplorer::Kit* kit, ProjectExplorer::KitManager::kits())
            proj->addTarget(new ProjectExplorer::Target(proj, kit));
        foreach (ProjectExplorer::Target* target, proj->targets()) {
            if (target->kit() == ProjectExplorer::KitManager::defaultKit())
                ProjectExplorer::SessionManager::setActiveTarget(proj, target, ProjectExplorer::SetActive::Cascade);
        }
    }
    QVERIFY(proj->activeTarget() != NULL);

    // Initialize build confirgurations if required
    foreach (ProjectExplorer::Target* target, proj->targets()) {
        if (target->activeBuildConfiguration() == NULL) {
            ProjectExplorer::IBuildConfigurationFactory* factory = ProjectExplorer::IBuildConfigurationFactory::find(target);
            QVERIFY(factory != NULL);
            QList<ProjectExplorer::BuildInfo *> buildInfos = factory->availableBuilds(target);
            ProjectExplorer::BuildInfo* releaseBuildInfo = NULL;
            ProjectExplorer::BuildInfo* debugBuildInfo = NULL;
            foreach (ProjectExplorer::BuildInfo* bi, buildInfos) {
                if (QString::compare(bi->typeName, QLatin1String("Release"), Qt::CaseInsensitive) == 0) {
                    releaseBuildInfo = bi;
                    releaseBuildInfo->displayName = bi->typeName;
                } else if (QString::compare(bi->typeName, QLatin1String("Release"), Qt::CaseInsensitive) == 0) {
                    debugBuildInfo = bi;
                    debugBuildInfo->displayName = bi->typeName;
                }
            }
            QVERIFY((releaseBuildInfo != NULL) || (debugBuildInfo != NULL));
            if (debugBuildInfo != NULL)
                target->addBuildConfiguration(factory->create(target, debugBuildInfo));
            else
                target->addBuildConfiguration(factory->create(target, releaseBuildInfo));
        }
    }
    QVERIFY(proj->activeTarget()->activeBuildConfiguration() != NULL);

    // Wait for project evaluated
    QmakeProjectManager::QmakeProject* qMakeProject = qobject_cast<QmakeProjectManager::QmakeProject*>(proj);
    QVERIFY(qMakeProject != NULL);
    QSignalSpy evaluateSpy(qMakeProject, SIGNAL(proFilesEvaluated()));
    evaluateSpy.wait();

    // Update targets:
    foreach (ProjectExplorer::Target* target, proj->targets())
        target->updateDefaultRunConfigurations();

    if (project != NULL)
        *project = proj;
    return true;
}


} // Test
} // QtcDevPlugin
