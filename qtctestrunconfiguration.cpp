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

    cmdArgs << QLatin1String("-test") << QFileInfo(targetName().toString().mid(3)).baseName();

    qDebug() << "Run config command line arguments:" << cmdArgs;
    return cmdArgs;
}

QtcTestRunConfigurationWidget::QtcTestRunConfigurationWidget(QtcTestRunConfiguration* runConfig, QWidget* parent)
    : QtcRunConfigurationWidget(runConfig, parent), mRunConfig(runConfig)
{
}

} // Internal
} // QTestLibPlugin

