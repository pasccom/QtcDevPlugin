REPOSITORY DESCRIPTION
----------------------

This repository contains the code for a plugin to efficiently develop
plugins for Qt Creator. The plugin defines new run configurations
specifically tailored to run or test Qt Creator with the current
version of the plugin, all older versions having been shadowed during
the run.


FEATURES
--------

Here is a list of the current features of the plugin:
- Allows to start Qt Creator with the current version of the plugin loaded
- Allows to test the current version of the plugin
- Auto-detection of plugin build output dir (`DESTDIR`) and install dir
- Tuning theme settings path and working directory of test instance
Ideas I currently have to extend the plugin are listed in the 
section PLANNED DEVELOPMENTS below.

COMPATIBILITY TABLE
-------------------

| QtcDevPlugin | Qt Creator | Linux | Windows | Mac OS |
|:-------------|:-----------|:-----:|:-------:|:------:|
| 0.0.1        | 3.6.0      |  `Y`  |   `N`   |  `?`   |
| 0.0.2        | 3.6.0      |  `Y`  |   `Y`   |  `?`   |
| 0.0.3        | 4.0.0      |  `Y`  |   `Y`   |  `?`   |
| 0.0.4        | 4.2.0      |  `Y`  |   `Y`   |  `?`   |
| 0.0.5        | 4.3.0      |  `Y`  |   `Y`   |  `?`   |
| 0.0.6        | 4.4.0      |  `Y`  |   `Y`   |  `?`   |
| 0.0.6        | 4.4.1      |  `Y`  |   `Y`   |  `?`   |
| 0.0.7        | 4.5.0      |  `Y`  |   `Y`   |  `?`   |
| 0.0.8        | 4.6.0      |  `P`  |   `P`   |  `?`   |
| 0.0.9        | 4.7.0      |  `P`  |   `P`   |  `?`   |
| 0.0.10       | 4.8.0      |  `P`  |   `P`   |  `?`   |
| 0.0.11       | 4.9.0      |  `P`  |   `P`   |  `?`   |
| 0.0.12       | 4.10.0     |  `P`  |   `P`   |  `?`   |
| 0.0.13       | 4.11.0     |  `P`  |   `P`   |  `?`   |
| 0.0.14       | 4.12.0     |  `P`  |   `P`   |  `?`   |
| 0.0.15       | 4.13.0     |  `P`  |   `P`   |  `?`   |
| 0.0.16       | 4.14.0     |  `P`  |   `P`   |  `?`   |

A `Y` means that QtcDevPlugin was tested against this Qt Creator version
under the given operating system. A `N` means that the version of the plugin
cannot work properly for the given operating system. A `P` means that
the sources should be patched (see `patches/` folder for that particular version)
for the plugin to work under the given operating system. A `?` means that I
currently do not know whether it works.

INSTALLATION
------------

To compile this file, you will need to give some information to `qMake` about
the location of Qt Creator files on your system. For this, you have two
alternatives:
- Either set the environment variables `QTC_BUILD` and `QTC_SOURCES` to the
path to Qt Creator build tree and Qt Creator source tree respectively; or
- Write a file called QtCreator.local.pri containing the following code:
```qmake
# Qt Creator source tree:
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/path/to/QtCreator/source/tree
# Qt Creator build tree:
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/path/to/QtCreator/build/tree
```
An example file (`QtCreator.local.pri.example`) is provided along with the sources.

Further build-related configuration options can be provided with
`QtcDevPlugin.local.pri`. Again an exemple file (`QtcDevPlugin.local.pri.example`)
is provided along with the sources.

PLANNED DEVELOPMENTS
--------------------

Here are some ideas I plan to implement later:
- Select test(s) to run
- Full configuration of the test Qt Creator instance
- Selection of the loaded plugin for the test Qt Creator instance

If you have any other feature you will be interested in, please let me know.
I will be pleased to develop it if I think it is a mush have.

If you want to implement extension, also tell me please. Admittedly you
can do what you desire with the code (under the constraints stated in the
section LICENSING INFORMATION below), but this will avoid double work.

LICENSING INFORMATION
---------------------

QtcDevPlugin is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

QtcDevPlugin is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with QtcDevPlugin. If not, see http://www.gnu.org/licenses/
