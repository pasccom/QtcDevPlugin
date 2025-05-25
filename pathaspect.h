/* Copyright 2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef PATH_ASPECT_H
#define PATH_ASPECT_H

#include "Widgets/filetypevalidatinglineedit.h"
#include <utils/aspects.h>
#include <utils/fileutils.h>
#include <utils/layoutbuilder.h>
#include <utils/macroexpander.h>

class QPushButton;
class QLabel;
class QCheckBox;

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief The PathAspect class provides an aspect allowing to enter a path.
 *
 * This class provides a project configuration aspect allowing to enter a path.
 * The path is validated while the user is typing.
 * The aspect can be made optional in which case a check box will appear to the
 * left of the line in the aspect configuration widget.
 * This aspect supports a macro expander which is used in the path validation.
 */
class PathAspect : public Utils::BaseAspect
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new \ref PathAspect.
     * \param container Container of the aspect.
     */
    inline PathAspect(Utils::AspectContainer* container)
        : Utils::BaseAspect(container), mCheckable(false) {}

    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \sa toMap()
     */
    void fromMap(const Utils::Store& map) override;
    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \param map The map to initialize with the contents of the instance
     * \sa fromMap()
     */
    void toMap(Utils::Store& map) const override;

    /*!
     * \brief Set the default value
     *
     * Sets the default value for this aspect and initize value if it is the default.
     * \param defaultValue The new default value for this aspect
     * \sa defaultValue(), setValue()
     */
    void setDefaultValue(const Utils::FilePath& defaultValue);
    /*!
     * \brief Get the default value
     *
     * Gets the default value of this aspect.
     * \return The default value of this aspect.
     * \sa setDefaultValue(), value()
     */
    inline Utils::FilePath defaultValue() const {return mDefaultValue;}
    /*!
     * \brief Set the value
     *
     * Sets the value for this aspect and emit changed() signal if needed.
     * \param value The new value for this aspect
     * \sa value(), setDefaultValue()
     */
    void setValue(const Utils::FilePath& value);
    /*!
     * \brief Get the value
     *
     * Gets the current value of this aspect.
     * \return The current value of this aspect.
     * \sa setValue(), defaultValue()
     */
    inline Utils::FilePath value(void) const {return mValue;}

    /*!
     * \brief Make the aspect checkable
     *
     * Make this aspect optional.
     * The aspect will show a check box, which disables the aspect when unchecked.
     * \param checkable Whether the aspect is optional.
     * \sa checkable()
     */
    inline void setCheckable(bool checkable) {mCheckable = checkable;}
    /*!
     * \brief Is the aspect checkable
     *
     * This function tells whether the aspect is optional.
     * \return Whether the aspect is optional.
     */
    inline bool checkable(void) const {return mCheckable;}

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
    inline void setAcceptFiles(bool accept) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::AcceptsFiles, accept);}
    /*!
     * \brief Does it accept files?
     *
     * This function tells whether the validator accepts file paths or not.
     * \return Whether the validator accepts file paths
     * \sa setAcceptFiles(), acceptDirectories()
     */
    inline bool acceptFiles(void) const {return mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptsFiles;}
    /*!
     * \brief Accect folders
     *
     * This function sets whether folder paths are accepted by the validator.
     * \param accept Whether folder paths are accepted.
     * \sa acceptDirectories(), setAcceptFiles()
     */
    inline void setAcceptDirectories(bool accept) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::AcceptsDirectories, accept);}
    /*!
     * \brief Does it accept folders?
     *
     * This function tells whether the validator accepts folder paths or not.
     * \return Whether the validator accepts folder paths
     * \sa setAcceptDirectories(), acceptFiles()
     */
    inline bool acceptDirectories(void) const {return mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptsDirectories;}

    /*!
     * \brief Requires paths to be readable
     *
     * This function sets the validator requirements concerning path readability.
     * \param require Whether readable paths are required by the validator.
     * \sa setRequireWritable(), setRequireExecutable(), requiredPermissions()
     */
    inline void setRequireReadable(bool require) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::RequireReadable, require);}
    /*!
     * \brief Requires paths to be writable
     *
     * This function sets the validator requirements concerning path writability.
     * \param require Whether writable paths are required by the validator.
     * \sa setRequireReadable(), setRequireExecutable(), requiredPermissions()
     */
    inline void setRequireWritable(bool require) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::RequireWritable, require);}
    /*!
     * \brief Requires paths to be executable
     *
     * This function sets the validator requirements concerning path executability.
     * \param require Whether executable paths are required by the validator.
     * \sa setRequireReadable(), setRequireWritable(), requiredPermissions()
     */
    inline void setRequireExecutable(bool require) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::RequireExecutable, require);}
    /*!
     * \brief Gives permission requirements
     *
     * This function returns the permission requirement of the validator.
     * \return The permission requirements of the validator
     * \sa setRequireReadable(), setRequireWritable(), setRequireExecutable()
     */
    inline Widgets::FileTypeValidatingLineEdit::Accepts requiredPermissions(void) const {return mAccepted & Widgets::FileTypeValidatingLineEdit::Accepts(Widgets::FileTypeValidatingLineEdit::RequireReadable | Widgets::FileTypeValidatingLineEdit::RequireWritable | Widgets::FileTypeValidatingLineEdit::RequireExecutable);}

    /*!
     * \brief Accepts empty contents
     *
     * This function set whether empty paths are accepted by the validator.
     * \param accept Whether the validator accepts empty paths
     * \sa acceptEmpty()
     */
    inline void setAcceptEmpty(bool accept) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::AcceptEmpty, accept);}
    /*!
     * \brief Does it accept empty paths?
     *
     * This function says whether empty paths are accepted by the validator.
     * \return Whether the validator accepts empty paths.
     * \sa setAcceptEmpty()
     */
    inline bool acceptEmpty(void) const {return mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptEmpty;}
    /*!
     * \brief Accepts new paths
     *
     * This function set whether non-existing paths are accepted by the validator.
     * \param accept Whether the validator accepts non-existing paths
     * \sa acceptNew()
     */
    inline void setAcceptNew(bool accept) {manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::AcceptNew, accept);}
    /*!
     * \brief Does it accept new paths?
     *
     * This function says whether non-existing paths are accepted by the validator.
     * \return Whether the validator accepts non-existing paths.
     * \sa setAcceptNew()
     */
    inline bool acceptNew(void) const {return mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptNew;}

    /*!
     * \brief Set the macro expander provider
     *
     * Sets the provider for the macro expander used to expand variables.
     *\param provider The new macro expander provider
     */
    inline void setMacroExpanderProvider(const Utils::MacroExpanderProvider& provider) {mMacroExpanderProvider = provider;}
