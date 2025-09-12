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

#ifndef FILETYPEVALIDATINGLINEEDIT_H
#define FILETYPEVALIDATINGLINEEDIT_H

#include <utils/fancylineedit.h>

#include <QStringList>

namespace Utils {
    class MacroExpander;
}

namespace QtcDevPlugin {
namespace Widgets {

/*!
 * \brief The FileTypeValidatingLineEdit class defines a line edit to enter paths.
 *
 * This class describes a line edit widget which is specialized for file and folder path input.
 * It is a workaround the limited possibilities of Qt Creator FileNameValidatingLineEdit.
 * It includes some nice features such as:
 *  \li Validating the file extension.
 *  \li Validating file access rights.
 *
 * It supports Qt Creator macro expander (which allows to add variables in the given paths).
 *
 * \note When I get these features in Qt Creator FileNameValidatingLineEdit, I will sure use it.
 */
class FileTypeValidatingLineEdit : public Utils::FancyLineEdit
{
#ifndef DOXYGEN
    Q_OBJECT
    Q_FLAGS(Accept Accepts);
#endif
public:
    /*!
     * \enum Accept
     * \brief Accetable input
     *
     * Each of the values of this enum describes an acceptable input.
     * \sa Accepts
     */
    enum Accept {
        AcceptsNone = 0x0,          /*!< No accepts flags */
        AcceptsFiles = 0x1,         /*!< Accepts a file path */
        AcceptsDirectories = 0x2,   /*!< Accepts a folder path */
        RequireReadable = 0x10,     /*!< Require the path to be readable */
        RequireWritable = 0x20,     /*!< Require the path to be writable */
        RequireExecutable = 0x40,   /*!< Require the path to be executable */
        AcceptEmpty = 0x100,        /*!< Accepts an empty string */
        AcceptNew = 0x200,          /*!< Accepts a non-existing path */
    };

    /*!
     * \typedef Accepts
     * \brief Acceptable input flags
     *
     * An OR-ed combination of the flags in emum Accept.
     */
    Q_DECLARE_FLAGS(Accepts, Accept);
#ifdef DOXYGEN
    typedef QFlags<Accept> Accepts;
#endif

    /*!
     * \brief Constructor
     *
     * Constructs a new instance of this class.
     * \param parent The parent widget.
     */
    FileTypeValidatingLineEdit(QWidget *parent = NULL);

    /*!
     * \brief Set required extensions
     *
     * The selected file must have one extension in this set (if this set contains one).
     * \note Extensions must be given without any leading character
     * (for executable files on windows, give <tt>"exe"</tt>).
     * \param extensions The valid extensions for the chosen file.
     * \sa addRequiredExtension(), addRequiredExtensions(), clearRequiredExtensions(), requiredExtensions()
     */
    inline void setRequiredExtensions(const QStringList& extensions) {mRequiredExtensions = extensions;}
    /*!
     * \brief Clear required extensions
     *
     * Removes all required extensions currently set.
     * This let the user choose a file with any extension.
     * \sa setRequiredExtensions(), addRequiredExtension(), addRequiredExtensions(), requiredExtensions()
     */
    inline void clearRequiredExtensions(void) {mRequiredExtensions.clear();}
    /*!
     * \brief Add a required extension
     *
     * The given extension is added to the list of required extensions.
     * \note Extension must be given without any leading character
     * (for executable files on windows, give <tt>"exe"</tt>).
     * \param extension The extension to add to the valid extensions set.
     * \sa addRequiredExtensions(), setRequiredExtensions(), clearRequiredExtensions(), requiredExtensions()
     */
    inline void addRequiredExtension(const QString& extension) {mRequiredExtensions << extension;}
    /*!
     * \brief Add required extensionq
     *
     * The given extension list is merged with the list of required extensions.
     * \note Extensions must be given without any leading character
     * (for executable files on windows, give <tt>"exe"</tt>).
     * \param extensions The list of extensions to add to the valid extensions set.
     * \sa addRequiredExtension(), setRequiredExtensions(), clearRequiredExtensions(), requiredExtensions()
     */
    inline void addRequiredExtensions(const QString& extensions) {mRequiredExtensions.append(extensions);}
    /*!
     * \brief The list of required extensions
     *
     * Returns the list of valid extensions, this line edit will accept.
     * \return The list of valid extensions.
     * \sa setRequiredExtensions(), addRequiredExtensions(), addRequiredExtensions(), clearRequiredExtensions(), requiredExtensions()
     */
    inline QStringList requiredExtensions(void) const {return mRequiredExtensions;}

    /*!
     * \brief Accect files
     *
     * This function sets whether file paths are accepted by the validator.
     * \param accept Whether file paths are accepted.
     * \sa acceptFiles(), setAcceptDirectories()
     */
    inline void setAcceptFiles(bool accept) {manageAcceptFlags(AcceptsFiles, accept);}
    /*!
     * \brief Does it accept files?
     *
     * This function tells whether the validator accepts file paths or not.
     * \return Whether the validator accepts file paths
     * \sa setAcceptFiles(), acceptDirectories()
     */
    inline bool acceptFiles(void) const {return mAccepted & AcceptsFiles;}
    /*!
     * \brief Accect folders
     *
     * This function sets whether folder paths are accepted by the validator.
     * \param accept Whether folder paths are accepted.
     * \sa acceptDirectories(), setAcceptFiles()
     */
    inline void setAcceptDirectories(bool accept) {manageAcceptFlags(AcceptsDirectories, accept);}
    /*!
     * \brief Does it accept folders?
     *
     * This function tells whether the validator accepts folder paths or not.
     * \return Whether the validator accepts folder paths
     * \sa setAcceptDirectories(), acceptFiles()
     */
    inline bool acceptDirectories(void) const {return mAccepted & AcceptsDirectories;}

