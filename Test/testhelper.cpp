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

#include "testhelper.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/buildinfo.h>

#include <QSignalSpy>

#undef QVERIFY
#define QVERIFY(_cond_)                                                       \
    do {                                                                      \
        if (!(_cond_)) {                                                      \
            qDebug() << #_cond_ << "evaluated to false.";                     \
            return false;                                                     \
        }                                                                     \
    } while(false);                                                           \

#undef QVERIFY2
#define QVERIFY2(_cond_, _msg_)                                               \
    do {                                                                      \
        if (!(_cond_)) {                                                      \
            qDebug() << #_cond_ << "evaluated to false:" << _msg_;            \
            return false;                                                     \
        }                                                                     \
    } while(false);                                                           \

namespace QtcDevPlugin {
namespace Test {

bool removeProjectUserFiles(const Utils::FilePath& projectPath)
{
    QVERIFY(projectPath.exists());

    for (Utils::FilePath projectUserPath: projectPath.dirEntries(Utils::FileFilter(QStringList() << QLatin1String("*.pro.user*"))))
        QVERIFY(projectUserPath.removeFile());

    return true;
}


bool openQMakeProject(const Utils::FilePath& projectFilePath, ProjectExplorer::Project** project)
{
    ProjectExplorer::Project* proj;

    // Open project
    ProjectExplorer::OpenProjectResult result = ProjectExplorer::ProjectExplorerPlugin::openProject(projectFilePath);
    QVERIFY((bool) result);
    proj = result.project();

    // Initialize targets if required
    if (proj->activeTarget() == NULL) {
        proj->addTargetForKit(ProjectExplorer::KitManager::defaultKit());
        for (ProjectExplorer::Target* target: proj->targets()) {
            if (target->kit() == ProjectExplorer::KitManager::defaultKit())
                proj->setActiveTarget(target, ProjectExplorer::SetActive::Cascade);
        }
    }
    QVERIFY(proj->activeTarget() != NULL);

    // Initialize build confirgurations if required
    for (ProjectExplorer::Target* target: proj->targets()) {
        if (target->activeBuildConfiguration() == NULL) {
            ProjectExplorer::BuildConfigurationFactory* factory = ProjectExplorer::BuildConfigurationFactory::find(target);
            QVERIFY(factory != NULL);
            QList<ProjectExplorer::BuildInfo> buildInfos = factory->allAvailableBuilds(target);
            ProjectExplorer::BuildInfo releaseBuildInfo;
            ProjectExplorer::BuildInfo debugBuildInfo;
            for (ProjectExplorer::BuildInfo bi: buildInfos) {
                if (QString::compare(bi.typeName, QLatin1String("Release"), Qt::CaseInsensitive) == 0) {
                    releaseBuildInfo = bi;
                    releaseBuildInfo.displayName = bi.typeName;
                } else if (QString::compare(bi.typeName, QLatin1String("Release"), Qt::CaseInsensitive) == 0) {
                    debugBuildInfo = bi;
                    debugBuildInfo.displayName = bi.typeName;
                }
            }
            QVERIFY(!((releaseBuildInfo == ProjectExplorer::BuildInfo()) && (debugBuildInfo == ProjectExplorer::BuildInfo())));
            if (debugBuildInfo == ProjectExplorer::BuildInfo())
                target->addBuildConfiguration(factory->create(target, releaseBuildInfo));
            else
                target->addBuildConfiguration(factory->create(target, debugBuildInfo));
        }
    }
    QVERIFY(proj->activeTarget()->activeBuildConfiguration() != NULL);

    // Wait for project parsed
    QSignalSpy parsedSpy(proj->activeTarget(), SIGNAL(parsingFinished(bool)));
    QVERIFY2((parsedSpy.count() > 0) || parsedSpy.wait(), "Project parsing takes too long");

    // Update targets:
    for (ProjectExplorer::Target* target: proj->targets())
        target->updateDefaultRunConfigurations();

    if (project != NULL)
        *project = proj;
    return true;
}

bool closeProject(ProjectExplorer::Project* project)
{
    if (project == NULL)
        return false;

    qRegisterMetaType<ProjectExplorer::Project*>();
    QSignalSpy removedSpy(ProjectExplorer::ProjectManager::instance(), &ProjectExplorer::ProjectManager::projectRemoved);
    ProjectExplorer::ProjectManager::removeProject(project);
    QVERIFY2((removedSpy.count() > 0) || removedSpy.wait(), "Project removal takes too long");

    return true;
}

} // Test
} // QtcDevPlugin
