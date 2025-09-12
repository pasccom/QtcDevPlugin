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

#include "filetypevalidatinglineedit.h"

#include <utils/macroexpander.h>

#include <QtCore>

namespace QtcDevPlugin {
namespace Widgets {

FileTypeValidatingLineEdit::FileTypeValidatingLineEdit(QWidget* parent) :
    Utils::FancyLineEdit(parent), mMacroExpander(NULL), mAccepted()
{
    mAccepted |= AcceptsFiles;
    mAccepted |= RequireReadable;

    setValidationFunction([this] (const QString& value) {
        if ((mAccepted & AcceptEmpty) && value.isEmpty())
            return Utils::ResultOk;

        QString expandedValue = value;
        if (mMacroExpander != NULL)
            expandedValue = mMacroExpander->expand(value);
        qDebug() << expandedValue;

        bool exists = QFileInfo::exists(expandedValue);
        if (!(mAccepted & AcceptNew) && !exists)
            return Utils::makeResult(false, tr("File \"%1\" does not exist").arg(expandedValue));

        Utils::Result<> result = Utils::ResultOk;
        if (result)
            result = validateName(expandedValue);
        if (exists && result)
            result = validateType(expandedValue);
        if (exists && result)
            result = validatePermissions(expandedValue);
        return result;
    });
}

void FileTypeValidatingLineEdit::manageAcceptFlags(Accept flag, bool enable)
{
    if (enable)
        mAccepted |= flag;
    else
        mAccepted &= ~Accepts(flag);
}

Utils::Result<> FileTypeValidatingLineEdit::validateName(const QString& value) const
{
    if (mRequiredExtensions.isEmpty())
        return Utils::ResultOk;

    for (QString ext : mRequiredExtensions) {
        if (value.endsWith(QLatin1Char('.') + ext))
            return Utils::ResultOk;
    }

    return Utils::makeResult(false, tr("File does not have one of the required extensions."));
}

Utils::Result<> FileTypeValidatingLineEdit::validateType(const QString& value) const
{
    QFileInfo info(value);

    if (info.isFile() && !(mAccepted & AcceptsFiles))
        return Utils::makeResult(false, tr("Regular files are not accepted"));
    if (info.isDir() && !(mAccepted & AcceptsDirectories))
        return Utils::makeResult(false, tr("Directories are not accepted"));

    return Utils::ResultOk;
}

Utils::Result<> FileTypeValidatingLineEdit::validatePermissions(const QString& value) const
{
    QFileInfo info(value);

    if ((mAccepted & RequireReadable) && !info.isReadable())
        return Utils::makeResult(false, tr("File must be readable"));
    if ((mAccepted & RequireWritable) && !info.isWritable())
        return Utils::makeResult(false, tr("File must be writable"));
    if ((mAccepted & RequireExecutable) && !info.isExecutable())
        return Utils::makeResult(false, tr("File must be executable"));

    return Utils::ResultOk;
}

} // Widgets
} // QtcDevPlugin
