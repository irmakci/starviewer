/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "converttodicomdir.h"

#include <QString>
#include <QProgressDialog>
#include <QStringList>
#include <QDir>
#include <QChar>
#include <QApplication>
#include <QFile>
#include <QTextStream>

#include "logging.h"
#include "status.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "seriesmask.h"
#include "imagemask.h"
#include "serieslist.h"
#include "imagelist.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "convertdicomtolittleendian.h"
#include "starviewersettings.h"
#include "createdicomdir.h"
#include "deletedirectory.h"
#include "starviewersettings.h"

namespace udg {

ConvertToDicomdir::ConvertToDicomdir()
{
    QDir dicomDir;

    m_study = 0;
    m_series = 0;
    m_image = 0;
    m_patient = 0;
}

void ConvertToDicomdir::addStudy( QString studyUID )
{
    /*Els estudis s'han d'agrupar per pacient, el que fem �s afegir-los a llista d'estudis per convertir a 
     dicomdir ja ordenats per pacient*/
    StudyToConvert studyToConvert;
    CacheStudyDAL cacheStudyDAL;
    Study study;
    int index = 0;
    bool stop = false;
    
    cacheStudyDAL.queryStudy( studyUID.toAscii().constData() , study ); //busquem patientID
    
    studyToConvert.studyUID = studyUID;
    studyToConvert.patientId = study.getPatientId().c_str();
    
    while ( index < m_studiesToConvert.count() && !stop )  //busquem la posici� on s'ha d'inserir l'estudi a llista d'estudis per convertir a dicomdir, ordenant per id de pacient
    {
        if ( studyToConvert.patientId < m_studiesToConvert.at( index ).patientId ) //comparem amb els altres estudis de la llista, fins trobar el seu llloc corresponentm 
        {
            stop = true;
        }
        else index++;
    }
    
    if ( stop ) //una vegada hem trobat la posici� on ha d'anar l'inserim
    {
        m_studiesToConvert.insert( index , studyToConvert );    
    }
    else m_studiesToConvert.push_back( studyToConvert );//en aquest cas val al final
}

Status ConvertToDicomdir::convert( QString dicomdirPath )
{
    /* Primer copiem els estudis al directori desti, i posteriorment convertim el directori en un dicomdir*/
    CacheImageDAL cacheImageDAL;
    ImageMask imageMask;
    Status state;
    int imageNumberStudy , imageNumberTotal = 0 , i = 0;
    CreateDicomdir createDicomdir;
    QString studyUID;
    
    m_dicomDirPath = dicomdirPath;
    
    //comptem el numero d'imatges pel progress de la barra
    while ( i < m_studiesToConvert.count() )
    {   
        imageMask.setStudyUID( m_studiesToConvert.value( i ).studyUID.toAscii().constData() );
        state = cacheImageDAL.countImageNumber( imageMask , imageNumberStudy );
        if ( !state.good() ) break;
            
        imageNumberTotal = imageNumberStudy + imageNumberTotal;
        i++;
    }

    if ( !state.good() )
    {
        deleteStudies();
        return state;
    }

    //sumem una imatge m�s per evitar que arribi el 100 % la progress bar, i aix� s'esperi a que es crei el dicomdir, que es fa quan s'invoca createDicomdir.Create()
    m_progress = new QProgressDialog( tr( "Creating Dicomdir..." ) , "" , 0 , imageNumberTotal + 1 );
    m_progress->setMinimumDuration( 0 );
    m_progress->setCancelButton( 0 );

    //copiem les imatges dels estudis seleccionats al directori desti
    state = startConversionToDicomdir();
 
    if ( !state.good() )
    {
        deleteStudies();
        return state;
    }
   
    state = createDicomdir.create ( m_dicomDirPath.toAscii().constData() );//invoquem el m�tode per convertir el directori dest� Dicomdir on ja s'han copiat les imatges en un dicomdir
    
    m_progress->close();
    
    if ( !state.good() )
    {
        deleteStudies();
    }

    return state;
}

Status ConvertToDicomdir::startConversionToDicomdir()
{
    StudyToConvert studyToConvert;
    QString m_OldPatientId;
    QString patientNameDir;
    QDir patientDir;
    Status state;
    QChar fillChar = '0'; 
    
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    
    m_patient = 0;

    //agrupem estudis1 per pacient, com que tenim la llista ordenada per patientId
    while ( !m_studiesToConvert.isEmpty() )
    {
        studyToConvert = m_studiesToConvert.takeFirst();
        
        //si el pacient es diferent creem un nou directori PAtient
        if ( m_OldPatientId != studyToConvert.patientId )
        {
            patientNameDir = QString( "/PAT%1" ).arg( m_patient , 5 , 10 , fillChar );
            m_dicomdirPatientPath = m_dicomDirPath + patientNameDir;
            patientDir.mkdir( m_dicomdirPatientPath );
            m_patient++;
            m_study = 0;
            m_patientDirectories.push_back( m_dicomdirPatientPath );//creem una llista amb els directoris creats, per si es produeix algun error esborrar-los
        }
        
        state = convertStudy( studyToConvert.studyUID );
        if ( !state.good() ) break;
        m_OldPatientId = studyToConvert.patientId;
    }
    
    QApplication::restoreOverrideCursor();

    return state;
}


Status ConvertToDicomdir::convertStudy( QString studyUID )
{
    /*Creem el directori de l'estudi on es mour� un estudi seleccionat per convertir a dicomdir*/
    CacheSeriesDAL cacheSeriesDAL;
    QDir studyDir;
    QChar fillChar = '0';    
    QString studyName = QString( "/STU%1" ).arg( m_study , 5 , 10 , fillChar );
    SeriesList seriesList;
    SeriesMask seriesMask;
    Series series;
    Status state;
    
    m_study++;
    m_series = 0;
    
    //Creem el directori on es guardar l'estudi en format DicomDir
    m_dicomDirStudyPath = m_dicomdirPatientPath + studyName;
    studyDir.mkdir( m_dicomDirStudyPath );

    seriesMask.setStudyUID( studyUID.toAscii().constData() );
    
    state = cacheSeriesDAL.querySeries( seriesMask , seriesList ); //cerquem s�ries de l'estudi

    if ( !state.good() ) return state;
    
    seriesList.firstSeries();

    while ( !seriesList.end() ) //per cada s�rie de l'estudi, creem el directori de la s�rie
    {
        state = convertSeries( seriesList.getSeries() );
        
        if ( !state.good() ) 
        {
            break;
        }
        else seriesList.nextSeries();
    }

    return state;
}

Status ConvertToDicomdir::convertSeries( Series series )
{
    QDir seriesDir;
    QChar fillChar = '0';    
    CacheImageDAL cacheImageDAL;
    //creem el nom del directori de la s�rie, el format �s SERXXXXX, on XXXXX �s el numero de s�rie dins l'estudi
    QString seriesName = QString( "/SER%1" ).arg( m_series , 5 , 10 , fillChar ); 
    Image image;
    ImageMask imageMask;
    ImageList imageList;
    Status state;

    m_series++;
    m_image = 0;
    //Creem el directori on es guardar� la s�rie en format DicomDir
    m_dicomDirSeriesPath = m_dicomDirStudyPath + seriesName;
    seriesDir.mkdir( m_dicomDirSeriesPath );

    imageMask.setSeriesUID( series.getSeriesUID() );
    imageMask.setStudyUID( series.getStudyUID() );

    state = cacheImageDAL.queryImages( imageMask , imageList ); // cerquem imatges de la s�rie

    if ( !state.good() ) return state;
    
    imageList.firstImage();

    while ( !imageList.end() ) //per cada imatge de la s�rie, la convertim a foramt littleEndian, i la copiem al directori desti
    {
        state = convertImage( imageList.getImage() );
        
        if ( !state.good() )
        {
            break;
        }
        else imageList.nextImage();
    }
    
    return state;
}

Status ConvertToDicomdir::convertImage( Image image )
{
    QChar fillChar = '0';    
    //creem el nom del fitxer de l'imatge, el format �s IMGXXXXX, on XXXXX �s el numero d'imatge dins la s�rie
    QString  imageName = QString( "/IMG%1" ).arg( m_image , 5 , 10 , fillChar ) , imageInputPath , imageOutputPath; 
    ConvertDicomToLittleEndian convertDicom;
    StarviewerSettings settings;
    Series serie;
    Status state;    
    
    m_image++;

    //Creem el path de la imatge
    imageInputPath.insert( 0 , settings.getCacheImagePath() );
    imageInputPath.append( image.getStudyUID().c_str() );
    imageInputPath.append( "/" );
    imageInputPath.append( image.getSeriesUID().c_str() );
    imageInputPath.append( "/" );
    imageInputPath.append( image.getImageName().c_str() );

    imageOutputPath = m_dicomDirSeriesPath + imageName;

    //convertim la imatge a littleEndian, demanat per la normativa DICOM i la guardem al directori desti
    state = convertDicom.convert( imageInputPath.toAscii().constData() , imageOutputPath.toAscii().constData() );

     m_progress->setValue( m_progress->value() + 1 ); // la barra de progr�s avan�a
     m_progress->repaint();
    
    return state;
}

void ConvertToDicomdir::deleteStudies()
{
    DeleteDirectory deleteDirectory;
    
    while ( !m_patientDirectories.isEmpty() )
    {
        deleteDirectory.deleteDirectory( m_patientDirectories.takeFirst() , true );
    }
}

void ConvertToDicomdir::createReadmeTxt( QString readmeTxtPath )
{
    QString readmeFilePath = readmeTxtPath + "/README.TXT";
    QFile file( readmeFilePath );
    StarviewerSettings settings;
    
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) return;

    QTextStream out( &file );
    
    if ( settings.getInstitutionName().length() > 0)
    {
        out << "Dicomdir Generated by : " << settings.getInstitutionName() << "\n";
        
        if ( settings.getInstitutionAddress().length() > 0) out << settings.getInstitutionAddress() << "\n";
        
        if ( settings.getInstitutionTown().length() > 0 ) out << settings.getInstitutionTown() << " " << settings.getInstitutionZipCode() << "\n"; 
        
        if ( settings.getInstitutionCountry().length() > 0 ) out << settings.getInstitutionCountry() << "\n";
        
        if ( settings.getInstitutionPhoneNumber().length() > 0 ) out << "Phone number "  << settings.getInstitutionPhoneNumber() << "\n";
        
        if ( settings.getInstitutionEmail().length() > 0 ) out << "E-mail contact : " << settings.getInstitutionEmail() << "\n";
        
        out << "\n" << "\n";
    }
    
    out << "The dicomdir has been generated with Starviewer version 0.1" << "\n";
    out << "E-mail contact : vismed@ima.udg.es" << "\n";
    
    file.close();
}

ConvertToDicomdir::~ConvertToDicomdir()
{
}

}