private slots:
    /*!
     * \brief Updates the path
     *
     * This slot is called when the user finishes editing the path in \ref mEdit.
     * It saves the path, if it is valid or restores the previous one if it is invalid.
     * \sa update(bool)
     */
    void update(void);
    /*!
     * \brief Updates the validity of the path
     *
     * This slot is called when the validity of the path changes in \ref mEdit.
     * If the current path is valid, it is saved. Otherwise the slot does nothing.
     * \param valid Whether the current path is valid in \ref mEdit
     * \sa update()
     */
    void update(bool valid);
    /*!
     * \brief Updates the state of the path line.
     *
     * This slot is called when the user (un)checks \ref mCheckbox.
     * It activates or deactivates the line depending on the state of \ref mCheckbox.
     * \param checked The state of \ref mCheckbox
     */
    void updateState(bool checked);
    /*!
     * \brief Open file browser
     *
     * This slot is called when the user clicks \ref mButton,
     * to select the path by browsing folders.
     */
    void browse(void);
private:
    /*!
     * \brief Add the aspect to the layout
     *
     * Creates and adds the aspect widgets and adds them to the layout.
     * \param builder A layout builder provided by ProjectExplorer.
     */
    void addToLayoutImpl(Layouting::Layout& builder) override;
    /*!
     * \brief Manage accept flags
     *
     * Helper function for the helper flag management.
     * \param flag The accept flag to change.
     * \param enable Whether to enable or disable it.
     */
    void manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::Accept flag, bool enable);

    Widgets::FileTypeValidatingLineEdit* mEdit;             /*!< A Widgets::FileTypeValidatingLineEdit for the user to type the path */
    QPushButton* mButton;                                   /*!< The QPushButton to start browsing for the path. */
    QLabel* mLabel;                                         /*!< The QLabel associated with the aspect. */
    QCheckBox* mCheckbox;                                   /*!< A QCheckBox to activate or deactivate this aspect. */

    Utils::FilePath mValue;                                 /*!< The current value for this aspect. */
    Utils::FilePath mDefaultValue;                          /*!< The default value for this aspect. */
    bool mCheckable;                                        /*!< Whether this aspect is optional. */
    Utils::MacroExpanderProvider mMacroExpanderProvider;    /*!< The provider for the macro expander. */
    QStringList mRequiredExtensions;                        /*!< The internal list of required extensions */
    Widgets::FileTypeValidatingLineEdit::Accepts mAccepted; /*!< Accept flags */
};

} // Internal
} // QtcDevPlugin

#endif // PATHASPECT_H
