#! /bin/bash

# Copyright 2015 Pascal COMBES <pascom@orange.fr>
#
# This file is part of QtcDevPlugin.
#
# QtcDevPlugin is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# QtcDevPlugin is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with QtcDevPlugin. If not, see <http://www.gnu.org/licenses/>

debug() {
    echo -e "\033[0;90mDEBUG:\033[0m $*"
}

error() {
    echo -e "\033[0;31mERROR:\033[0m $*"
}

# Usage
if [[ $# == 1 && $1 == "--help" ]]; then
    cat <<usage
Usage: ${0##*/} [options]

Runs Qt Creator tests (or Qt Creator) with the current plugin, ensuring that the
actual plugin version is loaded, by moving the old plugin versions out of
Qt Creator plugin path.

It uses the test configuration in ./testConfig-linux/ and the plugin in ./bin/

This is an alternative to QtcDevPlugin. It uses QtPluginExplorer to get
plugin name from plugin files.

Options:
    --help: Displays this help
    --before: Only move the old plugin versions out of Qt Creator plugin path
    --after: Only back the old plugin versions in Qt Creator plugin path
    --no-test: Run Qt Creator (and not Qt Creator tests)
usage
    exit 0
fi

# Check that QtPluginExplorer is installed:
if ! which qtpluginexplorer > /dev/null 2>&1; then
    error "Could not find \"qtpluginexplorer\" in path"
    exit 1
fi

# Get plugin name
PLUGINPATH="$(pwd)"
PLUGINNAME=${PLUGINPATH##*/}

# Get Qt Creator version
QTC_VERSION=$(qtcreator -version 2>&1 | grep -P "^Qt Creator \\d+\\.\\d+\\.\\d+" | cut -d' ' -f3)
if [[ ! $QTC_VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    error "Could not get Qt Creator version"
    exit 2
fi

# Get install dir
if [[ -n "$XDG_DATA_HOME" ]]; then
    DESTDIR="$XDG_DATA_HOME/data"
else
    DESTDIR="$HOME/.local/share/data"
fi
DESTDIR="$DESTDIR/QtProject/qtcreator/plugins/$QTC_VERSION"
#echo "Destdir: $DESTDIR"

# Removes existing plugin in install dir
if [[ $# == 0 || $1 == "--before" || $1 == "--notest" ]]; then
    pushd "$DESTDIR" > /dev/null
    mkdir -p ../.lock
    for FILE in $(ls *.so 2> /dev/null); do
        NAME=$(qtpluginexplorer "$FILE" | grep -P "^\\s{8}\"Name\":" | cut -d'"' -f4)
        if [[ "$NAME" == "$PLUGINNAME" ]]; then
            debug "Moving $FILE"
            mv "$FILE" "../.lock/$FILE"
        fi
    done
    popd > /dev/null
fi

TESTARGS=
if [[ $# == 0 || $1 != "--notest" ]]; then
    TESTARGS="-test $PLUGINNAME"
fi

# Run Qt Creator.
if [[ $# == 0 || $1 == "--notest" ]]; then
    qtcreator -theme dark -settingspath "./testConfig-linux/" -pluginpath "./bin/" $TESTARGS
fi

if [[ $# == 0 || $1 == "--after" || $1 == "--notest" ]]; then
    pushd "$DESTDIR/../.lock" > /dev/null
    for FILE in $(ls); do
        debug "Unmoving $FILE"
        mv "$FILE" "../$QTC_VERSION/$FILE"
    done
    rmdir ../.lock
    popd > /dev/null
fi
