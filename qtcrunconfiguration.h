#ifndef QTCRUNCONFIGURATION_H
#define QTCRUNCONFIGURATION_H

#include <projectexplorer/localapplicationrunconfiguration.h>
#include <projectexplorer/applicationlauncher.h>

#include <coreplugin/id.h>

#include <utils/fileutils.h>

#include <QWidget>

class QLabel;
class QPushButton;
class QCheckBox;
class QComboBox;

namespace ProjectExplorer {
    class ProjectNode;
    class Project;
    class Target;
}

namespace QtcDevPlugin {
namespace Widgets {
    class FileTypeValidatingLineEdit;
}

namespace Internal {

class QtcRunConfiguration;

class QtcRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    QtcRunConfigurationWidget(QtcRunConfiguration* runConfig, QWidget* parent = NULL);
protected:
    void showEvent(QShowEvent *se);
private slots:
    void updateWorkingDirectory(bool valid);
    void updateWorkingDirectory(void);
    void browseWorkingDirectory(void);
    void updateSettingsPathState(bool checked);
    void updateSettingsPath(bool valid);
    void updateSettingsPath(void);
    void browseSettingsPath(void);
    void updateTheme(const QString& theme);
private:
    QtcRunConfiguration* mRunConfig;
    Widgets::FileTypeValidatingLineEdit* mWorkingDirectoryEdit;
    QLabel* mWorkingDirectoryLabel;
    QPushButton* mWorkingDirectoryButton;
    Widgets::FileTypeValidatingLineEdit* mSettingsPathEdit;
    QCheckBox* mSettingsPathCheck;
    QPushButton* mSettingsPathButton;
    QLabel* mThemeLabel;
    QComboBox* mThemeCombo;
};

class QtcRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    inline QWidget* createConfigurationWidget(void) {return new QtcRunConfigurationWidget(this);}

    virtual inline ProjectExplorer::ApplicationLauncher::Mode runMode(void) const {return ProjectExplorer::ApplicationLauncher::Gui;}
    inline virtual QString executable() const {return QCoreApplication::applicationFilePath();}
    virtual QString workingDirectory(void) const;
    virtual QStringList commandLineArgumentsList(void) const;
    inline virtual QString commandLineArguments(void) const {return commandLineArgumentsList().join(QLatin1Char(' '));}

    virtual Utils::FileName installPath(void) const {return mInstallPath;}
    virtual Utils::FileName targetName(void) const {return mTargetName;}

    QVariantMap toMap(void) const;
    bool fromMap(const QVariantMap& map);
protected:
    QtcRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);
private:
    Utils::FileName mTargetName;
    Utils::FileName mDestDir;
    Utils::FileName mInstallPath;

    Utils::FileName mWorkingDirectory;
    Utils::FileName mSettingsPath;
    QString mThemeName;

    friend class QtcRunConfigurationFactory;
    friend class QtcRunConfigurationWidget;
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATION_H
