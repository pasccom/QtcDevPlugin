#ifndef QTCPLUGINTEST_H
#define QTCPLUGINTEST_H

#include "qtcplugintest_global.h"

#include <extensionsystem/iplugin.h>

namespace QtcPluginTest {
namespace Internal {

class QtcPluginTestPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtcPluginTest.json")

public:
    QtcPluginTestPlugin();
    ~QtcPluginTestPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void triggerAction();
};

} // namespace Internal
} // namespace QtcPluginTest

#endif // QTCPLUGINTEST_H
