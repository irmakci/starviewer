# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/inputoutput
# L'objectiu és una biblioteca:  

TRANSLATIONS += inputoutput_ca_ES.ts \
                inputoutput_es_ES.ts \
                inputoutput_en_GB.ts 
FORMS += qconfigurationscreenbase.ui \
         qpacslistbase.ui \
         qstudytreewidgetbase.ui \
         qserieslistwidgetbase.ui \
         qcreatedicomdirbase.ui \
         qoperationstatescreenbase.ui \
         qchooseoneobjectdialogbase.ui \
         queryscreenbase.ui
HEADERS += const.h \
           databaseconnection.h \
           dicomimage.h \
           imagelist.h \
           pacsconnection.h \
           pacslistdb.h \
           pacslist.h \
           pacsnetwork.h \
           pacsparameters.h \
           pacsserver.h \
           processimage.h \
           processimagesingleton.h \
           retrieveimages.h \
           dicomseries.h \
           serieslist.h \
           serieslistsingleton.h \
           starviewersettings.h \
           status.h \
           struct.h \
           dicomstudy.h \
           studylistsingleton.h \
           studylist.h \
           cachepool.h \
           scalestudy.h \
           multiplequerystudy.h \
           qquerystudythread.h \
           operation.h \
           queueoperationlist.h \
           cacheinstallation.h \
           qexecuteoperationthread.h \
           cachelayer.h \
           cacheimagedal.h \
           cacheseriesdal.h \
           cachestudydal.h \
           converttodicomdir.h \
           convertdicomtolittleendian.h \
           createdicomdir.h \
           cachetools.h \
           dicomdirreader.h \
           storeimages.h \
           starviewerprocessimagestored.h \
           starviewerprocessimageretrieved.h \
           imagelistsingleton.h \
           querypacs.h \
           dicommask.h \
           importdicomdir.h \
           qconfigurationscreen.h \
           qpacslist.h \
           qstudytreewidget.h \
           qserieslistwidget.h \
           qcreatedicomdir.h \
           qoperationstatescreen.h \
           qchooseoneobjectdialog.h \
           queryscreen.h
SOURCES += databaseconnection.cpp \
           dicomimage.cpp \
           imagelist.cpp \
           pacsconnection.cpp \
           pacslist.cpp \
           pacslistdb.cpp \
           pacsnetwork.cpp \
           pacsparameters.cpp \
           pacsserver.cpp \
           processimage.cpp \
           processimagesingleton.cpp \
           retrieveimages.cpp \
           dicomseries.cpp \
           serieslist.cpp \
           serieslistsingleton.cpp \
           starviewersettings.cpp \
           status.cpp \
           dicomstudy.cpp \
           studylistsingleton.cpp \
           studylist.cpp \
           cachepool.cpp \
           scalestudy.cpp \
           multiplequerystudy.cpp \
           qquerystudythread.cpp \
           operation.cpp \
           queueoperationlist.cpp \
           cacheinstallation.cpp \
           qexecuteoperationthread.cpp \
           cachelayer.cpp \
           cacheimagedal.cpp \
           cacheseriesdal.cpp \
           cachestudydal.cpp \
           converttodicomdir.cpp \
           convertdicomtolittleendian.cpp \
           createdicomdir.cpp \
           cachetools.cpp \
           dicomdirreader.cpp \
           storeimages.cpp \
           starviewerprocessimagestored.cpp \
           starviewerprocessimageretrieved.cpp \
           imagelistsingleton.cpp \
           querypacs.cpp \
           dicommask.cpp \
           importdicomdir.cpp \
           qconfigurationscreen.cpp \
           qpacslist.cpp \
           qstudytreewidget.cpp \
           qserieslistwidget.cpp \
           qcreatedicomdir.cpp \
           qoperationstatescreen.cpp \
           qchooseoneobjectdialog.cpp \
           queryscreen.cpp
TARGETDEPS += ../core/libcore.a
LIBS += ../core/libcore.a
INCLUDEPATH += ../core
DEPENDPATH += ../core
MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += warn_on \
staticlib
TEMPLATE = lib
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../compilationtype.inc)
