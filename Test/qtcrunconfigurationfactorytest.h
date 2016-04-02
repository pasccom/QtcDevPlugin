#ifndef QTCRUNCONFIGURATIONFACTORYTEST_H
#define QTCRUNCONFIGURATIONFACTORYTEST_H

#include <QObject>

namespace ProjectExplorer {
    class Project;
}

namespace QtcDevPlugin {
namespace Test {

class QtcRunConfigurationFactoryTest : public QObject
{
    Q_OBJECT
public:
    QtcRunConfigurationFactoryTest(void);
private Q_SLOTS:
    void init(void);
    void initTestCase(void);
    inline void testOpenProjectCreate_data(void) {testOpenProject_data();}
    inline void testOpenProjectCreate(void) {testOpenProject();}
    inline void testOpenProjectRestore_data(void) {testOpenProject_data();}
    inline void testOpenProjectRestore(void) {testOpenProject();}
    void cleanup(void);
private:
    ProjectExplorer::Project* mProject;

    void testOpenProject_data(void);
    void testOpenProject(void);
};

} // Test
} // QtcDevPlugin

#endif // QTCRUNCONFIGURATIONFACTORYTEST_H
