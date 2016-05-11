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

class QtcRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    inline QWidget* createConfigurationWidget(void) {return new QtcRunConfigurationWidget(this);}

    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Gui;}
    inline virtual QString executable() const {return QCoreApplication::applicationFilePath();}
    virtual QString workingDirectory(void) const;
    virtual QStringList commandLineArgumentsList(void) const;
    inline virtual QString commandLineArguments(void) const {return commandLineArgumentsList().join(QLatin1Char(' '));}

    virtual Utils::FileName installPath(void) const {return mInstallPath;}
    virtual Utils::FileName targetName(void) const {return mTargetName;}

    QVariantMap toMap(void) const;
    bool fromMap(const QVariantMap& map);
protected:
    QtcRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);
private:
    Utils::FileName mTargetName;
    Utils::FileName mDestDir;
    Utils::FileName mInstallPath;

    Utils::FileName mWorkingDirectory;
    Utils::FileName mSettingsPath;
    QString mThemeName;

    friend class QtcRunConfigurationFactory;
    friend class QtcRunConfigurationWidget;
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATION_H
