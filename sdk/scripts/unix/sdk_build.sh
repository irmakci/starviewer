#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

for BUILD_TYPE in $BUILD_TYPES
do
    # Dont' use LIB to not overwrite LIB variable in Windows (note if variable name is changed it must be changed accordingly in cmakebuild.sh)
    for ALIB in $LIBS
    do
        if [[ -f "$SCRIPTS_ROOT/libs/$ALIB.sh" ]]
        then
            . "$SCRIPTS_ROOT/config/$ALIB.sh"
            . "$SCRIPTS_ROOT/libs/$ALIB.sh"
        fi
    done
done

# Generate SDK enviornment configuration file
echo "#!/bin/bash" > $SDK_ENVIRONMENT_FILE
echo "# Environment variables to compile with the SDK." >> $SDK_ENVIRONMENT_FILE
echo "# To enter inside the environment run this file using 'source' command." >> $SDK_ENVIRONMENT_FILE
echo "SDK_INSTALL_PREFIX=$SDK_INSTALL_PREFIX" >> $SDK_ENVIRONMENT_FILE
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> $SDK_ENVIRONMENT_FILE
echo "QTDIR=$QTDIR" >> $SDK_ENVIRONMENT_FILE
echo "DCMTKLIBDIR=$DCMTKLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "DCMTKINCLUDEDIR=$DCMTKINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "VTKLIBDIR=$VTKLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "VTKINCLUDEDIR=$VTKINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "GDCMLIBDIR=$GDCMLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "GDCMINCLUDEDIR=$GDCMINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "ITKLIBDIR=$ITKLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "ITKINCLUDEDIR=$ITKINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "THREADWEAVERLIBDIR=$THREADWEAVERLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "THREADWEAVERINCLUDEDIR=$THREADWEAVERINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE

echo "export SDK_INSTALL_PREFIX" >> $SDK_ENVIRONMENT_FILE
echo "export LD_LIBRARY_PATH" >> $SDK_ENVIRONMENT_FILE
echo "export QTDIR" >> $SDK_ENVIRONMENT_FILE
echo "export DCMTKLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "export DCMTKINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "export VTKLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "export VTKINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "export GDCMLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "export GDCMINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "export ITKLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "export ITKINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE
echo "export THREADWEAVERLIBDIR" >> $SDK_ENVIRONMENT_FILE
echo "export THREADWEAVERINCLUDEDIR" >> $SDK_ENVIRONMENT_FILE

chmod ugo+x $SDK_ENVIRONMENT_FILE
