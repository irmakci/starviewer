#!/bin/bash

# Call ecm configuration script to make sure ECMCMAKEDIR is set
. "$SCRIPTS_ROOT/config/ecm.sh"

SOURCE_DIR="$SOURCE_DIR_BASE/threadweaver-5.3.0"

if [ $BUILD_TYPE = debug ]
then
    CMAKE_BUILD_TYPE=Debug
    BUILD_DIR="$SOURCE_DIR-build-debug"
fi
if [ $BUILD_TYPE = release ]
then
    CMAKE_BUILD_TYPE=RelWithDebInfo
    BUILD_DIR="$SOURCE_DIR-build-release"
fi


################ Nothing should need to be changed below this line ################

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DCMAKE_PREFIX_PATH:PATH=$QTDIR \
               -DBUILD_TESTING:BOOL=FALSE \
               -DECM_DIR:PATH=$ECMCMAKEDIR"

if [[ $(uname) == 'Linux' ]]
then
    if [[ -d /etc/debian_version ]]
    then
        LIBDIR=lib/x86_64-linux-gnu
    else
        LIBDIR=lib64
    fi
elif [[ $(uname) == 'Darwin' ]]
then
    LIBDIR=lib
fi

THREADWEAVERCMAKEDIR="$SDK_INSTALL_PREFIX/$LIBDIR/cmake/KF5ThreadWeaver/"
THREADWEAVERLIBDIR="$SDK_INSTALL_PREFIX/$LIBDIR"
THREADWEAVERINCLUDEDIR="$SDK_INSTALL_PREFIX/include/KF5"
