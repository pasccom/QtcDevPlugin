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

/*!
 * \brief The QtcRunConfigurationFactory class creates QtcRunConfiguration and QtcTestRunConfiguration for supported \c qMake projects.
 *
 * This class is in charge of find out whether a project is supported
 * by QtcRunConfiguration and QtcTestRunConfiguration.
 * For this is uses the result of the parsing of the project file
 * and also the features of the kit of the active target.
 *
 * It handles only desktop targets and is usefull only for \c qMake projects
 * including the project include for Qt Crator plugins. When this file is included
 * by multiple subprojects multiple run configurations can be created (one for each
 * sub project including Qt Creator plugin.
 *
 * \sa QtcRunConfiguration, QtcTestRunConfiguration
 */
class QtcRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * The constructor currently does nothing.
     * \param parent The parent object.
     */
    QtcRunConfigurationFactory(QObject *parent = NULL);
    /*!
     * \brief The run configuration IDs this factory can create
     *
     * This function returns a list of the the IDs this factory can create.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target The target of the future run configuration.
     * \param mode The creation mode.
     * \return A list of the run configuration IDs this factory can create.
     * \sa canCreate()
     */
    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *target, CreationMode mode = UserCreate) const;
    /*!
     * \brief The display name corresponding to the given ID
     *
     * Expects the given ID to be the QtcRunConfiguration ID or a QtcTestRunConfiguration ID
     * and returns the corresponding display name.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param id A run configuration ID (expects a QtcRunConfiguration ID or a QtcTestRunConfiguration ID)
     * \return The corresponding display name
     */
    QString displayNameForId(Core::Id id) const;

    /*!
     * \brief Whether the factory handles the given target
     *
     * Returns \c true if the factory handles the given target,
     * i.e. if the associated project is a \c qMake project and
     * if the associated kit is a desktop kit.
     * \param target A target
     * \return \c true if the factory can handle the target, \c false otherwise.
     * \sa isUseful(), canCreate(), canRestore(), canClone()
     */
    static bool canHandle(ProjectExplorer::Target* target);
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
     * \brief Whether a run configuration can be created
     *
     * Returns \c true when a run configuration with the given ID can be created.
     * This implementation supports QtcRunConfiguration and QtcTestRunConfiguration,
     * the target must be handled,
     * and a QtcRunConfiguration or a QtcTestRunConfiguration must be useful for the associated project.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target, which will be the parent of the run configuration
     * \param id The ID of the desired run configuration
     * \return \c true if the factory can create a run configuration.
     * \sa canHandle(), isUseful(), canRestore(), canClone()
     */
    bool canCreate(ProjectExplorer::Target* parent, Core::Id id) const;
    /*!
     * \brief Whether a run configuration can be restored
     *
     * Returns \c true when a run configuration can be restored from the given data.
     * This implementation supports only QtcRunConfiguration and QtcTestRunConfiguration,
     * the target must be handled,
     * and a QtcRunConfiguration or a QtcTestRunConfiguration must be useful for the associated project.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target, which will be the parent of the run configuration
     * \param map The run configuration data
     * \return \c true if the factory can restore a run configuration from the data.
     * \sa canHandle(), isUseful(), canCreate(), canClone()
     */
    bool canRestore(ProjectExplorer::Target* parent, const QVariantMap &map) const;
    /*!
     * \brief Whether a run configuration can be cloned
     *
     * Returns \c true when the given run configuration can be cloned.
     * This implementation supports only QtcRunConfiguration and QtcTestRunConfiguration,
     * the target must be handled,
     * and a QtcRunConfiguration or a QtcTestRunConfiguration must be useful for the associated project.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target, which will be the parent of the run configuration
     * \param product A run configuration
     * \return \c true if the factory can create a run configuration.
     * \sa canHandle(), isUseful(), canCreate(), canRestore(), clone()
     */
    bool canClone(ProjectExplorer::Target* parent, ProjectExplorer::RunConfiguration *product) const;

    /*!
     * \brief Clones a run configuration
     *
     * This method can be used after checking canClone() returns \c true.
     * It clones the run configuration on the new target by copying its internal data.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param parent A target (must be target the factory canHandle())
     * \param product A run configuration (must be a QtcRunConfiguration or a QtcTestRunConfiguration)
     * \return A clone of the given run configuration associated with the given target,
     * or \c NULL when unsupported.
     * \sa canClone()
     */
    ProjectExplorer::RunConfiguration* clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product);
private:
    /*!
     * \brief Really creates a run configuration.
     *
     * This method is called automatically by ProjectExplorer
     * to create an new run configuration with the given ID.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target A target (must be target the factory canHandle())
     * \param id An ID (must be a QtcRunConfiguration ID or a QtcTestRunConfiguration ID)
     * \return A new run configuration associated with the given target,
     * or \c NULL when unsupported.
     * \sa canCreate()
     */
    ProjectExplorer::RunConfiguration* doCreate(ProjectExplorer::Target* target, Core::Id id);
    /*!
     * \brief Really restores a run configuration.
     *
     * This method is called automatically by ProjectExplorer
     * to restore a run configuration from the given data.
     *
     * \note A more extensive documentation may be available in Qt Creator Developper documentation
     *
     * \param target A target (must be target the factory canHandle())
     * \param map Data to restore the run configuration
     * \return A new run configuration initialized with the given data,
     * or \c NULL when unsupported.
     * \sa canRestore()
     */
    ProjectExplorer::RunConfiguration* doRestore(ProjectExplorer::Target* target, const QVariantMap& map);

    /*!
     * \brief Updates a run configuration
     *
     * This function is used to update the given QtcRunConfiguration or QtcTestRunConfiguration.
     * It stores inside the run configuration the name of the target and the pathes
     * to build output and installation directories.
     * \param runConfig A QtcRunConfiguration or a QtcTestRunConfiguration to be updated
     * \param qMakeRoot The root project file node of the associated \c qMake project
     */
    static void updateRunConfiguration(QtcRunConfiguration* runConfig, QmakeProjectManager::QmakeProFileNode* qMakeRoot);

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
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATIONFACTORY_H
