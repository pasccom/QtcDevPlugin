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

#include "qtcdeveloperplugin.h"

#include "qtcdevpluginconstants.h"
#include "qtcrunconfigurationfactory.h"
#include "qtcrunconfiguration.h"

#ifdef BUILD_TESTS
#   include "Test/qtcrunconfigurationfactorytest.h"
#   include "Test/qtcrunconfigurationtest.h"
#   include "Test/qtcpluginrunnertest.h"
#endif

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/runcontrol.h>

#include <coreplugin/icore.h>

#include <extensionsystem/pluginmanager.h>

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
    qDeleteAll(mRunConfigurationFactories);
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

    // Locate QM file for the current language and use it
    QString qmFile = Core::ICore::userInterfaceLanguage();

    if (qmFile.isEmpty())
        qmFile = QLatin1String("en");
    else
        qmFile = qmFile.left(2);
    qmFile = QString(QLatin1String("qtcdevplugin_%1.qm")).arg(qmFile);

    QTranslator *translator = new QTranslator(this);
    if (translator->load(qmFile, Core::ICore::resourcePath() + QDir::separator() + QLatin1String("translations")) ||
        translator->load(qmFile, Core::ICore::userResourcePath() + QDir::separator() + QLatin1String("translations"))) {
        if (!qApp->installTranslator(translator))
            qWarning() << qPrintable(QString(QLatin1String("Failed to install translator (%1)")).arg(qmFile));
    } else {
        qWarning() << qPrintable(QString(QLatin1String("Translator file \"%1\" not found")).arg(qmFile));
    }

    mRunConfigurationFactories << new QtcRunConfigurationFactory();
    mRunConfigurationFactories << new QtcTestRunConfigurationFactory();

    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(aboutToExecuteRunControl(ProjectExplorer::RunControl*, Core::Id)),
            this, SLOT(handleRunControlStarted(ProjectExplorer::RunControl*)), Qt::DirectConnection);

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

void QtcDeveloperPlugin::handleRunControlStarted(ProjectExplorer::RunControl* runControl)
{
    qDebug() << "Starting" << runControl->runConfiguration()->id();

    if ((runControl->runConfiguration()->id() != Core::Id(Constants::QtcRunConfigurationId)) &&
        (runControl->runConfiguration()->id() != Core::Id(Constants::QtcTestRunConfigurationId)))
        return;

    connect(runControl, SIGNAL(stopped()), this, SLOT(handleRunControlStopped()));

    Utils::FilePath targetFilePath(static_cast<QtcRunConfiguration*>(runControl->runConfiguration())->targetFilePath());

    movePluginFile(targetFilePath, QString(), QLatin1String(".del"));
    foreach (Utils::FilePath pluginFilePath, pluginPaths(targetFilePath.fileName()))
        movePluginFile(pluginFilePath, QString(), QLatin1String(".del"));
}

void QtcDeveloperPlugin::handleRunControlStopped()
{
    ProjectExplorer::RunControl* runControl = qobject_cast<ProjectExplorer::RunControl*>(sender());
    QTC_ASSERT(runControl != nullptr, return);

    qDebug() << "End of" << runControl->runConfiguration()->id();

    if ((runControl->runConfiguration()->id() != Core::Id(Constants::QtcRunConfigurationId)) &&
        (runControl->runConfiguration()->id() != Core::Id(Constants::QtcTestRunConfigurationId)))
        return;

    Utils::FilePath targetFilePath(static_cast<QtcRunConfiguration*>(runControl->runConfiguration())->targetFilePath());

    movePluginFile(targetFilePath, QLatin1String(".del"), QString());
    foreach (Utils::FilePath pluginFilePath, pluginPaths(targetFilePath.fileName()))
        movePluginFile(pluginFilePath, QLatin1String(".del"), QString());
}

QLinkedList<Utils::FilePath> QtcDeveloperPlugin::pluginPaths(const QString& fileName)
{
    QLinkedList<Utils::FilePath> ans;

    foreach (QString pluginPath, ExtensionSystem::PluginManager::pluginPaths())
        ans << Utils::FilePath::fromString(pluginPath).pathAppended(fileName);

    return ans;
}

void QtcDeveloperPlugin::movePluginFile(const Utils::FilePath& targetPath, const QString& oldSuffix, const QString& newSuffix)
{
    Utils::FilePath oldTargetPath = Utils::FilePath(targetPath).stringAppended(oldSuffix);
    Utils::FilePath newTargetPath = Utils::FilePath(targetPath).stringAppended(newSuffix);

    qDebug() << oldTargetPath << oldTargetPath.toFileInfo().exists() << newTargetPath << newTargetPath.toFileInfo().exists();

    if (oldTargetPath.toFileInfo().exists()) {
        QTC_CHECK(QFile::rename(oldTargetPath.toString(), newTargetPath.toString()));
    }
    QTC_CHECK(!oldTargetPath.toFileInfo().exists());
}

#ifdef BUILD_TESTS
QVector<QObject *> QtcDeveloperPlugin::createTestObjects(void) const
{
    QVector<QObject *> testObjects;

    testObjects << new Test::QtcRunConfigurationFactoryTest;
    testObjects << new Test::QtcRunConfigurationTest;
    testObjects << new Test::QtcPluginRunnerTest;

    return testObjects;
}
#endif
