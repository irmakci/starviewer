include(defaultdirectories.pri)

INCLUDEPATH += $${VTKINCLUDEDIR}

VTKLIBSUFFIX = -8.2
VTKLIBS = \
    vtkCommonCore \
    vtkCommonDataModel \
    vtkCommonExecutionModel \
    vtkCommonMath \
    vtkCommonMisc \
    vtkCommonSystem \
    vtkCommonTransforms \
    vtkFiltersCore \
    vtkFiltersGeneral \
    vtkFiltersSources \
    vtkGUISupportQt \
    vtkGUISupportQtOpenGL \
    vtkImagingCore \
    vtkImagingGeneral \
    vtkImagingHybrid \
    vtkImagingMath \
    vtkInteractionStyle \
    vtkInteractionWidgets \
    vtkIOImage \
    vtkRenderingAnnotation \
    vtkRenderingCore \
    vtkRenderingFreeType \
    vtkRenderingImage \
    vtkRenderingOpenGL2 \
    vtkRenderingVolume \
    vtkRenderingVolumeOpenGL2

LIBS += -L$${VTKLIBDIR}

for(lib, VTKLIBS) {
    LIBS += -l$${lib}$${VTKLIBSUFFIX}
}
