#include "qtcdeveloperplugin.h"

#include "qtcdevpluginconstants.h"
#include "qtcrunconfigurationfactory.h"
#include "qtcrunconfiguration.h"

#ifdef BUILD_TESTS
#   include "Test/qtcrunconfigurationfactorytest.h"
#endif

#include <projectexplorer/projectexplorer.h>

#include <coreplugin/icore.h>

#include <QtCore>

using namespace QtcDevPlugin::Internal;

QtcDeveloperPlugin::QtcDeveloperPlugin()
{
    // Create your members
}

QtcDeveloperPlugin::~QtcDeveloperPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool QtcDeveloperPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Locate QML file for the current language and use it
    QString qmFile = Core::ICore::userInterfaceLanguage();
    if (qmFile.isEmpty())
        qmFile = QLatin1String("en");
    qmFile = QString(QLatin1String("qtcdevplugin_%1.qm")).arg(qmFile);

    QTranslator *translator = new QTranslator(this);
    if (translator->load(qmFile, Core::ICore::resourcePath() + QDir::separator() + QLatin1String("translations")) ||
        translator->load(qmFile, Core::ICore::userResourcePath() + QDir::separator() + QLatin1String("translations")))
        qApp->installTranslator(translator);
    else
        qWarning() << qPrintable(QString(QLatin1String("Translator file \"%1\" not found")).arg(qmFile));


    addAutoReleasedObject(new QtcRunConfigurationFactory(this));

    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlStarted(ProjectExplorer::RunControl*)),
            this, SLOT(handleRunControlStart(ProjectExplorer::RunControl*)), Qt::DirectConnection);
    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(runControlFinished(ProjectExplorer::RunControl*)),
            this, SLOT(handleRunControlEnd(ProjectExplorer::RunControl*)));

    return true;
}

void QtcDeveloperPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag QtcDeveloperPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void QtcDeveloperPlugin::handleRunControlStart(ProjectExplorer::RunControl* runControl)
{
    QTC_ASSERT(runControl != NULL, return);
    QTC_ASSERT(runControl->runConfiguration() != NULL, return);

    qDebug() << "Starting" << runControl->runConfiguration()->id();

    if (runControl->runConfiguration()->id().suffixAfter(Constants::QtcRunConfigurationId).isNull() &&
        runControl->runConfiguration()->id().suffixAfter(Constants::QtcTestRunConfigurationId).isNull())
        return;

    QtcRunConfiguration* runConfig = static_cast<QtcRunConfiguration*>(runControl->runConfiguration());
    QString targetAbsPath = runConfig->installPath().toString() + QDir::separator() + runConfig->targetName().toString();
    qDebug() << targetAbsPath << QFile(targetAbsPath).exists() << QFile(targetAbsPath + QLatin1String(".del")).exists();

    if (QFile(targetAbsPath).exists())
        QFile::rename(targetAbsPath, targetAbsPath + QLatin1String(".del"));
}

void QtcDeveloperPlugin::handleRunControlEnd(ProjectExplorer::RunControl* runControl)
{
    QTC_ASSERT(runControl != NULL, return);
    QTC_ASSERT(runControl->runConfiguration() != NULL, return);

    qDebug() << "End of" << runControl->runConfiguration()->id();

    if (runControl->runConfiguration()->id().suffixAfter(Constants::QtcRunConfigurationId).isNull() &&
        runControl->runConfiguration()->id().suffixAfter(Constants::QtcTestRunConfigurationId).isNull())
        return;

    QtcRunConfiguration* runConfig = static_cast<QtcRunConfiguration*>(runControl->runConfiguration());
    QString targetAbsPath = runConfig->installPath().toString() + QDir::separator() + runConfig->targetName().toString();
    qDebug() << targetAbsPath << QFile(targetAbsPath).exists() << QFile(targetAbsPath + QLatin1String(".del")).exists();

    if (QFile(targetAbsPath + QLatin1String(".del")).exists())
        QFile::rename(targetAbsPath + QLatin1String(".del"), targetAbsPath);
}

#ifdef BUILD_TESTS
QList<QObject *> QtcDeveloperPlugin::createTestObjects(void) const
{
    QList<QObject *> testObjects;

    testObjects << new Test::QtcRunConfigurationFactoryTest;

    return testObjects;
}
#endif
