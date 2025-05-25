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
#include "qtcrunworkerfactory.h"

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
    mRunWorkerFactory = nullptr;

    // Create your members
#ifdef BUILD_TESTS
    addTest<Test::QtcRunConfigurationFactoryTest>();
    addTest<Test::QtcRunConfigurationTest>();
    addTest<Test::QtcPluginRunnerTest>();
#endif
}

QtcDeveloperPlugin::~QtcDeveloperPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    qDeleteAll(mRunConfigurationFactories);
    if (mRunWorkerFactory != nullptr)
        delete mRunWorkerFactory;
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
    if (translator->load(qmFile, Core::ICore::resourcePath("translations").toUrlishString()) ||
        translator->load(qmFile, Core::ICore::userResourcePath("translations").toUrlishString())) {
        if (!qApp->installTranslator(translator))
            qWarning() << qPrintable(QString(QLatin1String("Failed to install translator (%1)")).arg(qmFile));
    } else {
        qWarning() << qPrintable(QString(QLatin1String("Translator file \"%1\" not found")).arg(qmFile));
    }

    mRunConfigurationFactories << new QtcRunConfigurationFactory();
    mRunConfigurationFactories << new QtcTestRunConfigurationFactory();
    mRunWorkerFactory = new QtcRunWorkerFactory();

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
