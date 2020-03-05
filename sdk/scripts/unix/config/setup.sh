#!/bin/bash

# Base directory for everything related to the SDK.
SDK_BASE_PREFIX=${SDK_BASE_PREFIX:-"$HOME/starviewer-sdk-0.15"}

# Where the libraries are dowloaded.
DOWNLOAD_PREFIX=${DOWNLOAD_PREFIX:-"$SDK_BASE_PREFIX/downloads"}

# Where the user will be asked to install Qt.
INSTALL_QTDIR=${INSTALL_QTDIR:-"$HOME/Qt5.6.3"}

# Where to install the SDK libraries once compiled.
SDK_INSTALL_PREFIX=${SDK_INSTALL_PREFIX:-"$SDK_BASE_PREFIX/usr/local"}

# Location of SDK sources to build.
SOURCE_DIR_BASE=${SOURCE_DIR_BASE:-"$SDK_INSTALL_PREFIX/src"}

# List of build types to use. Possible values: debug, release.
BUILD_TYPES="release"
# List of libs to build. Possible values: dcmtk, vtk, gdcm, itk, ecm, threadweaver.
LIBS="dcmtk vtk gdcm itk ecm threadweaver"

# Where to write the SDK environment configuration script.
SDK_ENVIRONMENT_FILE=$SCRIPTS_ROOT/../../../environment.sh

# If you use a local Qt installaton, the path where it is installed
QTDIR=$INSTALL_QTDIR/5.6.3/gcc_64

# Starviewer source code location
STARVIEWER_SOURCE_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer

# Starviwer shadow build directory
STARVIEWER_BUILD_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer-build

# Temporary directory for package creation
DPKG_TMP=/tmp/starviewer-dpkg

# Where to place the packages
DPKG_DESTINATION=$SCRIPTS_ROOT/../../../

# Location of the pathes to apply on SDK libraries
PATCHES_ROOT=$SCRIPTS_ROOT/../../patches

# Uncomment if you want to use distcc (distributed C compiler) on CMake.
#CMAKE_DISTCC='-DCMAKE_CXX_COMPILER_LAUNCHER:STRING=distcc -DCMAKE_C_COMPILER_LAUNCHER:STRING=distcc'

# c++11 support
CMAKE_CPP11='-DCMAKE_CXX_STANDARD:STRING=11 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON -DCMAKE_C_STANDARD:STRING=11 -DCMAKE_C_STANDARD_REQUIRED:BOOL=ON'

# Uncomment to enable compilation with clang
#CMAKE_COMPILER='-DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang'

# Verbose compilation: Uncomment to output the compiler calls.
MAKE_VERBOSE='VERBOSE=yes'

# Number of simultaneous make jobs (-j8)
MAKE_CONCURRENCY=4

# Because SDK libraries binares are not on a standard location.
# This environment variable has to be set when starting starviewer binary or
# when you compile it. (if not the linker will fail).
LD_LIBRARY_PATH=$SDK_INSTALL_PREFIX/lib:$SDK_INSTALL_PREFIX/lib/x86_64-linux-gnu:$QTDIR/lib/
