/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOL_H
#define UDGHANGINGPROTOCOL_H

#include <QObject>

namespace udg {

class HangingProtocolLayout;
class HangingProtocolMask;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocol : public QObject
{
Q_OBJECT
public:
    HangingProtocol( QObject *parent = 0 );

    ~HangingProtocol();

    /// Posar el nom al hanging protocol
    void setName( QString name );

    /// Definició dels diferents nivells que pot tenir un Hanging Protocol
    enum HangingProtocolLevel { MANUFACTURER, SITE, USER_GROUP, SINGLE_USER };

    /// Obtenir el layout
    HangingProtocolLayout * getHangingProtocolLayout();

    ///Obtenir la mascara
    HangingProtocolMask * getHangingProtocolMask();

    /// Assigna el nombre de screens
    void setNumberOfScreens( int screens );

    /// Assigna els protocols que tracta el hanging protocol
    void setProtocolsList( QList<QString> protocols );

    /// Assigna les posicions de les finestres
    void setDisplayEnvironmentSpatialPositionList( QList<QString> positions );

    /// Afegeix un image set
    void addImageSet ( HangingProtocolImageSet * imageSet );

    /// Afegeix un display set
    void addDisplaySet ( HangingProtocolDisplaySet * displaySet );

    /// Obtenir el nom del hanging protocol
    QString getName();

    /// Obté el nombre total d'image sets
    int getNumberOfImageSets();

	/// Obté el nombre total d'image sets
    int getNumberOfDisplaySets();

    /// Obté l'image set amb identificador "identificador"
    HangingProtocolImageSet * getImageSet( int identificador );

	/// Obté el display set amb identificador "identificador"
	HangingProtocolDisplaySet * getDisplaySet( int identificador );

    /// Obté el display set corresponent a l'image set amb l'identificador detallat
    HangingProtocolDisplaySet * getDisplaySetOfImageSet( int numberOfImageSet );

    /// Mètode per mostrar els valors
    void show();

	/// Posar l'identificador al hanging protocol
	void setIdentifier( int id );

	/// Obtenir l'identificador del hanging protocol
	int getIdentifier();

	/// Mètode per comparar hanging protocols
	bool gratherThan( HangingProtocol * hangingToCompare );

	/// Retorna si el mètode és estricte o no ho hes
	bool getStrictness();

	/// Assigna si el mètode és estricte o no ho hes
	void setStrictness( bool strictness );

private:

	/// Identificador
	int m_identifier;

    /// Nom del hanging protocol
    QString m_name;

    /// Descripció del hanging protocol
    QString m_description;

    /// Nivell del hanging protocol
    HangingProtocolLevel m_level;

    /// Creador del hanging protocol
    QString m_creator;

    /// Data i hora de creació del hanging protocol
    QString m_dateTime;

	/// Saber si es candidat
	bool m_candidate;

    /// Definició de layouts
    HangingProtocolLayout * m_layout;

    /// Definició de la màscara
    HangingProtocolMask * m_mask;

    /// Llista d'image sets
    QList< HangingProtocolImageSet * > m_listOfImageSets;

    /// Llista de displays sets
    QList< HangingProtocolDisplaySet * > m_listOfDisplaySets;

	/// Boolea que indica si és estricte o no. Si és estricte vol dir que per ser correcte tots els image sets han d'estar assignats.
	bool m_strictness;

};

}

#endif
