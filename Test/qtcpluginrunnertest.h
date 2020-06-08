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

#ifndef QTCPLUGINRUNNERTEST_H
#define QTCPLUGINRUNNERTEST_H

#include <QObject>

#include <coreplugin/id.h>

#include "qttestsubfunction.h"

namespace ProjectExplorer {
    class Project;
    class RunControl;
}

namespace QtcDevPlugin {
namespace Internal {
    class QtcRunConfiguration;
}

namespace Test {

class QtcPluginRunnerTest : public QObject
{
    Q_OBJECT
    HAS_SUB_TEST_FUNCTIONS
public:
    inline QtcPluginRunnerTest(QObject* parent = nullptr) :
        QObject(parent) {mProject = nullptr; mRunControl = nullptr;}
private Q_SLOTS:
    void initTestCase(void);
    inline void init(void) {mRunControl = nullptr;}
    void testRunner_data(void);
    void testRunner(void);
    void cleanup(void);
    void cleanupTestCase(void);
private:
    void qtcRunConfiguration(const Core::Id& runConfigId, QtcDevPlugin::Internal::QtcRunConfiguration** qtcRunConfig);

    ProjectExplorer::Project* mProject;
    ProjectExplorer::RunControl* mRunControl;
};

} // Test
} // QtcDevPlugin

#endif // QTCPLUGINRUNNERTEST_H
