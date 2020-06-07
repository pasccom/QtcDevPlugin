#ifndef PATH_ASPECT_H
#define PATH_ASPECT_H

#include <projectexplorer/projectconfiguration.h>
#include <utils/fileutils.h>
#include <utils/macroexpander.h>

class QPushButton;
class QLabel;
class QCheckBox;

namespace QtcDevPlugin {
namespace Widgets {
    class FileTypeValidatingLineEdit;
}

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
class PathAspect : public ProjectExplorer::ProjectConfigurationAspect
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new \ref PathAspect.
     */
    inline PathAspect(void) : mCheckable(false) {}

    /*!
     * \brief Add the aspect to the layout
     *
     * Creates and adds the aspect widgets and adds them to the layout.
     * \param builder A layout builder provided by ProjectExplorer.
     */
    void addToLayout(ProjectExplorer::LayoutBuilder& builder) override;

    /*!
     * \brief Conversion from map
     *
     * Initializes the instance with the given QVariantMap.
     * \param map The map containing the data of the instance
     * \sa toMap()
     */
    void fromMap(const QVariantMap& map) override;
    /*!
     * \brief Conversion to map
     *
     * Converts the instance into a QVariantMap by initializing the fields of the given map.
     * \param map The map to initialize with the contents of the instance
     * \sa fromMap()
     */
    void toMap(QVariantMap& map) const override;

    /*!
     * \brief Set the default value
     *
     * Sets the default value for this aspect and initize value if it is the default.
     * \param defaultValue The new default value for this aspect
     * \sa setValue()
     */
    void setDefaultValue(const QString& defaultValue);
    /*!
     * \brief Set the value
     *
     * Sets the value for this aspect and emit changed() signal if needed.
     * \param value The new value for this aspect
     * \sa setDefaultValue(), value()
     */
    void setValue(Utils::FilePath value);
    /*!
     * \brief Get the value
     *
     * Gets the current value of this aspect.
     * \return The current value of this aspect.
     * \sa setValue()
     */
    inline Utils::FilePath value() const {return mValue;}

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
    Widgets::FileTypeValidatingLineEdit* mEdit;             /*!< A Widgets::FileTypeValidatingLineEdit for the user to type the path */
    QPushButton* mButton;                                   /*!< The QPushButton to start browsing for the path. */
    QLabel* mLabel;                                         /*!< The QLabel associated with the aspect. */
    QCheckBox* mCheckbox;                                   /*!< A QCheckBox to activate or deactivate this aspect. */

    Utils::FilePath mValue;                                 /*!< The current value for this aspect. */
    QString mDefaultValue;                                  /*!< The default value for this aspect. */
    bool mCheckable;                                        /*!< Whether this aspect is optional. */
    Utils::MacroExpanderProvider mMacroExpanderProvider;    /*!< The provider for the macro expander. */
};

} // Internal
} // QtcDevPlugin

#endif // PATHASPECT_H
