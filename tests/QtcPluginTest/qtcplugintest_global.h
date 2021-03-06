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

#ifndef QTCPLUGINTEST_GLOBAL_H
#define QTCPLUGINTEST_GLOBAL_H

#include <QtGlobal>

#if defined(QTCPLUGINTEST_LIBRARY)
#  define QTCPLUGINTESTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTCPLUGINTESTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTCPLUGINTEST_GLOBAL_H
