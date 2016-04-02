#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QString>

namespace ProjectExplorer {
    class Project;
}

namespace QtcDevPlugin {
namespace Test {

bool removeProjectUserFiles(const QString& projectPath);
bool openQMakeProject(const QString& projectFilePath, ProjectExplorer::Project** project);

} // Test
} // QtcDevPlugin

#endif // TESTHELPER_H
