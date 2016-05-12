#ifndef QTCRUNCONFIGURATION_H
#define QTCRUNCONFIGURATION_H

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <coreplugin/id.h>

#include <utils/fileutils.h>

#include <QWidget>

class QLabel;
class QPushButton;
class QCheckBox;
class QComboBox;

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QtcDevPlugin {
namespace Widgets {
    class FileTypeValidatingLineEdit;
}

namespace Internal {

class QtcRunConfiguration;


/*!
 * \brief The QtcRunConfigurationWidget class provides a form widget to edit a QtcRunConfiguration
 *
 * This class describes a form widget which allows to easily configure:
 *  \li The working directory for the test Qt Creator instance
 *  \li The path to the settings used by the test Qt Creator instance
 *  \li The theme of the test Qt Creator instance
 *
 * \sa QtcRunConfiguration
 */
class QtcRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Construct a new instance of this widget to configure
     * the given QtcRunConfiguration
     * \param runConfig The run configuration which is configured through this form widget.
     * \param parent The parent widget
     */
    QtcRunConfigurationWidget(QtcRunConfiguration* runConfig, QWidget* parent = NULL);
protected:
    /*!
     * \brief On show
     *
     * As a reimplementation of <tt>QWidget::showEvent()</tt>,
     * this function is called when the widget is shown.
     * It ensures all the fields are correctly initialized.
     * \param se The show event.
     */
    void showEvent(QShowEvent *se);
private slots:
    /*!
     * \brief Updates the validity of the working directory path
     *
     * This slot is called when the validity of the working directory path changes in \ref mWorkingDirectoryEdit.
     * If the current working directory path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current working directory path is valid in \ref mWorkingDirectoryEdit
     */
    void updateWorkingDirectory(bool valid);
    /*!
     * \brief Updates the working directory path
     *
     * This slot is called when the user finishes editing the working directory path in \ref mWorkingDirectoryEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateWorkingDirectory(void);
    /*!
     * \brief Open file browser for working directory path.
     *
     * This slot is called when the user clicks \ref mWorkingDirectoryButton,
     * to browse the folders for a working directory.
     */
    void browseWorkingDirectory(void);

    /*!
     * \brief Updates the state of settings path line.
     *
     * This slot is called when the user (un)checks mSettingsPathCheck.
     * It activates or deactivates the line depending on the state of mSettingsPathCheck.
     * \param checked The state of mSettingsPathCheck
     */
    void updateSettingsPathState(bool checked);
    /*!
     * \brief Updates the validity of the settings path
     *
     * This slot is called when the validity of Qt Creator settings path changes in \ref mSettingsPathEdit.
     * If the current settings path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current settings path is valid in \ref mSettingsPathEdit
     */
    void updateSettingsPath(bool valid);
    /*!
     * \brief Updates the settings path
     *
     * This slot is called when the user finishes editing Qt Creator settings path in \ref mSettingsPathEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     */
    void updateSettingsPath(void);
    /*!
     * \brief Open file browser for settings path.
     *
     * This slot is called when the user clicks \ref mSettingsPathButton,
     * to browse the folders for Qt Creator settings path.
     */
    void browseSettingsPath(void);

    /*!
     * \brief Updates the theme
     *
     * This slot is called when the user changes the theme through \ref mThemeCombo.
     * It saves the theme.
     */
    void updateTheme(const QString& theme);
private:
    QtcRunConfiguration* mRunConfig;                            /*!< The QtcRunConfiguration that is configured by this form widget. */
    Widgets::FileTypeValidatingLineEdit* mWorkingDirectoryEdit; /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to the working directory. */
    QLabel* mWorkingDirectoryLabel;                             /*!< The QLabel associated with \ref mWorkingDirectoryEdit. */
    QPushButton* mWorkingDirectoryButton;                       /*!< The QPushButton to start browsing for the working directory. */
    Widgets::FileTypeValidatingLineEdit* mSettingsPathEdit;     /*!< A Widgets::FileTypeValidatingLineEdit for the user to enter a path to Qt Creator settings path. */
    QCheckBox* mSettingsPathCheck;                              /*!< A QCheckBox to activate or deactivate Qt Creator settings path control. */
    QPushButton* mSettingsPathButton;                           /*!< The QPushButton to start browsing for Qt Creator settings path. */
    QLabel* mThemeLabel;                                        /*!< The QLabel associated with \ref mThemeCombo. */
    QComboBox* mThemeCombo;                                     /*!< A QComboBox for the user to choose the theme of Qt Creator instance. */
};

/*!
 * \brief The QtcRunConfiguration class stores information required to start an instance of Qt Creator
 *
 * When an instance of this class is added to a target, Qt Creator will propose to the user to
 * start Qt Creator with the plugin being developped loaded. It also tries to ensure that all other
 * versions of the plugin are hidden (by moving them to alternative pathes) when Qt Creator stats,
 * so that the current version is the only loaded in the current Qt Creator instance.
 *
 * This run configuration can be easily edited using QtcRunConfigurationWidget, which
 * defines a suitable form wigdet to ease this process.
 *
 * \sa QtcRunConfigurationWidget
 */
class QtcRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    /*!
     * \brief Creates a configuration widget
     *
     * Creates an instance of the configuration widget QtcRunConfigurationWidget
     * associated to the current instance.
     * \return The newly allocated instance of the configuration widget.
     */
    inline QWidget* createConfigurationWidget(void) {return new QtcRunConfigurationWidget(this);}

    /*!
     * \brief Run mode
     *
     * Return the run mode for the run configuration,
     * which is always <tt>ProjectExplorer::ApplicationLauncher::Gui</tt>.
     * \return The run mode, which is always <tt>ProjectExplorer::ApplicationLauncher::Gui</tt>
     */
    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Gui;}
    /*!
     * \brief Path to executable
     *
     * Returns the path to the executable which should be started,
     * which is the path to \c Qt Creator exexutable (the application file path).
     * \return The path to Qt Creator executable
     */
    virtual inline QString executable() const {return QCoreApplication::applicationFilePath();}
    /*!
     * \brief Path to the working directory
     *
     * Returns the path to the working directory.
     * \return The path to the working directory.
     */
    virtual QString workingDirectory(void) const;
    /*!
     * \brief Command-line arguments list
     *
     * Returns the list of the command-line arguments to be passed to Qt Creator executable.
     * \return The command-line arguments to be passed to the Qt Creator instance
     * \sa commandLineArguments()
     */
    virtual QStringList commandLineArgumentsList(void) const;
    /*!
     * \brief Command-line arguments
     *
     * Returns command-line arguments to be passed to Qt Creator executable.
     * \return The command-line arguments to be passed to the Qt Creator instance
     * \sa commandLineArgumentsList()
     */
    virtual inline QString commandLineArguments(void) const {return commandLineArgumentsList().join(QLatin1Char(' '));}

    /*!
     * \brief The path where the plugin should be installed
     *
     * Returns the pathe where plugin library file should be installed,
     * which is the path to the target.
     * \return The pathe where the plugin should be installed
     * \sa targetName()
     */
    virtual Utils::FileName installPath(void) const {return mInstallPath;}
    /*!
     * \brief The name of the target
     *
     * Returns the name of the target: The name of the library file containing the plugin.
     * \return  The name of the target.
     * \sa installPath()
     */
    virtual Utils::FileName targetName(void) const {return mTargetName;}

    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \return The map initialized with the contents of the instance
     * \sa fromMap()
     */
    QVariantMap toMap(void) const;
    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \return \c true when the initialization of the instance was sucessful, \c false otherwise
     * \sa toMap()
     */
    bool fromMap(const QVariantMap& map);
protected:
    /*!
     * \brief Constructor
     *
     * Initializes a new instance and set default values in the private fields.
     * \param parent A target
     * \param id The ID of this instance
     */
    QtcRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);
private:
    Utils::FileName mTargetName;        /*!< The name of the target, i.e. the name of the library file containing the plugin */
    Utils::FileName mDestDir;           /*!< The path where the plugin is output after build. \sa mInstallPath */
    Utils::FileName mInstallPath;       /*!< The path where the plugin is installed. \sa mDestDir */

    Utils::FileName mWorkingDirectory;  /*!< The path to the working directory */
    Utils::FileName mSettingsPath;      /*!< The path to Qt Creator settings */
    QString mThemeName;                 /*!< Qt Creator theme to be used */

    friend class QtcRunConfigurationFactory;
    friend class QtcRunConfigurationWidget;
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATION_H
