/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "extensioncontext.h"

#include "patient.h"
#include "volume.h"
#include "logging.h"

namespace udg {

ExtensionContext::ExtensionContext()
{
}

ExtensionContext::~ExtensionContext()
{
}

Patient* ExtensionContext::getPatient() const
{
    return m_patient;
}

void ExtensionContext::setPatient(Patient *patient)
{
    m_patient = patient;
}

Volume *ExtensionContext::getDefaultVolume() const
{
    Volume *defaultVolume = NULL;
    Series *defaultSeries = NULL;
    bool searchForDefaultSeries = false;
    QList<Series*> selectedSeries = m_patient->getSelectedSeries();

    if (selectedSeries.isEmpty())
    {
        searchForDefaultSeries = true;
    }
    else
    {
        // TODO de moment només agafem la primera de les possibles seleccionades
        defaultSeries = selectedSeries.at(0);
        // Necessitem que les sèries siguin visualitzables
        if (!defaultSeries->isViewable())
        {
            searchForDefaultSeries = true;
        }
        else
        {
            defaultVolume = defaultSeries->getFirstVolume();
        }
    }
    // En comptes de searchForDefaultSeries podríem fer servir
    // defaultVolume, però amb la var. booleana el codi és més llegible
    if (searchForDefaultSeries)
    {
        bool ok = false;
        foreach (Study *study, m_patient->getStudies())
        {
            QList<Series*> viewableSeries = study->getViewableSeries();
            if (!viewableSeries.isEmpty())
            {
                ok = true;
                defaultVolume = viewableSeries.at(0)->getFirstVolume();
                break;
            }
        }
        if (!ok)
        {
            ERROR_LOG("No hi ha cap serie de l'actual pacient que sigui visualitzable. Retornem volum NUL.");
        }
    }

    return defaultVolume;
}

DicomEntityFlags ExtensionContext::getDicomEntities(QList<Study*> studies) const
{
    if (studies.isEmpty())
    {
        if (m_patient)
        {
            studies = m_patient->getStudies();
        }
    }

    DicomEntityFlags entities;

    foreach (Study *study, studies)
    {
        foreach (Series *series, study->getSeries())
        {
            if (series->hasImages())
            {
                entities.setFlag(DicomEntity::Image);
            }
            if (series->hasEncapsulatedDocuments())
            {
                entities.setFlag(DicomEntity::EncapsulatedDocument);
            }
        }
    }

    return entities;
}

}