    /*!
     * \brief Requires paths to be readable
     *
     * This function sets the validator requirements concerning path readability.
     * \param require Whether readable paths are required by the validator.
     * \sa setRequireWritable(), setRequireExecutable(), requiredPermissions()
     */
    inline void setRequireReadable(bool require) {manageAcceptFlags(RequireReadable, require);}
    /*!
     * \brief Requires paths to be writable
     *
     * This function sets the validator requirements concerning path writability.
     * \param require Whether writable paths are required by the validator.
     * \sa setRequireReadable(), setRequireExecutable(), requiredPermissions()
     */
    inline void setRequireWritable(bool require) {manageAcceptFlags(RequireWritable, require);}
    /*!
     * \brief Requires paths to be executable
     *
     * This function sets the validator requirements concerning path executability.
     * \param require Whether executable paths are required by the validator.
     * \sa setRequireReadable(), setRequireWritable(), requiredPermissions()
     */
    inline void setRequireExecutable(bool require) {manageAcceptFlags(RequireExecutable, require);}
    /*!
     * \brief Gives permission requirements
     *
     * This function returns the permission requirement of the validator.
     * \return The permission requirements of the validator
     * \sa setRequireReadable(), setRequireWritable(), setRequireExecutable()
     */
    inline Accepts requiredPermissions(void) const {return mAccepted & Accepts(RequireReadable | RequireWritable | RequireExecutable);}

    /*!
     * \brief Accepts empty contents
     *
     * This function set whether empty paths are accepted by the validator.
     * \param accept Whether the validator accepts empty paths
     * \sa acceptEmpty()
     */
    inline void setAcceptEmpty(bool accept) {manageAcceptFlags(AcceptEmpty, accept);}
    /*!
     * \brief Does it accept empty paths?
     *
     * This function says whether empty paths are accepted by the validator.
     * \return Whether the validator accepts empty paths.
     * \sa setAcceptEmpty()
     */
    inline bool acceptEmpty(void) const {return mAccepted & AcceptEmpty;}
    /*!
     * \brief Accepts new paths
     *
     * This function set whether non-existing paths are accepted by the validator.
     * \param accept Whether the validator accepts non-existing paths
     * \sa acceptNew()
     */
    inline void setAcceptNew(bool accept) {manageAcceptFlags(AcceptNew, accept);}
    /*!
     * \brief Does it accept new paths?
     *
     * This function says whether non-existing paths are accepted by the validator.
     * \return Whether the validator accepts non-existing paths.
     * \sa setAcceptNew()
     */
    inline bool acceptNew(void) const {return mAccepted & AcceptNew;}

    /*!
     * \brief Set accept flags
     *
     * Note this function allows to directly set accept flags.
     * \warning End-users should use the other convenience functions.
     * \param accepted Accept flags to set
     * \sa acceptFlags()
     */
    inline void setAcceptFlags(Accepts accepted) {mAccepted = accepted;}
    /*!
     * \brief Get accept flags
     *
     * Note this function allows to directly get accept flags.
     * \warning End-users should use the other convenience functions.
     * \return The current accept flags
     * \sa setAcceptFlags()
     */
    inline Accepts acceptFlags(void) const {return mAccepted;}

    /*!
     * \brief Set the macro expander
     *
     * Sets Qt Creator macro expander associated with this line edit.
     * It is used to expand the macros variables the user may enter,
     * so that the right path is tested.
     * \param macroExpander Qt Creator macro expander.
     * \sa macroExpander()
     */
    inline void setMacroExpander(Utils::MacroExpander* macroExpander) {mMacroExpander = macroExpander;}
    /*!
     * \brief Return the macro expander
     *
     * Return the Qt Creator macro expander used for macro variable expansion.
     * \return Qt Creator macro expander or \c NULL if unset.
     * \sa setMacroExpander()
     */
    inline Utils::MacroExpander* macroExpander(void) const {return mMacroExpander;}
private:
    /*!
     * \brief Check name and extension of the given path.
     *
     * Check name and extension of the given path:
     *  \li If requiredExtensions() list is empty, the path is valid
     *  \li Otherwise, the path is valid only if it ends with a dot followed by one of the extensions.
     *
     * \param value The path to validate.
     * \return \c ResultOk if the given string is valid,
     * otherwise an error result with a descriptive error message.
     */
    Utils::Result<> validateName(const QString& value) const;
    /*!
     * \brief Check the type of the given path.
     *
     * Check that the type of the given path corresponds to an acceptable type
     * (\see acceptFiles(), acceptDirectories())
     * \param value The path to validate.
     * \return \c ResultOk if the given string is valid,
     * otherwise an error result with a descriptive error message.
     */
    Utils::Result<> validateType(const QString& value) const;
    /*!
     * \brief Check permissions of the given path.
     *
     * Check that the requirements on the permission (\see requiredPermissions())
     * of the given path are enforced.
     * \param value The path to validate.
     * \return \c ResultOk if the given string is valid,
     * otherwise an error result with a descriptive error message.
     */
    Utils::Result<> validatePermissions(const QString& value) const;

    /*!
     * \brief Manage accept flags
     *
     * Helper function for the helper flag management.
     * \param flag The accept flag to change.
     * \param enable Whether to enable or disable it.
     */
    void manageAcceptFlags(Accept flag, bool enable);

    Utils::MacroExpander* mMacroExpander;   /*!< Qt Creator macro expander (or \c NULL if none) */
    QStringList mRequiredExtensions;        /*!< The internal list of required extensions */
    Accepts mAccepted;                      /*!< Accept flags */
};

} // Widgets
} // QtcDevPlugin

#endif // FILETYPEVALIDATINGLINEEDIT_H
