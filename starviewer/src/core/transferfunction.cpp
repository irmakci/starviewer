/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "transferfunction.h"
#include "logging.h"

#include <QVariant>

// vtk
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkLookupTable.h>

namespace udg {

TransferFunction::TransferFunction()
{
    m_changed = m_colorChanged = m_opacityChanged = true;
    m_colorTransferFunction = 0;
    m_opacityTransferFunction = 0;
}

TransferFunction::TransferFunction(const TransferFunction &transferFunction)
{
    m_name = transferFunction.name();
    m_color = transferFunction.m_color;
    m_opacity = transferFunction.m_opacity;

    m_changed = transferFunction.m_changed;
    if (!m_changed) m_definedX = transferFunction.m_definedX;

    m_colorTransferFunction = 0;
    m_opacityTransferFunction = 0;
    m_colorChanged = m_opacityChanged = true;
}

TransferFunction::TransferFunction(vtkLookupTable *lookupTable)
{
    m_colorTransferFunction = 0;
    m_opacityTransferFunction = 0;
    
    if (lookupTable)
    {
        // Transformem la vtkLookupTable al nostre format
        double range[2];
        lookupTable->GetTableRange(range);
        
        // Calculem quina és la diferència entre valor i valor de la taula
        double step = (range[1] - range[0]) / (double)lookupTable->GetNumberOfTableValues();
        
        // Recorrem la vtkLookupTable i inserim els valors al nostre format
        for (double value = range[0]; value < range[1]; value += step)
        {
            double rgb[3];
            double opacity;
            lookupTable->GetColor(value, rgb);
            opacity = lookupTable->GetOpacity(value);
            addPointToColorRGB(value, rgb[0], rgb[1], rgb[2]);
            addPointToOpacity(value, opacity);
        }
    }
    else
    {
        DEBUG_LOG("No es pot construir la transfer function a través d'un objecte nul de vtkLookupTable");
        m_changed = m_colorChanged = m_opacityChanged = true;
    }
}

TransferFunction::~TransferFunction()
{
    m_definedX.clear(); /// \todo en teoria no cal, però sense això peta (???!!!)
    if (m_colorTransferFunction) m_colorTransferFunction->Delete();
    if (m_opacityTransferFunction) m_opacityTransferFunction->Delete();
}

const QString & TransferFunction::name() const
{
    return m_name;
}

void TransferFunction::setName( const QString & name )
{
    m_name = name;
}

void TransferFunction::addPoint( double x, const QColor & rgba )
{
    m_color[x] = rgba;
    m_opacity[x] = rgba.alphaF();
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::addPoint( double x, const QColor & color, double opacity )
{
    m_color[x] = color;
    m_opacity[x] = opacity;
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::removePoint( double x )
{
    m_color.remove( x );
    m_opacity.remove( x );
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::addPointToColor( double x, const QColor & color )
{
    m_color[x] = color;
    m_changed = m_colorChanged = true;
}

void TransferFunction::addPointToColorRGB( double x, int r, int g, int b )
{
    m_color[x] = QColor( r, g, b );
    m_changed = m_colorChanged = true;
}

void TransferFunction::addPointToColorRGB( double x, double r, double g, double b )
{
    QColor color;
    color.setRgbF( r, g, b );
    m_color[x] = color;
    m_changed = m_colorChanged = true;
}

void TransferFunction::removePointFromColor( double x )
{
    m_color.remove( x );
    m_changed = m_colorChanged = true;
}

void TransferFunction::addPointToOpacity( double x, double opacity )
{
    m_opacity[x] = opacity;
    m_changed = m_opacityChanged = true;
}

void TransferFunction::removePointFromOpacity( double x )
{
    m_opacity.remove( x );
    m_changed = m_opacityChanged = true;
}

void TransferFunction::clear()
{
    m_color.clear();
    m_opacity.clear();
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::clearColor()
{
    m_color.clear();
    m_changed = m_colorChanged = true;
}

void TransferFunction::clearOpacity()
{
    m_opacity.clear();
    m_changed = m_opacityChanged = true;
}

QList<double>& TransferFunction::getPoints() const
{
    if (m_changed)
    {
        m_definedX = m_color.keys();
        m_definedX << m_opacity.keys();
        qSort(m_definedX);

        for (int i = 0; i < m_definedX.size() - 1; i++)
        {
            if (m_definedX.at(i) == m_definedX.at(i+1)) m_definedX.removeAt(i+1);
        }

        m_changed = false;
    }

    return m_definedX;
}

QList< double > TransferFunction::getColorPoints() const
{
    return m_color.keys();
}

QMap< double, QColor > TransferFunction::getColorMap() const
{
    return m_color;
}

QList< double > TransferFunction::getOpacityPoints() const
{
    return m_opacity.keys();
}

QMap< double, double > TransferFunction::getOpacityMap() const
{
    return m_opacity;
}

vtkColorTransferFunction * TransferFunction::getColorTransferFunction() const
{
    if ( m_colorChanged )
    {
        if ( !m_colorTransferFunction )
            m_colorTransferFunction = vtkColorTransferFunction::New();

        m_colorTransferFunction->RemoveAllPoints();

        QMapIterator< double, QColor > it( m_color );

        while ( it.hasNext() )
        {
            it.next();
            m_colorTransferFunction->AddRGBPoint( it.key(), it.value().redF(), it.value().greenF(), it.value().blueF() );
        }

        m_colorChanged = false;
    }

    return m_colorTransferFunction;
}

vtkPiecewiseFunction * TransferFunction::getOpacityTransferFunction() const
{
    if ( m_opacityChanged )
    {
        if ( !m_opacityTransferFunction )
            m_opacityTransferFunction = vtkPiecewiseFunction::New();

        m_opacityTransferFunction->RemoveAllPoints();

        QMapIterator< double, double > it( m_opacity );

        while ( it.hasNext() )
        {
            it.next();
            m_opacityTransferFunction->AddPoint( it.key(), it.value() );
        }

        m_opacityChanged = false;
    }

    return m_opacityTransferFunction;
}

bool TransferFunction::setNewRange( double min, double max )
{
    // Primer afegim els extrems. Si ja existeixen es quedarà igual.
    this->addPoint( min, this->get( min ) );
    this->addPoint( max, this->get( max ) );

    // Després eliminem els punts fora del rang.

    QMutableMapIterator<double, QColor> itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        if ( itc.key() < min ) this->removePointFromColor( itc.key() );
        else break;
    }
    itc.toBack();
    while ( itc.hasPrevious() )
    {
        itc.previous();
        if ( itc.key() > max ) this->removePointFromColor( itc.key() );
        else break;
    }

    QMutableMapIterator<double, double> ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        if ( ito.key() < min ) this->removePointFromOpacity( ito.key() );
        else break;
    }
    ito.toBack();
    while ( ito.hasPrevious() )
    {
        ito.previous();
        if ( ito.key() > max ) this->removePointFromOpacity( ito.key() );
        else break;
    }

    m_changed = m_colorChanged = m_opacityChanged = true;

	return true;
}

void TransferFunction::print() const
{
    DEBUG_LOG( "Color:" );
    QMapIterator< double, QColor > itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        DEBUG_LOG( qPrintable( QString( "    x = %1    rgb = (%2, %3, %4)" ).arg( itc.key() )
                  .arg( itc.value().redF() ).arg( itc.value().greenF() ).arg( itc.value().blueF() ) ) );
    }

    DEBUG_LOG( "Opacity:" );
    QMapIterator< double, double > ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        DEBUG_LOG( qPrintable( QString( "    x = %1    opacity = %2" ).arg( ito.key() ).arg( ito.value() ) ) );
    }
}

TransferFunction& TransferFunction::operator =(const TransferFunction &transferFunction)
{
    m_name = transferFunction.m_name;
    m_color = transferFunction.m_color;
    m_opacity = transferFunction.m_opacity;

    m_changed = transferFunction.m_changed;
    if (!m_changed) m_definedX = transferFunction.m_definedX;

    m_colorChanged = m_opacityChanged = true;

    return *this;
}

bool TransferFunction::operator ==( const TransferFunction & transferFunction ) const
{
    return m_name == transferFunction.m_name && m_color == transferFunction.m_color && m_opacity == transferFunction.m_opacity;
}

QVariant TransferFunction::toVariant() const
{
    QMap<QString, QVariant> color;
    QMapIterator<double, QColor> itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        color[QString::number( itc.key() )] = itc.value().rgba();
    }

