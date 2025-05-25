#ifndef QTCRUNWORKERFACTORY_H
#define QTCRUNWORKERFACTORY_H

#include <projectexplorer/runcontrol.h>

namespace QtcDevPlugin {
namespace Internal {

/*!
 * \brief The QtcRunWorkerFactory class creates QtcRunWorker for run configurations
 * associated with Qt Creator plugins.
 *
 * This class support normal and debug run modes on desktop devices.
 *
 * \sa QtcRunConfiguration, QtcTestRunConfiguration, QtcRunWorker
 */
class QtcRunWorkerFactory : public ProjectExplorer::RunWorkerFactory
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an new run worker factory instance for run configurations
     * associated with Qt Creator plugins.
     */
    QtcRunWorkerFactory();
};

} // Internal
} // QtcDevPlugin

#endif // QTCRUNWORKERFACTORY_H
