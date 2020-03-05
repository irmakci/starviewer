#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

mkdir -p "${SOURCE_DIR_BASE}"

for LIB in $LIBS
do
    case $LIB in
        qt)
            echo "Please install Qt on ${INSTALL_QTDIR}"

            if [[ $(uname) == 'Linux' ]]
            then
                chmod u+x "${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.6.3.run"
                "${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.6.3.run"
            elif [[ $(uname) == 'Darwin' ]]
            then
                hdiutil mount "${DOWNLOAD_PREFIX}/qt-opensource-mac-x64-clang-5.6.3.dmg"
                open -W /Volumes/qt-opensource-mac-x64-clang-5.6.3/qt-opensource-mac-x64-clang-5.6.3.app
                hdiutil unmount /Volumes/qt-opensource-mac-x64-clang-5.6.3
            fi
            ;;
        dcmtk)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/dcmtk-3.6.1_20120515.tar.gz"
            ;;
        vtk)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/VTK-7.0.0.tar.gz"
            ;;
        gdcm)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/GDCM-2.8.6.tar.gz"
            ;;
        itk)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/InsightToolkit-4.10.0.tar.xz"
            ;;
        ecm)
            mkdir -p "${SOURCE_DIR_BASE}/threadweaver-5.3.0"
            tar -C "${SOURCE_DIR_BASE}/threadweaver-5.3.0" -xvf "${DOWNLOAD_PREFIX}/extra-cmake-modules-1.3.0.tar.xz"
            ;;
        threadweaver)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/threadweaver-5.3.0.tar.xz"
            ;;
    esac
done
