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
                chmod u+x "${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.12.6.run"
                https_proxy="http://0.0.0.0" "${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.12.6.run" --no-force-installations TargetDir="${INSTALL_QTDIR}"
            elif [[ $(uname) == 'Darwin' ]]
            then
                hdiutil mount "${DOWNLOAD_PREFIX}/qt-opensource-mac-x64-5.12.6.dmg"
                https_proxy="http://0.0.0.0" open -W /Volumes/qt-opensource-mac-x64-5.12.6/qt-opensource-mac-x64-5.12.6.app --args --no-force-installations TargetDir="${INSTALL_QTDIR}"
                hdiutil unmount /Volumes/qt-opensource-mac-x64-5.12.6
            elif [[ $(uname) == 'MSYS_NT'* ]]
            then
                "${DOWNLOAD_PREFIX}/qt-opensource-windows-x86-5.12.6.exe" --no-force-installations TargetDir="${INSTALL_QTDIR}"
            fi
            ;;
        dcmtk)
            if [[ $(uname) == 'MSYS_NT'* ]]
            then
                unzip -o "${DOWNLOAD_PREFIX}/dcmtk-3.6.5-win64-support-MT-iconv-msvc-15.8.zip" -d "${SOURCE_DIR_BASE}"
            fi
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/dcmtk-DCMTK-3.6.5.tar.gz"
            ;;
        vtk)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/VTK-8.2.0.tar.gz"
            ;;
        gdcm)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/GDCM-3.0.4.tar.gz"
            ;;
        itk)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/InsightToolkit-5.0.1.tar.gz"
            ;;
        ecm)
            mkdir -p "${SOURCE_DIR_BASE}/threadweaver-5.64.0"
            tar -C "${SOURCE_DIR_BASE}/threadweaver-5.64.0" -xvf "${DOWNLOAD_PREFIX}/extra-cmake-modules-5.64.0.tar.gz"
            ;;
        threadweaver)
            tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/threadweaver-5.64.0.tar.gz"
            ;;
    esac
done
