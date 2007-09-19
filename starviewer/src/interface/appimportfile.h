/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGAPPIMPORTFILE_H
#define UDGAPPIMPORTFILE_H

#include <QObject>
#include <QStringList>

namespace udg {

// Fordward Declarations
class VolumeRepository;

/**
Mini-aplicació encarregada de permetre carregar un model des del sistema de fitxers al repositori de volums

@author Grup de Gràfics de Girona  ( GGG )
*/
class AppImportFile : public QObject{
Q_OBJECT
public:

    AppImportFile(QObject *parent = 0, QString name = 0);
    ~AppImportFile();

    /// Obre el diàleg per poder obrir arxius, retornant true si s'ha obert una imatge, false si no
    bool open();

    /// Obre el diàleg per poder obrir arxius d'un directori DICOM
    bool openDirectory();

signals:
    /// Signal que s'emet quan s'escull obrir un Key Image Note. Es passa el path d'aquest
    void openKeyImageNote( const QString & path);

    /// Signal que s'emet quan s'escull obrir un Presentation State. Es passa el path d'aquest
    void openPresentationState( const QString & path);

    /// Senyal que s'emet quan s'han escollit un o més arxius que seran processats externament
    void selectedFiles( QStringList );

private:
    /**
     * Ens retorna la llista d'arxius DICOM agrupables en series d'un directori
     * @param directory Nom del directori on es troben els arxius
     * @return La llista de noms de fitxers ordenada
     */
    QStringList generateFilenames( QString dirPath );

    /// llegeix escriu configuracions
    void readSettings();
    void writeSettings();

private:
    /// Directori de treball per fitxers ordinaris
    QString m_workingDirectory;

    /// Directori de treball per directoris dicom
    QString m_workingDicomDirectory;
};

};  //  end  namespace udg

#endif
