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

#include "automaticsynchronizationtool.h"
#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "volume.h"
#include "imageplane.h"
#include "slicelocator.h"

#include <vtkPlane.h>

namespace udg {

AutomaticSynchronizationTool::AutomaticSynchronizationTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolData = 0;
    m_toolName = "AutomaticSynchronizationTool";
    m_hasSharedData = true;

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    m_sliceLocator = new SliceLocator;

    connect(m_2DViewer, SIGNAL(selected()), SLOT(changePositionIfActive()));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(changePositionIfActive()));
    
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(reset()));

    setToolData(new AutomaticSynchronizationToolData());
}

AutomaticSynchronizationTool::~AutomaticSynchronizationTool()
{
    delete m_sliceLocator;
}

void AutomaticSynchronizationTool::reset()
{
    initialize();
}

void AutomaticSynchronizationTool::setToolData(ToolData *data)
{
    this->m_toolData = dynamic_cast<AutomaticSynchronizationToolData*>(data);
    connect(m_toolData, SIGNAL(changed()), SLOT(updatePosition()));

    initialize();
}

void AutomaticSynchronizationTool::initialize()
{
    if (m_2DViewer->hasInput())
    {
        QString frameOfReferenceUID = m_2DViewer->getMainInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReferenceUID);

        if (groupOfActualViewer == -1)
        {
            if (frameOfReferenceUID != "")
            {
                m_toolData->setGroupForUID(frameOfReferenceUID, m_toolData->getNumberOfGroups());
            }
            else
            {
                DEBUG_LOG(QString("Viewer sense frameOfReference, no es tindrà en compte a la sincronitzacio automatica."));
            }
        }

        updateSliceLocator();
        changePositionIfActive();
        updatePosition();
    }
}

void AutomaticSynchronizationTool::updateSliceLocator()
{
    if (m_2DViewer)
    {
        m_sliceLocator->setVolume(m_2DViewer->getMainInput());
        m_sliceLocator->setPlane(m_2DViewer->getView());
    }
}

ToolData* AutomaticSynchronizationTool::getToolData() const
{
    return this->m_toolData;
}

void AutomaticSynchronizationTool::handleEvent(unsigned long eventID)
{
    Q_UNUSED(eventID);
}

void AutomaticSynchronizationTool::changePositionIfActive()
{
    if (m_2DViewer->hasInput() && m_2DViewer->isActive())
    {
        setPositionToToolData();
    }
}

void AutomaticSynchronizationTool::setPositionToToolData()
{
    QString frameOfReference = m_2DViewer->getMainInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
    Vector3 center = m_2DViewer->getCurrentImagePlane()->getCenter();
    m_toolData->setPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel(), center.toArray());
}

void AutomaticSynchronizationTool::updatePosition()
{
    if (m_2DViewer->hasInput() && !m_2DViewer->isActive())
    {
        QString frameOfReference = m_2DViewer->getMainInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();

        int activeGroup = m_toolData->getGroupForUID(m_toolData->getSelectedUID());
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReference);

        if (groupOfActualViewer != -1)
        {
            if (groupOfActualViewer == activeGroup && m_2DViewer->getCurrentAnatomicalPlaneLabel() == m_toolData->getSelectedView()) //Actualitzem la llesca
            {
                if (m_toolData->getSelectedUID() == frameOfReference)
                {
                    // Actualitzem per posició
                    const std::array<double, 3> &position = m_toolData->getPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel());
                    
                    int nearestSlice = m_sliceLocator->getNearestSlice(position.data());
                    if (nearestSlice != -1)
                    {
                        m_2DViewer->setSlice(nearestSlice);
                    }
                }
            }
        }
    }
}

}
