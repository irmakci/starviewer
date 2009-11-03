/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomitem.h"

#include "dicomattribute.h"

namespace udg {

DICOMItem::DICOMItem( )
{
}

DICOMItem::~DICOMItem()
{

}

void DICOMItem::addAttribute(DICOMAttribute * attribute )
{
    m_attributeList.append( attribute );
}

}
