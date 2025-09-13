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

#include "qtctestrunconfiguration.h"

#include "qtcdevpluginconstants.h"
#include "Widgets/filetypevalidatinglineedit.h"

#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <QtWidgets>
#include <QtDebug>

namespace QtcDevPlugin {
namespace Internal {

QtcTestRunConfiguration::QtcTestRunConfiguration(ProjectExplorer::BuildConfiguration* parent, Utils::Id id):
    QtcRunConfiguration(parent, id)
{
    setDefaultDisplayName(tr("Run Qt Creator tests"));
}

QStringList QtcTestRunConfiguration::commandLineArgumentsList(void) const
{
    QStringList cmdArgs =  QtcRunConfiguration::commandLineArgumentsList();

    cmdArgs << QLatin1String("-test") << pluginName() << QLatin1String("-load") << QLatin1String("all");

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs;
}

Utils::Id QtcTestRunConfiguration::id(void)
{
    return Utils::Id(Constants::QtcTestRunConfigurationId);
}

QString QtcTestRunConfiguration::displayNamePattern(void)
{
    return QCoreApplication::translate("QtcDevPlugin::Internal::QtcTestRunConfiguration", "Run Qt Creator tests for \"%1\"");
}

} // Internal
} // QtcDevPlugin

