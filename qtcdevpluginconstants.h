#ifndef QTCDEVPLUGINCONSTANTS_H
#define QTCDEVPLUGINCONSTANTS_H

#include <QString>

namespace QtcDevPlugin {
namespace Constants {

#define QTC_RUN_CONFIGURATION_ID "QtcDevPlugin.QtcRunConfiguration"
#define QTC_TEST_RUN_CONFIGURATION_ID "QtcDevPlugin.QtcTestRunConfiguration"

const char QtcRunConfigurationId [] = QTC_RUN_CONFIGURATION_ID;
const char QtcTestRunConfigurationId [] = QTC_TEST_RUN_CONFIGURATION_ID;

const QString QtCreatorPluginPriName = QLatin1String("qtcreatorplugin.pri");

const QString WorkingDirectoryId = QLatin1String(QTC_RUN_CONFIGURATION_ID ".WorkingDirectory");
const QString SettingsPathId = QLatin1String(QTC_RUN_CONFIGURATION_ID ".SettingsPath");

} // namespace QtcDevPlugin
} // namespace Constants

#endif // QTCDEVPLUGINCONSTANTS_H
