/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�fics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQCONFIGURATIONSCREEN_H
#define UDGQCONFIGURATIONSCREEN_H

#include <qconfigurationscreenbase.h>
#include "pacslistdb.h"

namespace udg {

/** Interf�cie que permet configurar els par�metres del pacs i de la cach�
@author marc
*/
class QConfigurationScreen : public QConfigurationScreenBase
{
Q_OBJECT

private :

    //PacsListDB m_pacsList;
    int m_PacsID;

    bool validatePacsParameters();
    bool validateChanges();
    void fillPacsListView();

    void databaseError(Status *,PacsParameters *);
    void databaseError(Status *);
    
    void loadCacheDefaults();
    void loadPacsDefaults();
    void loadCachePoolDefaults();
    
    void applyChangesPacs();
    void applyChangesCache();
   
     
public slots :

    /**Configuraci� de la llista de Pacs*/
    void clear();
    void addPacs();
    void selectedPacs(QListViewItem * item);
    void updatePacs();
    void deletePacs();
    void test();
    
    /*Configuraci� de la cach�*/
    void examinateDataBaseRoot();
    void examinateCacheImagePath();

    void deleteStudies();
    void compactCache();
    
    /*Configuraci� dels par�metres del Pacs */
    void applyChanges();
    void acceptChanges();
    void cancelChanges();

    void configurationChanged(int);
    void configurationChanged(const QString&);
       
public:
    QConfigurationScreen(QWidget *parent = 0, const char *name = 0);

    ~QConfigurationScreen();

};

};

#endif
