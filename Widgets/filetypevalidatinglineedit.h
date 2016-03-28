#ifndef FILETYPEVALIDATINGLINEEDIT_H
#define FILETYPEVALIDATINGLINEEDIT_H

#include <utils/fancylineedit.h>

#include <QStringList>

namespace Utils {
    class MacroExpander;
}

namespace QtcDevPlugin {
namespace Widgets {

class FileTypeValidatingLineEdit : public Utils::FancyLineEdit
{
    Q_OBJECT
    Q_FLAGS(Accept Accepts)
public:
    enum Accept {
        AcceptsNone = 0x0,
        AcceptsFiles = 0x1,
        AcceptsDirectories = 0x2,
        RequireReadable = 0x10,
        RequireWritable = 0x20,
        RequireExecutable = 0x40,
        AcceptEmpty = 0x100,
        AcceptNew = 0x200,
    };
    Q_DECLARE_FLAGS(Accepts, Accept)

    FileTypeValidatingLineEdit(QWidget *parent = NULL);

    inline void setRequiredExtensions(const QStringList& extensions) {mRequiredExtensions = extensions;}
    inline void clearRequiredExtensions(void) {mRequiredExtensions.clear();}
    inline void addRequiredExtension(const QString& extension) {mRequiredExtensions << extension;}
    inline void addRequiredExtensions(const QString& extensions) {mRequiredExtensions.append(extensions);}
    inline QStringList requiredExtensions(void) const {return mRequiredExtensions;}

    inline void setAcceptFiles(bool accept) {manageAcceptFlags(AcceptsFiles, accept);}
    inline bool acceptFiles(void) const {return mAccepted & AcceptsFiles;}
    inline void setAcceptDirectories(bool accept) {manageAcceptFlags(AcceptsDirectories, accept);}
    inline bool acceptDirectories(void) const {return mAccepted & AcceptsDirectories;}

    inline void setRequireReadable(bool require) {manageAcceptFlags(RequireReadable, require);}
    inline void setRequireWritable(bool require) {manageAcceptFlags(RequireWritable, require);}
    inline void setRequireExecutable(bool require) {manageAcceptFlags(RequireExecutable, require);}
    inline Accepts requiredPermissions(void) const {return mAccepted & Accepts(RequireReadable | RequireWritable | RequireExecutable);}

    inline void setAcceptEmpty(bool accept) {manageAcceptFlags(AcceptEmpty, accept);}
    inline bool acceptEmpty(void) const {return mAccepted & AcceptEmpty;}
    inline void setAcceptNew(bool accept) {manageAcceptFlags(AcceptNew, accept);}
    inline bool acceptNew(void) const {return mAccepted & AcceptNew;}

    inline void setMacroExpander(Utils::MacroExpander* macroExpander) {mMacroExpander = macroExpander;}
    inline Utils::MacroExpander* macroExpander(void) const {return mMacroExpander;}
protected:
    virtual bool validate(const QString& value, QString *errorMessage) const;
private:
    bool validateName(const QString& value, QString* errorMessage) const;
    bool validateType(const QString& value, QString* errorMessage) const;
    bool validatePermissions(const QString& value, QString* errorMessage) const;

    void manageAcceptFlags(Accept flag, bool enable);

    Utils::MacroExpander* mMacroExpander;
    QStringList mRequiredExtensions;
    Accepts mAccepted;
};

} // Widgets
} // QTestLibPlugin

#endif // FILETYPEVALIDATINGLINEEDIT_H