    QMap<QString, QVariant> opacity;
    QMapIterator<double, double> ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        opacity[QString::number( ito.key() )] = ito.value();
    }

    QMap<QString, QVariant> map;
    map["name"] = m_name;
    map["color"] = color;
    map["opacity"] = opacity;

    return map;
}

TransferFunction TransferFunction::fromVariant( const QVariant &variant )
{
    TransferFunction transferFunction;
    QMap<QString, QVariant> map = variant.toMap();

    transferFunction.setName( map["name"].toString() );

    QMap<QString, QVariant> color = map["color"].toMap();
    QMapIterator<QString, QVariant> itc( color );
    while ( itc.hasNext() )
    {
        itc.next();
        transferFunction.addPointToColor( itc.key().toDouble(), itc.value().toUInt() );
    }

    QMap<QString, QVariant> opacity = map["opacity"].toMap();
    QMapIterator<QString, QVariant> ito( opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        transferFunction.addPointToOpacity( ito.key().toDouble(), ito.value().toDouble() );
    }

    return transferFunction;
}

TransferFunction TransferFunction::to01( double minimum, double maximum ) const
{
    double shift = -minimum, scale = 1.0 / ( maximum - minimum );
    TransferFunction transferFunction01;
    QList<double> points = this->getPoints();

    foreach ( double x, points ) transferFunction01.addPoint( ( x + shift ) * scale, this->get( x ) );

    return transferFunction01;
}


