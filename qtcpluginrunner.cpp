#include "qtcpluginrunner.h"

#include <qtcdevpluginconstants.h>
#include <qtcrunconfiguration.h>

#include <QtCore>

namespace QtcDevPlugin {
namespace Internal {

void QtcPluginRunner::start(void)
{
    handleRunControlStarted();

    connect(runControl(), SIGNAL(stopped()),
            this, SLOT(handleRunControlStopped()));

    ProjectExplorer::SimpleTargetRunner::start();
}

void QtcPluginRunner::handleRunControlStarted(void)
{
    qDebug() << "Starting" << runControl()->runConfiguration()->id();

    QTC_ASSERT(!runControl()->runConfiguration()->id().suffixAfter(Constants::QtcRunConfigurationId).isNull() ||
               !runControl()->runConfiguration()->id().suffixAfter(Constants::QtcTestRunConfigurationId).isNull(),
               return);

    QtcRunConfiguration* runConfig = static_cast<QtcRunConfiguration*>(runControl()->runConfiguration());
    QString targetAbsPath = runConfig->installPath().toString() + QDir::separator() + runConfig->targetName().toString();
    qDebug() << targetAbsPath << QFile(targetAbsPath).exists() << QFile(targetAbsPath + QLatin1String(".del")).exists();

    if (QFile(targetAbsPath).exists())
        QFile::rename(targetAbsPath, targetAbsPath + QLatin1String(".del"));
}

void QtcPluginRunner::handleRunControlStopped(void)
{
    qDebug() << "End of" << runControl()->runConfiguration()->id();

    QTC_ASSERT(!runControl()->runConfiguration()->id().suffixAfter(Constants::QtcRunConfigurationId).isNull() ||
               !runControl()->runConfiguration()->id().suffixAfter(Constants::QtcTestRunConfigurationId).isNull(),
               return);

    QtcRunConfiguration* runConfig = static_cast<QtcRunConfiguration*>(runControl()->runConfiguration());
    QString targetAbsPath = runConfig->installPath().toString() + QDir::separator() + runConfig->targetName().toString();
    qDebug() << targetAbsPath << QFile(targetAbsPath).exists() << QFile(targetAbsPath + QLatin1String(".del")).exists();

    if (QFile(targetAbsPath + QLatin1String(".del")).exists())
        QFile::rename(targetAbsPath + QLatin1String(".del"), targetAbsPath);
}

} // Internal
} // QtcDevPlugin
