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

#include "qtctestrunconfiguration.h"

#include "qtcdevpluginconstants.h"
#include "Widgets/filetypevalidatinglineedit.h"

#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>

#include <qtsupport/qtsupportconstants.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <coreplugin/variablechooser.h>

#include <QtWidgets>
#include <QtDebug>

namespace QtcDevPlugin {
namespace Internal {

QtcTestRunConfiguration::QtcTestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id):
    QtcRunConfiguration(parent, id)
{
    setDefaultDisplayName(tr("Run Qt Creator tests"));
}

QVariantMap QtcTestRunConfiguration::toMap(void) const
{
    return QtcRunConfiguration::toMap();
}

bool QtcTestRunConfiguration::fromMap(const QVariantMap& map)
{
    return QtcRunConfiguration::fromMap(map);
}

QStringList QtcTestRunConfiguration::commandLineArgumentsList(void) const
{
    QStringList cmdArgs =  QtcRunConfiguration::commandLineArgumentsList();

    cmdArgs << QLatin1String("-test") << pluginName();

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs;
}

QtcTestRunConfigurationWidget::QtcTestRunConfigurationWidget(QtcTestRunConfiguration* runConfig, QWidget* parent)
    : QtcRunConfigurationWidget(runConfig, parent), mRunConfig(runConfig)
{
}

} // Internal
} // QTestLibPlugin

