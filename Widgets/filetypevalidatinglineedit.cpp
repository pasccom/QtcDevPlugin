#include "filetypevalidatinglineedit.h"

#include <utils/macroexpander.h>

#include <QtCore>

namespace QtcDevPlugin {
namespace Widgets {

FileTypeValidatingLineEdit::FileTypeValidatingLineEdit(QWidget* parent) :
    Utils::FancyLineEdit(parent), mMacroExpander(NULL), mAccepted(0)
{
    mAccepted |= AcceptsFiles;
    mAccepted |= RequireReadable;

    setValidationFunction([this] (FancyLineEdit* edit, QString* errMsg) {
        return validate(edit->text(), errMsg);
    });
}

bool FileTypeValidatingLineEdit::validate(const QString& value, QString *errorMessage) const
{
    if ((mAccepted & AcceptEmpty) && value.isEmpty())
        return true;

    QString expandedValue = value;
    if (mMacroExpander != NULL)
        expandedValue = mMacroExpander->expand(value);
    qDebug() << expandedValue;

    if ((mAccepted & AcceptNew) && !QFileInfo::exists(expandedValue))
        return validateName(expandedValue, errorMessage);

    if (!QFileInfo::exists(expandedValue)) {
        if (errorMessage != NULL)
            *errorMessage = tr("File \"%1\" does not exist").arg(expandedValue);
        return false;
    }

    return (validateName(expandedValue, errorMessage)
         && validateType(expandedValue, errorMessage)
         && validatePermissions(expandedValue, errorMessage));
}

void FileTypeValidatingLineEdit::manageAcceptFlags(Accept flag, bool enable)
{
    if (enable)
        mAccepted |= flag;
    else
        mAccepted &= ~Accepts(flag);
}

bool FileTypeValidatingLineEdit::validateName(const QString& value, QString* errorMessage) const
{
    if (mRequiredExtensions.isEmpty())
        return true;

    foreach (QString ext, mRequiredExtensions) {
        if (value.endsWith(QLatin1Char('.') + ext))
            return true;
    }

    if (errorMessage != NULL)
        *errorMessage = tr("File does not have one of the required extensions.");
    return false;
}

bool FileTypeValidatingLineEdit::validateType(const QString& value, QString* errorMessage) const
{
    QFileInfo info(value);

    if (info.isFile() && !(mAccepted & AcceptsFiles)) {
        if (errorMessage != NULL)
            *errorMessage = tr("Regular files are not accepted");
        return false;
    }

    if (info.isDir() && !(mAccepted & AcceptsDirectories)) {
        if (errorMessage != NULL)
            *errorMessage = tr("Directories are not accepted");
        return false;
    }

    return true;
}

bool FileTypeValidatingLineEdit::validatePermissions(const QString& value, QString* errorMessage) const
{
    QFileInfo info(value);

    if ((mAccepted & RequireReadable) && !info.isReadable()) {
        if (errorMessage != NULL)
            *errorMessage = tr("File must be readable");
        return false;
    }
    if ((mAccepted & RequireWritable) && !info.isWritable()) {
        if (errorMessage != NULL)
            *errorMessage = tr("File must be writable");
        return false;
    }
    if ((mAccepted & RequireExecutable) && !info.isExecutable()) {
        if (errorMessage != NULL)
            *errorMessage = tr("File must be executable");
        return false;
    }

    return true;
}

} // Widgets
} // QTestLibPlugin
