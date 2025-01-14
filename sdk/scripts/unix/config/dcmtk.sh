#!/bin/bash

SOURCE_DIR="$SOURCE_DIR_BASE/dcmtk-DCMTK-3.6.5"

################ Nothing should need to be changed below this line ################

if [[ $(uname) == 'Linux' ]]
then
    ICONV=FALSE
    STDLIBC_ICONV=TRUE
    CMAKE_EXTRA_OPTIONS=
elif [[ $(uname) == 'Darwin' ]]
then
    ICONV=TRUE
    STDLIBC_ICONV=FALSE
    CMAKE_EXTRA_OPTIONS=-DCMAKE_PREFIX_PATH:PATH=/usr/local/opt/openssl
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    SUPPORT_LIBS_PATH="$SOURCE_DIR_BASE/dcmtk-3.6.5-win64-support-MT-iconv-msvc-15.8"

    cp "$SUPPORT_LIBS_PATH/libiconv-1.15/lib/charset_o.lib" "$SDK_INSTALL_PREFIX/lib"
    cp "$SUPPORT_LIBS_PATH/libiconv-1.15/lib/libiconv_o.lib" "$SDK_INSTALL_PREFIX/lib"

    ICONV=TRUE
    STDLIBC_ICONV=FALSE
    CMAKE_EXTRA_OPTIONS="-DWITH_ZLIBINC:PATH=$SUPPORT_LIBS_PATH/zlib-1.2.11 \
                         -DWITH_LIBICONVINC:PATH=$SUPPORT_LIBS_PATH/libiconv-1.15"
fi

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE \
               -DCMAKE_INSTALL_PREFIX:PATH=$SDK_INSTALL_PREFIX \
               -DDCMTK_WITH_TIFF:BOOL=FALSE \
               -DDCMTK_WITH_PNG:BOOL=FALSE \
               -DDCMTK_WITH_XML:BOOL=FALSE \
               -DDCMTK_WITH_ZLIB:BOOL=TRUE \
               -DDCMTK_WITH_OPENSSL:BOOL=TRUE \
               -DDCMTK_WITH_SNDFILE:BOOL=FALSE \
               -DDCMTK_WITH_ICONV:BOOL=$ICONV \
               -DDCMTK_WITH_ICU:BOOL=FALSE \
               -DDCMTK_WITH_STDLIBC_ICONV:BOOL=$STDLIBC_ICONV \
               -DDCMTK_WITH_WRAP:BOOL=FALSE \
               -DDCMTK_ENABLE_PRIVATE_TAGS:BOOL=TRUE \
               -DDCMTK_WITH_THREADS:BOOL=TRUE \
               -DDCMTK_WITH_DOXYGEN:BOOL=FALSE \
               -DDCMTK_ENABLE_CXX11:BOOL=TRUE \
               -DDCMTK_ENABLE_BUILTIN_DICTIONARY:BOOL=TRUE \
               -DDCMTK_ENABLE_EXTERNAL_DICTIONARY:BOOL=FALSE \
               $CMAKE_EXTRA_OPTIONS"

DCMTKLIBDIR="$SDK_INSTALL_PREFIX/$LIB64DIR"
DCMTKINCLUDEDIR="$SDK_INSTALL_PREFIX/include/dcmtk"
