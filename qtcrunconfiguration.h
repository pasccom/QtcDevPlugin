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

#ifndef QTCRUNCONFIGURATION_H
#define QTCRUNCONFIGURATION_H

#include "qtcdevpluginconstants.h"

#include <projectexplorer/runconfiguration.h>
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
    void showEvent(QShowEvent *se) override;
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
     * \param theme The selected theme.
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
class QtcRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Creates a new instance with parent target.
     * \param parent The parent target
     * \param id The id for the run configuration
     * \sa initialize()
     */
    QtcRunConfiguration(ProjectExplorer::Target *parent, Core::Id id = Core::Id(Constants::QtcRunConfigurationId));

    /*!
     * \brief Creates a configuration widget
     *
     * Creates an instance of the configuration widget QtcRunConfigurationWidget
     * associated to the current instance.
     * \return The newly allocated instance of the configuration widget.
     */
    virtual inline QWidget* createConfigurationWidget(void) override {return new QtcRunConfigurationWidget(this);}

    /*!
     * \brief Command-line arguments list
     *
     * Returns the list of the command-line arguments to be passed to Qt Creator executable.
     * \return The command-line arguments to be passed to the Qt Creator instance
     */
    virtual QStringList commandLineArgumentsList(void) const;

    /*!
     * \brief The runnable for this runconfiguration
     *
     * Returns the runnable for this runconfiguration, which contains
     *  - The executable
     *  - The command line arguments
     *  - The environment
     *  - The working directory
     *
     * \return The runnable for this runconfiguration
     */
    virtual ProjectExplorer::Runnable runnable(void) const override;

    /*!
     * \brief The path of the plugin when installed
     *
     * Returns the path of plugin library file when it has been installed,
     * which is the path to the installed target.
     * \return The path where of plugin library file when installed
     * \sa pluginName()
     */
    inline Utils::FileName targetFilePath(void) const {return buildTargetInfo().targetFilePath;}
    /*!
     * \brief The name of the plugin
     *
     * Returns the name of the plugin: This is the project file name without the *.pro extension.
     * \return  The name of the plugin.
     * \sa targetFilePath()
     */
    QString pluginName(void) const;

    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \return The map initialized with the contents of the instance
     * \sa fromMap()
     */
    virtual QVariantMap toMap(void) const override;
    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \return \c true when the initialization of the instance was sucessful, \c false otherwise
     * \sa toMap()
     */
    virtual bool fromMap(const QVariantMap& map) override;
private:
    Utils::FileName mWorkingDirectory;  /*!< The path to the working directory */
    Utils::FileName mSettingsPath;      /*!< The path to Qt Creator settings */
    QString mThemeName;                 /*!< Qt Creator theme to be used */

    friend class QtcRunConfigurationWidget;
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATION_H
