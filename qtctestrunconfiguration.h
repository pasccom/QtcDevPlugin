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

#ifndef QTCTESTRUNCONFIGURATION_H
#define QTCTESTRUNCONFIGURATION_H

#include "qtcrunconfiguration.h"
#include <projectexplorer/applicationlauncher.h>

#include <QWidget>

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QtcDevPlugin {
namespace Internal {

class QtcTestRunConfiguration;

/*!
 * \brief The QtcTestRunConfigurationWidget class provides a form widget to edit a QtcTestRunConfiguration
 *
 * This class describes a form widget which allows to easily configure:
 *  \li The working directory for the test Qt Creator instance
 *  \li The path to the settings used by the test Qt Creator instance
 *  \li The theme of the test Qt Creator instance
 *
 * \note This class currently does nothing more than its parent, QtcRunConfigurationWidget,
 * it is proposed as a basis for further developments (test case selection, ...)
 *
 * \sa QtcTestRunConfiguration
 */
class QtcTestRunConfigurationWidget : public QtcRunConfigurationWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Construct a new instance of this widget to configure
     * the given QtcTestRunConfiguration
     * \param runConfig The run configuration which is configured through this form widget.
     * \param parent The parent widget
     */
    QtcTestRunConfigurationWidget(QtcTestRunConfiguration* runConfig, QWidget* parent = NULL);
private:
    QtcTestRunConfiguration* mRunConfig;    /*!< The QtcTestRunConfiguration that is configured by this form widget. */
};

/*!
 * \brief The QtcTestRunConfiguration class stores information required to start a test instance of Qt Creator
 *
 * When an instance of this class is added to a target, Qt Creator will propose to the user to
 * start Qt Creator with the plugin being developped loaded. It will then start the tests
 * associated with this plugin. It also tries to ensure that all other
 * versions of the plugin are hidden (by moving them to alternative pathes) when Qt Creator stats,
 * so that the current version is the only loaded in the current Qt Creator instance.
 * Otherwide the tests of the other instance could shadow those of the current version being tested.
 *
 * This run configuration can be easily edited using QtcTestRunConfigurationWidget, which
 * defines a suitable form wigdet to ease this process.
 *
 * \sa QtcTestRunConfigurationWidget
 */
class QtcTestRunConfiguration : public QtcRunConfiguration
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
    QtcTestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id = Core::Id(Constants::QtcTestRunConfigurationId));

    /*!
     * \brief Creates a configuration widget
     *
     * Creates an instance of the configuration widget QtcRunConfigurationWidget
     * associated to the current instance.
     * \return The newly allocated instance of the configuration widget.
     */
    inline QWidget* createConfigurationWidget(void) override {return new QtcTestRunConfigurationWidget(this);}

    /*!
     * \copydoc QtcRunConfiguration::commandLineArgumentsList()
     */
    virtual QStringList commandLineArgumentsList(void) const override;

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

    friend class QtcTestRunConfigurationWidget;
};

} // Internal
} // QtcDevPlugin

#endif // QTCTESTRUNCONFIGURATION_H
