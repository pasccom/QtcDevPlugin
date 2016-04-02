#ifndef QTCDEVPLUGIN_H
#define QTCDEVPLUGIN_H

#include "qtcdevplugin_global.h"

#include <extensionsystem/iplugin.h>

namespace ProjectExplorer {
    class RunControl;
}

namespace QtcDevPlugin {
namespace Internal {

class QtcDeveloperPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtcDevPlugin.json")

public:
    QtcDeveloperPlugin();
    ~QtcDeveloperPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();
#ifdef BUILD_TESTS
    virtual QList<QObject *> createTestObjects(void) const;
#endif
private slots:
    void handleRunControlStart(ProjectExplorer::RunControl* runControl);
    void handleRunControlEnd(ProjectExplorer::RunControl* runControl);
};

} // namespace Internal
} // namespace QtcDevPlugin

#endif // QTCDEVPLUGIN_H
