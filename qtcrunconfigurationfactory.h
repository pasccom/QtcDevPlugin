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
}

namespace QtcDevPlugin {
namespace Internal {

class QtcRunConfiguration;

class QtcRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    QtcRunConfigurationFactory(QObject *parent = NULL);
    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *target, CreationMode mode = UserCreate) const;
    QString displayNameForId(Core::Id id) const;

    static bool canHandle(ProjectExplorer::Target* target);
    static bool isReady(ProjectExplorer::Project* project);
    static bool isUseful(ProjectExplorer::Project* project);

    bool canCreate(ProjectExplorer::Target* parent, Core::Id id) const;
    bool canRestore(ProjectExplorer::Target* parent, const QVariantMap &map) const;
    bool canClone(ProjectExplorer::Target* parent, ProjectExplorer::RunConfiguration *product) const;
    ProjectExplorer::RunConfiguration* clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product);
private:
    ProjectExplorer::RunConfiguration* doCreate(ProjectExplorer::Target* target, Core::Id id);
    ProjectExplorer::RunConfiguration* doRestore(ProjectExplorer::Target* target, const QVariantMap& map);

    static void updateRunConfiguration(QtcRunConfiguration* runConfig, QmakeProjectManager::QmakeProFileNode* qMakeRoot);

    static bool findQtcPluginPri(ProjectExplorer::ProjectNode* node);
    static bool isQtCreatorPlugin(ProjectExplorer::ProjectNode* node);
    static bool hasQtCreatorPlugin(ProjectExplorer::ProjectNode* node);
    static QList<ProjectExplorer::ProjectNode*> qtCreatorPlugins(ProjectExplorer::ProjectNode* node);
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATIONFACTORY_H
