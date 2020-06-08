#include "pathaspect.h"
#include "Widgets/filetypevalidatinglineedit.h"

#include <coreplugin/variablechooser.h>

#include <utils/qtcassert.h>

#include <QtWidgets>

namespace QtcDevPlugin {
namespace Internal {

void PathAspect::fromMap(const QVariantMap& map)
{
    QTC_ASSERT(!settingsKey().isEmpty(), return);
    mValue = Utils::FilePath::fromString(map.value(settingsKey(), mDefaultValue.toString()).toString());
    qDebug() << __func__ << settingsKey()  << "value:" << mValue;
}

void PathAspect::toMap(QVariantMap& map) const
{
    QTC_ASSERT(!settingsKey().isEmpty(), return);
    if (QString::compare(mValue.toString(), mDefaultValue.toString(), Utils::HostOsInfo::fileNameCaseSensitivity()) != 0)
        map.insert(settingsKey(), mValue.toString());
}

void PathAspect::setDefaultValue(const Utils::FilePath& defaultValue)
{
    bool isDefault = (QString::compare(mValue.toString(), mDefaultValue.toString(), Utils::HostOsInfo::fileNameCaseSensitivity()) == 0);
    mDefaultValue = defaultValue;

    if (isDefault)
        mValue = defaultValue;
}

void PathAspect::setValue(const Utils::FilePath& value)
{
    bool same = (mValue == value);
    mValue = value;
    qDebug() << __func__ << settingsKey() << "value:" << mValue;

    if (!same)
        emit changed();
}

void PathAspect::addToLayout(ProjectExplorer::LayoutBuilder &builder)
{
    mEdit = new Widgets::FileTypeValidatingLineEdit;
    mEdit->setMacroExpander(mMacroExpanderProvider());
    mEdit->setAcceptFlags(mAccepted);
    mEdit->setText(mValue.toString());

    if (mMacroExpanderProvider != nullptr) {
        Core::VariableChooser* variableChooser = new Core::VariableChooser(builder.layout()->parentWidget());
        variableChooser->addSupportedWidget(mEdit);
        variableChooser->addMacroExpanderProvider(mMacroExpanderProvider);
    }

    mButton = new QPushButton(tr("Browse..."));

    connect(mEdit, SIGNAL(validChanged(bool)), this, SLOT(update(bool)));
    connect(mEdit, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(mButton, SIGNAL(released()), this, SLOT(browse()));

    QLayout* fieldLayout = new QHBoxLayout;
    fieldLayout->addWidget(mEdit);
    fieldLayout->addWidget(mButton);

    if (mCheckable) {
        mCheckbox = new QCheckBox(displayName() + ":");
        mCheckbox->setChecked(QString::compare(mValue.toString(), mDefaultValue.toString(), Utils::HostOsInfo::fileNameCaseSensitivity()) != 0);
        mEdit->setEnabled(mCheckbox->isChecked());
        mButton->setEnabled(mCheckbox->isChecked());

        connect(mCheckbox, SIGNAL(toggled(bool)), this, SLOT(updateState(bool)));

        builder.addItems(mCheckbox, fieldLayout);
    } else {
        mLabel = new QLabel(displayName() + ":");
        mLabel->setBuddy(mEdit);

        builder.addItems(mLabel, fieldLayout);
    }
}

void PathAspect::manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::Accept flag, bool enable)
{
    if (enable)
        mAccepted |= flag;
    else
        mAccepted &= ~Widgets::FileTypeValidatingLineEdit::Accepts(flag);
}

void PathAspect::update(void)
{
    if (mEdit->isValid())
        setValue(Utils::FilePath::fromUserInput(mEdit->text()));
    mEdit->setText(mValue.toString());
}

void PathAspect::update(bool valid)
{
    if (valid)
        setValue(Utils::FilePath::fromUserInput(mEdit->text()));
}

void PathAspect::updateState(bool checked)
{
    mEdit->setEnabled(checked);
    mButton->setEnabled(checked);
    setValue(checked ? Utils::FilePath::fromUserInput(mEdit->text()) : Utils::FilePath());
}

void PathAspect::browse(void)
{
    QString path;
    if (mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptsDirectories) {
        path = QFileDialog::getExistingDirectory(nullptr, displayName(), mValue.toString());
    } else if (mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptsFiles) {
        if (mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptNew)
            path = QFileDialog::getSaveFileName(nullptr, displayName(), mValue.toString());
        else
            path = QFileDialog::getOpenFileName(nullptr, displayName(), mValue.toString());
    }

    if (!path.isNull())
        setValue(Utils::FilePath::fromString(path));
    mEdit->setText(mValue.toString());
}

} // Internal
} // QtcDevPlugin
