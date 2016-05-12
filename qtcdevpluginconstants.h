#ifndef QTCDEVPLUGINCONSTANTS_H
#define QTCDEVPLUGINCONSTANTS_H

#include <QString>

namespace QtcDevPlugin {
namespace Constants {

#define QTC_RUN_CONFIGURATION_ID "QtcDevPlugin.QtcRunConfiguration"
#define QTC_TEST_RUN_CONFIGURATION_ID "QtcDevPlugin.QtcTestRunConfiguration"

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
/*!@}*/

/*!
 * \defgroup QtcRunConfigurationSettings QtcRunConfiguration settings
 * @{
 */
const QString WorkingDirectoryKey = QLatin1String(QTC_RUN_CONFIGURATION_ID ".WorkingDirectory");    /*!< Key for working directory path Internal::QtcRunConfiguration */
const QString SettingsPathKey = QLatin1String(QTC_RUN_CONFIGURATION_ID ".SettingsPath");            /*!< Key for Qt Creator settings path Internal::QtcRunConfiguration */
/*!@}*/

/*!@}*/
} // namespace QtcDevPlugin
} // namespace Constants

#endif // QTCDEVPLUGINCONSTANTS_H
