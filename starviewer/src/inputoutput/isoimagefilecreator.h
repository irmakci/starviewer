/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGISOIMAGEFILECREATOR_H
#define UDGISOIMAGEFILECREATOR_H

#include <QString>

namespace udg {

/** Crea un arxiu d'imatge ISO a partir d'un directori o fitxer.
    @author Grup de Gr�fics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class IsoImageFileCreator {

public:

    enum ImageFileCreationError { InputPathNotFound, OutputPathNotFound, OutputDirPathAccessDenied, OutputFilePathAccessDenied, InternalError };

    IsoImageFileCreator();
    IsoImageFileCreator( const QString &inputPath, const QString &outputIsoImageFilePath );
    ~IsoImageFileCreator();

    /// Estableix/Retorna el label del fitxer d'imatge ISO que es vol crear
    QString getIsoImageLabel() const;
    void setIsoImageLabel( const QString &isoImageLabel );

    /// Estableix/Retorna el path del fitxer o directori a partir del qual es vol crear el fitxer d'imatge ISO
    QString getInputPath() const;
    void setInputPath( const QString &inputPath );

    /// Estableix/Retorna el path del fitxer d'imatge ISO que es vol crear
    QString getOutputIsoImageFilePath() const;
    void setOutputIsoImageFilePath( const QString &outputIsoImageFilePath );
  
    /// Retorna la descripci� de l'�ltim error que s'ha produit
    QString getLastErrorDescription() const;

    /// Retorna l'�ltim error que s'ha produit
    ImageFileCreationError getLastError() const;

    /** Crea el fitxer d'imatge ISO a partir d'un directori o fitxer
     * El path del fitxer d'imatge ISO que es crear� es troba a l'atribut m_inputPath
     * El path del directori o fitxer a partir del qual es crea el fitxer d'imatge ISO es troba a l'atribut m_outputIsoImageFilePath
     * @return true si la imatge s'ha creat correctament i false en cas contrari
     */
    bool createIsoImageFile();

private:

    /// Label de la imatge ISO que es vol crear
    QString m_isoImageLabel;

    /// Path del directori o fitxer a partir del qual volem generar el fitxer d'imatge ISO
    QString m_inputPath;

    /// Path del fitxer d'imatge ISO que es vol generar
    QString m_outputIsoImageFilePath;

    /// Descripci� de l'�ltim error que s'ha produit
    QString m_lastErrorDescription;

    /// �ltim error que s'ha produit
    ImageFileCreationError m_lastError;
};

}

#endif
