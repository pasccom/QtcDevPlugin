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
    inline QtcRunConfigurationFactoryTest(QObject* parent = nullptr) :
        QObject(parent) {mProject = nullptr;}
private Q_SLOTS:
    inline void init(void) {mProject = nullptr;}
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
