#ifndef QTCTESTRUNCONFIGURATION_H
#define QTCTESTRUNCONFIGURATION_H

#include "qtcrunconfiguration.h"
#include <projectexplorer/applicationlauncher.h>

#include <QWidget>

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QtcDevPlugin {
namespace Internal {

class QtcTestRunConfiguration;

class QtcTestRunConfigurationWidget : public QtcRunConfigurationWidget
{
    Q_OBJECT
public:
    QtcTestRunConfigurationWidget(QtcTestRunConfiguration* runConfig, QWidget* parent = NULL);
private:
    QtcTestRunConfiguration* mRunConfig;
};

class QtcTestRunConfiguration : public QtcRunConfiguration
{
    Q_OBJECT
public:
    inline QWidget* createConfigurationWidget(void) {return new QtcTestRunConfigurationWidget(this);}

    virtual QStringList commandLineArgumentsList(void) const;

    QVariantMap toMap(void) const;
    bool fromMap(const QVariantMap& map);
protected:
    QtcTestRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    friend class QtcRunConfigurationFactory;
    friend class QtcTestRunConfigurationWidget;
};

} // Internal
} // QtcDevPlugin

#endif // QTCTESTRUNCONFIGURATION_H
