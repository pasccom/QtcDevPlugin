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

#ifndef QTCDEVPLUGINCONSTANTS_H
#define QTCDEVPLUGINCONSTANTS_H

#include <QString>

namespace QtcDevPlugin {
namespace Constants {

#define QTC_RUN_CONFIGURATION_ID "QtcDevPlugin.QtcRunConfiguration"
#define QTC_TEST_RUN_CONFIGURATION_ID "QtcDevPlugin.QtcTestRunConfiguration"
#define QTC_WORKING_DIRECTORY_ID QTC_RUN_CONFIGURATION_ID ".WorkingDirectory"
#define QTC_SETTINGS_PATH_ID QTC_RUN_CONFIGURATION_ID ".SettingsPath"
#define QTC_THEME_ID QTC_RUN_CONFIGURATION_ID ".Theme"

/*!
 * \defgroup QtcDevPluginConstants QtcDevPlugin constants
 * @{
 */
const QString PluginName = QLatin1String("QtcDevPlugin");                                           /*!< The name of the plugin (used as root group name in the settings) */
const QString QtCreatorPluginPriName = QLatin1String("qtcreatorplugin.pri");                        /*!< The name of the project include file for Qt Creator plugins */

/*!
 * \defgroup QtcDevPluginIds QtcDevPlugin Ids
 * @{
 */
const char QtcRunConfigurationId [] = QTC_RUN_CONFIGURATION_ID;                                     /*!< Id for Internal::QtcRunConfiguration */
const char QtcTestRunConfigurationId [] = QTC_TEST_RUN_CONFIGURATION_ID;                            /*!< Id for Internal::QtcTestRunConfiguration */
const char WorkingDirectoryId [] = QTC_WORKING_DIRECTORY_ID;
const char SettingsPathId [] = QTC_SETTINGS_PATH_ID;
const char ThemeId [] = QTC_THEME_ID;
/*!@}*/

/*!
 * \defgroup QtcRunConfigurationSettings QtcRunConfiguration settings
 * @{
 */
const char WorkingDirectoryKey [] = QTC_RUN_CONFIGURATION_ID ".WorkingDirectory";                   /*!< Key for working directory path Internal::QtcRunConfiguration */
const char SettingsPathKey [] = QTC_RUN_CONFIGURATION_ID ".SettingsPath";                           /*!< Key for Qt Creator settings path Internal::QtcRunConfiguration */
const char ThemeKey [] = QTC_RUN_CONFIGURATION_ID ".Theme";                                         /*!< Key for the theme in Internal::QtcRunConfiguration */
/*!@}*/

/*!@}*/
} // namespace QtcDevPlugin
} // namespace Constants

#endif // QTCDEVPLUGINCONSTANTS_H