QList<double> TransferFunction::getPointsInInterval(double begin, double end) const
{
    getPoints();    // per actualitzar m_definedX

    QList<double>::const_iterator lowerBound = qLowerBound(m_definedX, begin);
    QList<double>::const_iterator itEnd = m_definedX.constEnd();
    QList<double> pointsInInterval;

    while (lowerBound != itEnd && *lowerBound <= end) pointsInInterval << *(lowerBound++);

    return pointsInInterval;
}


QList<double> TransferFunction::getPointsNear( double x, double distance ) const
{
    return getPointsInInterval( x - distance, x + distance );
}


TransferFunction TransferFunction::normalize() const
{
    getPoints();    // per actualitzar m_definedX

    TransferFunction normalized;
    QListIterator<double> it(m_definedX);

    while (it.hasNext())
    {
        double x = it.next();
        normalized.addPoint(x, getColor(x), getOpacity(x));
    }

    normalized.m_name = m_name;

    return normalized;
}


TransferFunction TransferFunction::simplify() const
{
    TransferFunction simplified = *this;

    QMapIterator<double, QColor> itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        TransferFunction tentative = simplified;
        tentative.removePointFromColor( itc.key() );
        if ( tentative.getColor( itc.key() ) == simplified.getColor( itc.key() ) ) simplified = tentative;
    }

    QMapIterator<double, double> ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        TransferFunction tentative = simplified;
        tentative.removePointFromOpacity( ito.key() );
        if ( tentative.getOpacity( ito.key() ) == simplified.getOpacity( ito.key() ) ) simplified = tentative;
    }

    return simplified;
}


}
