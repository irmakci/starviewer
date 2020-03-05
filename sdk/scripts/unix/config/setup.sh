#!/bin/bash

# Base directory for everything related to the SDK.
SDK_BASE_PREFIX=${SDK_BASE_PREFIX:-"$HOME/starviewer-sdk-0.15"}

# Where the libraries are dowloaded.
DOWNLOAD_PREFIX=${DOWNLOAD_PREFIX:-"$SDK_BASE_PREFIX/downloads"}

# Where the user will be asked to install Qt.
INSTALL_QTDIR=${INSTALL_QTDIR:-"$HOME/Qt5.6.3"}

# Directory where the specific version of Qt is installed.
if [[ $(uname) == 'Linux' ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.6.3/gcc_64"}
elif [[ $(uname) == 'Darwin' ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.6.3/clang_64"}
fi

# Where to install the SDK libraries once compiled.
SDK_INSTALL_PREFIX=${SDK_INSTALL_PREFIX:-"$SDK_BASE_PREFIX/usr/local"}

# Location of SDK sources to build.
SOURCE_DIR_BASE=${SOURCE_DIR_BASE:-"$SDK_INSTALL_PREFIX/src"}

# List of build types to use. Possible values: debug, release.
BUILD_TYPES=${BUILD_TYPES:-"release"}

# List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
LIBS=${LIBS:-"dcmtk vtk gdcm itk ecm threadweaver"}

# CMake executable
if [[ $(uname) == 'Linux' ]]
then
    CMAKE=${CMAKE:-cmake}
elif [[ $(uname) == 'Darwin' ]]
then
    CMAKE=${CMAKE:-"/Applications/CMake.app/Contents/bin/cmake"}
fi

# Set to appropriate value (example below) if you want to use distcc (distributed C compiler) on CMake.
CMAKE_DISTCC=${CMAKE_DISTCC:-}
#CMAKE_DISTCC='-DCMAKE_CXX_COMPILER_LAUNCHER:STRING=distcc -DCMAKE_C_COMPILER_LAUNCHER:STRING=distcc'

# Set to appropriate value (example below) to compile with non-default compiler (default is gcc on Linux and clang in Mac).
CMAKE_COMPILER=${CMAKE_COMPILER:-}
#CMAKE_COMPILER='-DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang'

# Number of simultaneous make jobs.
MAKE_CONCURRENCY=${MAKE_CONCURRENCY:--j4}

# Verbose compilation: set to 'VERBOSE=yes' to output the compiler calls.
MAKE_VERBOSE=${MAKE_VERBOSE:-}

# Where to write the SDK environment configuration script.
SDK_ENVIRONMENT_FILE=$SCRIPTS_ROOT/../../../environment.sh

# Starviewer source code location
STARVIEWER_SOURCE_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer

# Starviwer shadow build directory
STARVIEWER_BUILD_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer-build

# Temporary directory for package creation
DPKG_TMP=/tmp/starviewer-dpkg

# Where to place the packages
DPKG_DESTINATION=$SCRIPTS_ROOT/../../../

# c++11 support
CMAKE_CPP11='-DCMAKE_CXX_STANDARD:STRING=11 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON -DCMAKE_C_STANDARD:STRING=11 -DCMAKE_C_STANDARD_REQUIRED:BOOL=ON'

# Because SDK libraries binares are not on a standard location.
# This environment variable has to be set when starting starviewer binary or
# when you compile it. (if not the linker will fail).
LD_LIBRARY_PATH=$SDK_INSTALL_PREFIX/lib:$SDK_INSTALL_PREFIX/lib/x86_64-linux-gnu:$QTDIR/lib/
