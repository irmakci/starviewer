/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�fics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmprextension.h"
#include "volume.h"
#include "q2dviewer.h"
#include "mathtools.h" // per c�lculs d'interseccions
// qt
#include <qspinbox.h> // pel control m_axialSpinBox
#include <qslider.h> // pel control m_axialSlider
#include <qsettings.h>
#include <qtextstream.h>
#include <qsplitter.h>
#include <qpushbutton.h>
// vtk
#include <vtkRenderer.h>
#include <vtkMath.h> // pel vtkMath::Cross
#include <vtkActor.h>
#include <vtkAxisActor2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkCellPicker.h>
#include <vtkPlaneSource.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkCamera.h>

namespace udg {

QMPRExtension::QMPRExtension(QWidget *parent, const char *name)
 : QMPRExtensionBase(parent, name)
{
        
    m_axialPlaneSource = vtkPlaneSource::New();
    m_axialPlaneSource->SetXResolution( 1 ); // aix� estan configurats a vtkImagePlaneWidget
    m_axialPlaneSource->SetYResolution( 1 );
    
    m_sagitalPlaneSource = vtkPlaneSource::New();
    m_sagitalPlaneSource->SetXResolution( 1 ); 
    m_sagitalPlaneSource->SetYResolution( 1 );
    
    m_coronalPlaneSource = vtkPlaneSource::New();
    m_coronalPlaneSource->SetXResolution( 1 );
    m_coronalPlaneSource->SetYResolution( 1 );

    m_sagitalReslice = vtkImageReslice::New();
    m_sagitalReslice->AutoCropOutputOn(); // perqu� l'extent d'output sigui suficient i no es "mengi" dades
    m_sagitalReslice->SetInterpolationModeToCubic();
    
    m_coronalReslice = vtkImageReslice::New();
    m_coronalReslice->AutoCropOutputOn();
    m_coronalReslice->SetInterpolationModeToLinear();

    /// per defecte isom�tric
    m_axialSpacing[0] = 1.;
    m_axialSpacing[1] = 1.;
    m_axialSpacing[2] = 1.;
    
    //\TODO haur�em de fer un transform per cada pla/reslice
    m_transform = vtkTransform::New();
        
    
    createConnections();
    createActors();
    
    readSettings();
    
}

void QMPRExtension::setInput( Volume *input )
{ 
    m_volume = input; 

    m_volume->updateInformation();
    m_volume->getSpacing( m_axialSpacing );
    
    m_sagitalReslice->SetInput( m_volume->getVtkData() );
    m_coronalReslice->SetInput( m_volume->getVtkData() );
    
    // faltaria refrescar l'input dels 3 mpr     
    m_axial2DView->setInput( m_volume );
              
    // Totes les vistes tindran com a refer�ncia el sistema de coordenades Axial, base de tots els reslice que aplicarem. 
    m_axial2DView->setViewToAxial();
    vtkCamera *axialCam = m_axial2DView->getRenderer() ? m_axial2DView->getRenderer()->GetActiveCamera() : NULL;
    if ( axialCam )
    {
        axialCam->SetViewUp(0,-1,0);
    }
    m_coronal2DView->setViewToAxial();
    m_sagital2DView->setViewToAxial();
    
    // refrescar el controls
    m_axialSpinBox->setMinValue( 0 );
    m_axialSpinBox->setMaxValue( m_volume->getVtkData()->GetDimensions()[2] );    
    m_axialSlider->setMaxValue(  m_volume->getVtkData()->GetDimensions()[2] );
    m_axialSlider->setValue( m_axial2DView->getSlice() );    
// Ja no hi s�n , ho deixem comentat temporalment per si interess�s re-incorporar-los, res m�s 
//     m_sagitalSpinBox->setMinValue( 0 );
//     m_sagitalSpinBox->setMaxValue( m_volume->getVtkData()->GetDimensions()[1] );    
//     m_sagitalSlider->setMaxValue(  m_volume->getVtkData()->GetDimensions()[1] );
//     m_sagitalSlider->setValue( m_sagital2DView->getSlice() );
//     
//     m_coronalSpinBox->setMinValue( 0 );
//     m_coronalSpinBox->setMaxValue( m_volume->getVtkData()->GetDimensions()[0] );    
//     m_coronalSlider->setMaxValue(  m_volume->getVtkData()->GetDimensions()[0] );
//     m_coronalSlider->setValue( m_coronal2DView->getSlice() );

    // posta a punt dels planeSource
    initOrientation();
    
    m_axial2DView->render();
//     m_axial2DView->displayInformationText( false );

    Volume *sagitalResliced = new Volume;
    sagitalResliced->setData( m_sagitalReslice->GetOutput() );
    m_sagital2DView->setInput( sagitalResliced );

    Volume *coronalResliced = new Volume;
    coronalResliced->setData( m_coronalReslice->GetOutput() );
    m_coronal2DView->setInput( coronalResliced );
    
    m_sagital2DView->render();
//     m_sagital2DView->displayInformationText( false );
    m_coronal2DView->render();
//     m_coronal2DView->displayInformationText( false );    
        
    updateControls();
    
}

void QMPRExtension::initOrientation()
{
/**
    IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    ara li donem a cada pla unes dimensions, extent , espaiat, etc d'acord com si aquests plans haguessin de ser ortogonals,per� segons el pla de tall ser� d'una manera o altre
    La vista axial mantindr� els espaiats i extents originals
    La vista sagital, com que �nicament es podr� rotar sobre l'eix Y, mantindr� l'extent de la seva X igual que l'extent Y original. Els altres s'hauran d'adaptar a les dist�ncies corresponents a les diagonals zmax-xmax
    En la vista coronal, com que pot tenir qualsevol orientacio tindr� que adaptar els seus extents als m�xims
    
*/

    // This method must be called _after_ setInput
    //
    m_volume->setData( vtkImageData::SafeDownCast( m_coronalReslice->GetInput()) );
    if ( ! m_volume->getVtkData() )
    {
        std::cerr << "SetInput() before setting plane orientation." << std::endl;
        return;
    }

    m_volume->updateInformation();
    int extent[6];
    m_volume->getWholeExtent(extent);
    double origin[3];
    m_volume->getOrigin(origin);
    double spacing[3];
    m_volume->getSpacing(spacing);
    
    // Prevent obscuring voxels by offsetting the plane geometry
    //
    double xbounds[] = {origin[0] + spacing[0] * (extent[0] - 0.5),
                        origin[0] + spacing[0] * (extent[1] + 0.5)};
    double ybounds[] = {origin[1] + spacing[1] * (extent[2] - 0.5),
                        origin[1] + spacing[1] * (extent[3] + 0.5)};
    double zbounds[] = {origin[2] + spacing[2] * (extent[4] - 0.5),
                        origin[2] + spacing[2] * (extent[5] + 0.5)};
    
    if ( spacing[0] < 0.0 )
    {
        double t = xbounds[0];
        xbounds[0] = xbounds[1];
        xbounds[1] = t;
    }
    if ( spacing[1] < 0.0 )
    {
        double t = ybounds[0];
        ybounds[0] = ybounds[1];
        ybounds[1] = t;
    }
    if ( spacing[2] < 0.0 )
    {
        double t = zbounds[0];
        zbounds[0] = zbounds[1];
        zbounds[1] = t;
    }
    //XY, z-normal : vista axial , en principi d'aquesta vista nom�s canviarem la llesca
    m_axialPlaneSource->SetOrigin( xbounds[0] , ybounds[0] , zbounds[0] );
    m_axialPlaneSource->SetPoint1( xbounds[1] , ybounds[0] , zbounds[0] );
    m_axialPlaneSource->SetPoint2( xbounds[0] , ybounds[1] , zbounds[0] );
    // posem en la llesca central    \TODO: sincronitzar b� aquestes llesques amb les del visor
    m_sagitalPlaneSource->Push( 0.5 * ( zbounds[1] - zbounds[0] ) );
    
    //YZ, x-normal : vista sagital
    // estem ajustant la mida del pla a les dimensions d'aquesta orientaci�
    // \TODO podr�em donar unes mides a cada punt que fossin suficientment grans com per poder mostrejar qualssevol orientaci� en el volum, potser fent una bounding box o simplement d'una forma m�s "bruta" doblant la longitud d'aquest pla :P

    double maxYBound = sqrt( ybounds[1]*ybounds[1] + xbounds[1]*xbounds[1] );
    double diffYBound = maxYBound - ybounds[1];
// \TODO maxYBound �s correcte, el problema que hi ha �s que inicialment est� ben distribuit (0.5 amunt i avall ) perqu� tenim les llesques centrades, per� al canviar de llesca haur�em de tenir en compte que aquestes difer�ncies s'han de canviar i equilibrar segons la llesca en que ens trobem
    m_sagitalPlaneSource->SetOrigin( xbounds[0] , ybounds[0]/* - diffYBound*0.5*/ , zbounds[0]  );
    m_sagitalPlaneSource->SetPoint1( xbounds[0] , ybounds[1] /*maxYBound - diffYBound*0.5*/ , zbounds[0] );
    m_sagitalPlaneSource->SetPoint2( xbounds[0] , ybounds[0]/* - diffYBound*0.5*/ , zbounds[1] );
        
    // \TODO aqui caldria canviar-li els bounds del point 2 perqu� siguin m�s llargs i encaixi amb la diagonal
    // perqu� quedi centrat hauriem de despla�ar la meitat de l'espai extra per l'origen i pel punt2
    // posem en la llesca central    
    m_sagitalPlaneSource->Push( 0.5 * ( xbounds[1] - xbounds[0] ) );
    
    //ZX, y-normal : vista coronal
    // �dem anterior

    m_coronalPlaneSource->SetOrigin( xbounds[0] /*-diffXBound*0.5*/, ybounds[0] , zbounds[0] /*-diffZBound*0.5*/ );
    m_coronalPlaneSource->SetPoint1( xbounds[1] /*maxZBound*/ , ybounds[0] , zbounds[0]);
    m_coronalPlaneSource->SetPoint2( xbounds[0] , ybounds[0] , zbounds[1] /*maxZBound*/ );
    // posem en la llesca central    
    m_coronalPlaneSource->Push( - 0.5 * ( ybounds[1] - ybounds[0] ) );
    
    updatePlanes();

}

void QMPRExtension::createConnections()
{
    connect( m_axial2DView , SIGNAL( sliceChanged(int) ) , this , SLOT( axialSliceUpdated(int) ) );
    
    // temporal
    
    connect( m_rotateXPlus , SIGNAL( clicked() ), this , SLOT( rotateXPlus() ) );
    connect( m_rotateXMinus , SIGNAL( clicked() ), this , SLOT( rotateXMinus() ) );
    
    connect( m_rotateYPlus , SIGNAL( clicked() ), this , SLOT( rotateYPlus() ) );
    connect( m_rotateYMinus , SIGNAL( clicked() ), this , SLOT( rotateYMinus() ) );
    
    connect( m_sagitalYPlus , SIGNAL( clicked() ), this , SLOT( rotateSagitalYPlus() ) );
    connect( m_sagitalYMinus , SIGNAL( clicked() ), this , SLOT( rotateSagitalYMinus() ) );
    
    connect( m_sagSlicePlus , SIGNAL( clicked() ), this , SLOT( sagitalSlicePlus() ) );
    connect( m_sagSliceMinus , SIGNAL( clicked() ), this , SLOT( sagitalSliceMinus() ) );
    
    connect( m_corSlicePlus , SIGNAL( clicked() ), this , SLOT( coronalSlicePlus() ) );
    connect( m_corSliceMinus , SIGNAL( clicked() ), this , SLOT( coronalSliceMinus() ) );
    
    // fi temporal    
}

void QMPRExtension::createActors()
{
    /*
    // creem el quadradet que controla les llesques 
    vtkPoints *polyPoints = vtkPoints::New();
    polyPoints->SetNumberOfPoints( 4 );
    polyPoints->InsertPoint( 0 , 0 , 0 , 0 );
    polyPoints->InsertPoint( 1 , 10 , 0 , 0 );
    polyPoints->InsertPoint( 2 , 10 , 10 , 0 );
    polyPoints->InsertPoint( 3 , 0 , 10 , 0 );

    vtkCellArray *lines = vtkCellArray::New();
    lines->InsertNextCell(5);
    lines->InsertCellPoint(0);
    lines->InsertCellPoint(1);
    lines->InsertCellPoint(2);
    lines->InsertCellPoint(3);
    lines->InsertCellPoint(0);   
     
    vtkPolyData *quad = vtkPolyData::New();
    quad->SetPoints( polyPoints );
    quad->SetLines( lines );
    
    vtkPolyDataMapper2D *polyMapper = vtkPolyDataMapper2D::New();
    polyMapper->SetInput( quad );
    
    m_axialSliceActor = vtkActor2D::New();
    m_axialSliceActor->SetMapper( polyMapper );
    m_axialSliceActor->GetProperty()->SetColor( 1 , 1 , 0 );
    
    m_coronalSagitalSliceActor = vtkActor2D::New();
    m_coronalSagitalSliceActor->SetMapper( polyMapper );
    m_coronalSagitalSliceActor->GetProperty()->SetColor( 1 , 1 , 0 );
    */
    
    // creem els axis actors 
    m_sagitalOverAxialAxisActor = vtkAxisActor2D::New(); 
    m_coronalOverAxialIntersectionAxis = vtkAxisActor2D::New(); 
    m_coronalOverSagitalIntersectionAxis = vtkAxisActor2D::New(); 
    m_axialOverSagitalIntersectionAxis = vtkAxisActor2D::New();  
    
    m_sagitalOverAxialAxisActor->AxisVisibilityOn(); 
    m_sagitalOverAxialAxisActor->TickVisibilityOff();
    m_sagitalOverAxialAxisActor->LabelVisibilityOff();
    m_sagitalOverAxialAxisActor->TitleVisibilityOff();
    m_sagitalOverAxialAxisActor->GetProperty()->SetColor( 1 , 0 , 0 );
    
    m_coronalOverAxialIntersectionAxis->TickVisibilityOff();
    m_coronalOverAxialIntersectionAxis->LabelVisibilityOff();
    m_coronalOverAxialIntersectionAxis->TitleVisibilityOff();
    m_coronalOverAxialIntersectionAxis->GetProperty()->SetColor( 0 , 0 , 1 );
    
    m_coronalOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_coronalOverSagitalIntersectionAxis->TickVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->TitleVisibilityOff();    
    m_coronalOverSagitalIntersectionAxis->GetProperty()->SetColor( 0 , 0 , 1 );
    
    m_axialOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_axialOverSagitalIntersectionAxis->TickVisibilityOff();
    m_axialOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_axialOverSagitalIntersectionAxis->TitleVisibilityOff();    
    m_axialOverSagitalIntersectionAxis->GetProperty()->SetColor( 1 , 1 , 0 );
    
    m_axial2DView->getRenderer()->AddActor2D( m_sagitalOverAxialAxisActor );
    m_axial2DView->getRenderer()->AddActor2D( m_coronalOverAxialIntersectionAxis );
    m_sagital2DView->getRenderer()->AddActor2D( m_coronalOverSagitalIntersectionAxis );
    m_sagital2DView->getRenderer()->AddActor2D( m_axialOverSagitalIntersectionAxis );

}

void QMPRExtension::axialSliceUpdated( int slice )
{
    // nou push - anterior push = push relatiu que hem de fer
    m_axialPlaneSource->Push( ( slice * m_axialSpacing[2] ) - m_axialPlaneSource->GetOrigin()[2] );
    m_axialPlaneSource->Update();
    updateIntersectionPoint();
    updateControls();
}

void QMPRExtension::sagitalSliceUpdated( int slice )
{

}

void QMPRExtension::coronalSliceUpdated( int slice )
{
}

void QMPRExtension::rotateXPlus()
{
    updateIntersectionPoint();
    
    double axis[3];
    getCoronalYVector( axis );
    rotateMiddle( 5 , axis , m_coronalPlaneSource , m_coronalReslice );
   
    updatePlanes();
    updateControls();
    
}

void QMPRExtension::rotateXMinus()
{
    updateIntersectionPoint();
        
    double axis[3];
    getCoronalYVector( axis );
    rotateMiddle( -5 , axis , m_coronalPlaneSource , m_coronalReslice );
    

    updatePlanes();
    updateControls();
}

void QMPRExtension::rotateSagitalYPlus()
{
    updateIntersectionPoint();

    double axis[3];
    getSagitalYVector( axis );
    rotateMiddle( 5 , axis , m_sagitalPlaneSource , m_sagitalReslice );    
   
    updatePlanes();
    updateControls();
    
}

void QMPRExtension::rotateSagitalYMinus()
{
    updateIntersectionPoint();
    
    double axis[3];
    getSagitalYVector( axis );
    rotateMiddle( -5 , axis , m_sagitalPlaneSource , m_sagitalReslice );

    updatePlanes();
    updateControls();
}

void QMPRExtension::rotateYPlus()
{   

    updateIntersectionPoint();    
     
    double axis[3];
    getCoronalXVector( axis );
    rotateMiddle( 5 , axis , m_coronalPlaneSource , m_coronalReslice );
       
    updatePlanes();
    updateControls();
}

void QMPRExtension::rotateYMinus()
{

    updateIntersectionPoint();
    
    double axis[3];
    getCoronalXVector( axis );
    rotateMiddle( -5 , axis , m_coronalPlaneSource , m_coronalReslice );
    
    updatePlanes();
    updateControls();
}

void QMPRExtension::sagitalSlicePlus()
{
    m_sagitalPlaneSource->Push( 10 );
    m_sagitalPlaneSource->Update();
    updatePlanes();
    updateControls();
}

void QMPRExtension::sagitalSliceMinus()
{
    m_sagitalPlaneSource->Push( -10 );
    m_sagitalPlaneSource->Update();
    updatePlanes();
    updateControls();
}

void QMPRExtension::coronalSlicePlus()
{
    m_coronalPlaneSource->Push( 10 );
    m_coronalPlaneSource->Update();
    updatePlanes();
    updateControls();
}

void QMPRExtension::coronalSliceMinus()
{
    m_coronalPlaneSource->Push( -10 );
    m_coronalPlaneSource->Update();
    updatePlanes();
    updateControls();
}
    
QMPRExtension::~QMPRExtension()
{
    writeSettings();
    
    m_sagitalOverAxialAxisActor->Delete();
    m_axialOverSagitalIntersectionAxis->Delete();
    m_coronalOverAxialIntersectionAxis->Delete();
    m_coronalOverSagitalIntersectionAxis->Delete();
    

}

void QMPRExtension::updateControls()
{

// posem la representaci� dels plans sobre cada vista
    updateIntersectionPoint();
//     
// 
//     PLA SAGITAL : L�nia vermella. Aquest sempre ser� perpendicular al pla axial, per tant �s el m�s f�cil 
// de pintar
// 
// 
// 
    // Passem a sistema de coordenades de m�n
    m_sagitalOverAxialAxisActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_sagitalOverAxialAxisActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    
    m_coronalOverSagitalIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_coronalOverSagitalIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    
    m_coronalOverAxialIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_coronalOverAxialIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    
    m_axialOverSagitalIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_axialOverSagitalIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    
    double r[3] , t[3] , position1[3] , position2[3];
    
    // projecci� sagital sobre axial i viceversa
    MathTools::planeIntersection( m_axialPlaneSource->GetOrigin() , m_axialPlaneSource->GetNormal() ,  m_sagitalPlaneSource->GetOrigin() , m_sagitalPlaneSource->GetNormal() , r , t );
    
    position1[0] = r[0] - t[0]*2000;
    position1[1] = r[1] - t[1]*2000;
    position1[2] = r[2] - t[2]*2000;
    
    position2[0] = r[0] + t[0]*2000;
    position2[1] = r[1] + t[1]*2000;
    position2[2] = r[2] + t[2]*2000;
    
    m_sagitalOverAxialAxisActor->SetPosition(  position1[0] , position1[1] );
    m_sagitalOverAxialAxisActor->SetPosition2( position2[0] , position2[1] );
    
    m_axialOverSagitalIntersectionAxis->SetPosition(  position1[1] , position1[2] );
    m_axialOverSagitalIntersectionAxis->SetPosition2( position2[1] , position2[2] );
    
    // projecci� coronal sobre sagital
    
    
    MathTools::planeIntersection( m_coronalPlaneSource->GetOrigin() , m_coronalPlaneSource->GetNormal() , m_sagitalPlaneSource->GetOrigin() , m_sagitalPlaneSource->GetNormal() , r , t );
    
    position1[0] = r[0] - t[0]*2000;
    position1[1] = r[1] - t[1]*2000;
    position1[2] = r[2] - t[2]*2000;
    
    position2[0] = r[0] + t[0]*2000;
    position2[1] = r[1] + t[1]*2000;
    position2[2] = r[2] + t[2]*2000;
    
    m_coronalOverSagitalIntersectionAxis->SetPosition( position1[1] , position1[2] );
    m_coronalOverSagitalIntersectionAxis->SetPosition2( position2[1] , position2[2] );
    
    
    // projecci� coronal sobre axial
    MathTools::planeIntersection( m_coronalPlaneSource->GetOrigin() , m_coronalPlaneSource->GetNormal() , m_axialPlaneSource->GetOrigin() , m_axialPlaneSource->GetNormal() , r , t );
    
    position1[0] = r[0] - t[0]*2000;
    position1[1] = r[1] - t[1]*2000;
    position1[2] = r[2] - t[2]*2000;
    
    position2[0] = r[0] + t[0]*2000;
    position2[1] = r[1] + t[1]*2000;
    position2[2] = r[2] + t[2]*2000;
    
    m_coronalOverAxialIntersectionAxis->SetPosition(  position1[0] , position1[1] );
    m_coronalOverAxialIntersectionAxis->SetPosition2( position2[0] , position2[1] );  

// 
// 
//     Indicadors d'slices
// 
// 
    
// \TODO
//     Cal retocar que no es descentrin

/*    m_axialSliceActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_axialSliceActor->SetPosition( m_intersectionPoint[1]-5 , m_intersectionPoint[2]-5 );*/
    
//     m_coronalSagitalSliceActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
//     m_coronalSagitalSliceActor->SetPosition( m_intersectionPoint[0]-5 , m_intersectionPoint[1]-5 );

    
//     Repintem l'escena

    m_axial2DView->getInteractor()->Render();
    m_sagital2DView->getInteractor()->Render();
    m_coronal2DView->getInteractor()->Render();
}

void QMPRExtension::reset()
{

}

void QMPRExtension::setAxialSlice( int slice )
{
    m_axial2DView->setSlice( slice );
    //\TODO update dels eixos
    
}

void QMPRExtension::setCoronalSlice( int slice )
{
    m_coronal2DView->setSlice( slice );
    //\TODO update dels eixos
}

void QMPRExtension::setSagitalSlice( int slice )
{
    m_sagital2DView->setSlice( slice );
    //\TODO update dels eixos
}

void QMPRExtension::setSlice( int slice , int view )
{
    switch( view )
    {
    case 0:
        //Axial
        setAxialSlice( slice );
    break;
    
    case 1:
        // Coronal
        setCoronalSlice( slice );
    break;
    
    case 2:
        // Sagital
        setSagitalSlice( slice );
    break;
    
    default:
        // Axial
        setAxialSlice( slice );
    break;
    }
}

void QMPRExtension::updateIntersectionPoint()
{
    MathTools::planeIntersection(  m_coronalPlaneSource->GetOrigin() , m_coronalPlaneSource->GetNormal() , m_sagitalPlaneSource->GetOrigin() , m_sagitalPlaneSource->GetNormal() , m_axialPlaneSource->GetOrigin() , m_axialPlaneSource->GetNormal() ,  m_intersectionPoint );   
}

void QMPRExtension::updatePlanes()
{
    Volume *resliced = new Volume;
    updatePlane( m_sagitalPlaneSource , m_sagitalReslice );
    updatePlane( m_coronalPlaneSource , m_coronalReslice );    
    updateIntersectionPoint();
}

void QMPRExtension::updatePlane( vtkPlaneSource *planeSource , vtkImageReslice *reslice )
{
    if ( !reslice || !( vtkImageData::SafeDownCast( reslice->GetInput() ) )  )
    {
        return;
    }
    
    // Calculate appropriate pixel spacing for the reslicing
    //
    m_volume->updateInformation();
    double spacing[3];
    m_volume->getSpacing( spacing );
    
    int i;
    /*
    if ( this->RestrictPlaneToVolume )
    {
    *//*
        double origin[3];
        m_volume->getOrigin( origin );
        int extent[6];
        m_volume->getWholeExtent(extent);
        double bounds[] = { origin[0] + spacing[0]*extent[0], //xmin
                        origin[0] + spacing[0]*extent[1], //xmax
                        origin[1] + spacing[1]*extent[2], //ymin
                        origin[1] + spacing[1]*extent[3], //ymax
                        origin[2] + spacing[2]*extent[4], //zmin
                        origin[2] + spacing[2]*extent[5] };//zmax
    
        for ( i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
        {
            if ( bounds[i] > bounds[i+1] )
            {
                double t = bounds[i+1];
                bounds[i+1] = bounds[i];
                bounds[i] = t;
            }
        }
    
        double abs_normal[3];
        planeSource->GetNormal(abs_normal);
        double planeCenter[3];
        planeSource->GetCenter(planeCenter);
        double nmax = 0.0;
        int k = 0;
        for ( i = 0; i < 3; i++ )
        {
            abs_normal[i] = fabs(abs_normal[i]);
            if ( abs_normal[i]>nmax )
            {
                nmax = abs_normal[i];
                k = i;
            }
        }
    // Force the plane to lie within the true image bounds along its normal
    //
        if ( planeCenter[k] > bounds[2*k+1] )
        {
            planeCenter[k] = bounds[2*k+1];
        }
        else if ( planeCenter[k] < bounds[2*k] )
        {
            planeCenter[k] = bounds[2*k];
        }
    
        planeSource->SetCenter(planeCenter);*/
//     }
    
    double planeAxis1[3];
    double planeAxis2[3];
    // obtenim els vectors
    planeAxis1[0] = planeSource->GetPoint1()[0] - planeSource->GetOrigin()[0];
    planeAxis1[1] = planeSource->GetPoint1()[1] - planeSource->GetOrigin()[1];
    planeAxis1[2] = planeSource->GetPoint1()[2] - planeSource->GetOrigin()[2];
    
    planeAxis2[0] = planeSource->GetPoint2()[0] - planeSource->GetOrigin()[0];
    planeAxis2[1] = planeSource->GetPoint2()[1] - planeSource->GetOrigin()[1];
    planeAxis2[2] = planeSource->GetPoint2()[2] - planeSource->GetOrigin()[2];
        
    // The x,y dimensions of the plane
    //
    double planeSizeX = vtkMath::Normalize( planeAxis1 );
    double planeSizeY = vtkMath::Normalize( planeAxis2 );
    
    double normal[3];
    planeSource->GetNormal( normal );
    
    // Generate the slicing matrix
    //
    // Podria ser membre de classe, com era originalriament o passar per par�metre
    vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();
    resliceAxes->Identity();
    for ( i = 0; i < 3; i++ )
    {
        resliceAxes->SetElement(0,i,planeAxis1[i]);
        resliceAxes->SetElement(1,i,planeAxis2[i]);
        resliceAxes->SetElement(2,i,normal[i]);
    }
    
    double planeOrigin[4];
    planeSource->GetOrigin(planeOrigin);
    planeOrigin[3] = 1.0;
    double originXYZW[4];
    resliceAxes->MultiplyPoint(planeOrigin,originXYZW);
    
    resliceAxes->Transpose();
    double neworiginXYZW[4];
    double point[] =  {originXYZW[0],originXYZW[1],originXYZW[2],originXYZW[3]};
    resliceAxes->MultiplyPoint(point,neworiginXYZW);
    
    resliceAxes->SetElement(0,3,neworiginXYZW[0]);
    resliceAxes->SetElement(1,3,neworiginXYZW[1]);
    resliceAxes->SetElement(2,3,neworiginXYZW[2]);
    
    reslice->SetResliceAxes( resliceAxes );
    
    double spacingX = fabs(planeAxis1[0]*spacing[0])+\
                    fabs(planeAxis1[1]*spacing[1])+\
                    fabs(planeAxis1[2]*spacing[2]);
    
    double spacingY = fabs(planeAxis2[0]*spacing[0])+\
                    fabs(planeAxis2[1]*spacing[1])+\
                    fabs(planeAxis2[2]*spacing[2]);
    
    
    // Pad extent up to a power of two for efficient texture mapping
    
    // make sure we're working with valid values
    double realExtentX = ( spacingX == 0 ) ? 0 : planeSizeX / spacingX;
    
    int extentX;
    // Sanity check the input data:
    // * if realExtentX is too large, extentX will wrap
    // * if spacingX is 0, things will blow up.
    // * if realExtentX is naturally 0 or < 0, the padding will yield an
    //   extentX of 1, which is also not desirable if the input data is invalid.
    if (realExtentX > (VTK_INT_MAX >> 1) || realExtentX < 1)
    {
//         vtkErrorMacro(<<"Invalid X extent.  Perhaps the input data is empty?");
        std::cerr << "Invalid X extent. [" << realExtentX << "] Perhaps the input data is empty?" << std::endl;
        extentX = 0;
    }
    else
    {
        extentX = 1;
        while (extentX < realExtentX)
        {
            extentX = extentX << 1;
        }
    }
    
    // make sure extentY doesn't wrap during padding
    double realExtentY = ( spacingY == 0 ) ? 0 : planeSizeY / spacingY;
    
    int extentY;
    if (realExtentY > (VTK_INT_MAX >> 1) || realExtentY < 1)
    {
        std::cerr << "Invalid Y extent. [" << realExtentY << "] Perhaps the input data is empty?" << std::endl;    
        extentY = 0;
    }
    else
    {
        extentY = 1;
        while (extentY < realExtentY)
        {
            extentY = extentY << 1;
        }
    }
    
    reslice->SetOutputSpacing( planeSizeX/extentX , planeSizeY/extentY , 1 );
    reslice->SetOutputOrigin( 0.0 , 0.0 , 0.0 );
    // �?caldria posar-li correctament l'extent ! �s possiblement pe aix� que ens d�na la excepci� aquella
    reslice->SetOutputExtent( 0 , extentX-1 , 0 , extentY-1 , 0 , 0 ); // obtenim una �nica llesca
    reslice->Update();
    
}

void QMPRExtension::getSagitalXVector( double x[3] )
{
    double* p1 = m_sagitalPlaneSource->GetPoint1();
    double* o =  m_sagitalPlaneSource->GetOrigin();
    x[0] = p1[0] - o[0];
    x[1] = p1[1] - o[1];
    x[2] = p1[2] - o[2];
}

void QMPRExtension::getSagitalYVector( double y[3] )
{
    double* p1 = m_sagitalPlaneSource->GetPoint2();
    double* o =  m_sagitalPlaneSource->GetOrigin();
    y[0] = p1[0] - o[0];
    y[1] = p1[1] - o[1];
    y[2] = p1[2] - o[2];
}

void QMPRExtension::getCoronalXVector( double x[3] )
{
    double* p1 = m_coronalPlaneSource->GetPoint1();
    double* o =  m_coronalPlaneSource->GetOrigin();
    x[0] = p1[0] - o[0];
    x[1] = p1[1] - o[1];
    x[2] = p1[2] - o[2];
}

void QMPRExtension::getAxialXVector( double x[3] )
{
    double* p1 = m_axialPlaneSource->GetPoint1();
    double* o =  m_axialPlaneSource->GetOrigin();
    x[0] = p1[0] - o[0];
    x[1] = p1[1] - o[1];
    x[2] = p1[2] - o[2];
}

void QMPRExtension::getAxialYVector( double y[3] )
{
    double* p2 = m_axialPlaneSource->GetPoint2();
    double* o =  m_axialPlaneSource->GetOrigin();
    y[0] = p2[0] - o[0];
    y[1] = p2[1] - o[1];
    y[2] = p2[2] - o[2];
}
void QMPRExtension::getCoronalYVector( double y[3] )
{
    double* p1 = m_coronalPlaneSource->GetPoint2();
    double* o =  m_coronalPlaneSource->GetOrigin();
    y[0] = p1[0] - o[0];
    y[1] = p1[1] - o[1];
    y[2] = p1[2] - o[2];
}

double QMPRExtension::angleInRadians( double vec1[3] , double vec2[3] )
{
    return acos( vtkMath::Dot( vec1,vec2 ) / ( vtkMath::Norm(vec1)*vtkMath::Norm(vec2) ) );
}

double QMPRExtension::angleInDegrees( double vec1[3] , double vec2[3] )
{
    return angleInRadians( vec1 , vec2 ) * vtkMath::DoubleRadiansToDegrees();
}

// to be deprecated
double QMPRExtension::getDegrees( double p1[3] , double p2[3], double origin[3] )
{
    // definim els vectors de les rectes
    double v1[3] , v2[3];
    v1[0] = p1[0] - origin[0];
    v1[1] = p1[1] - origin[1];
    v1[2] = p1[2] - origin[2];
    
    v2[0] = p2[0] - origin[0];
    v2[1] = p2[1] - origin[1];
    v2[2] = p2[2] - origin[2];
    
//     return acos( vtkMath::Dot( v1,v2 ) / ( vtkMath::Norm(v1)*vtkMath::Norm(v2) ) );
    return angleInDegrees( v1,v2 );
}
bool QMPRExtension::isParallel( double axis[3] )
{
    double xyzAxis[3] = {1,0,0};
    if( angleInDegrees( xyzAxis , axis ) == 0.0 )
    {
        return true;
    }
    xyzAxis[0] = -1;
    xyzAxis[1] = 0;
    xyzAxis[2] = 0;
    if( angleInDegrees( xyzAxis , axis ) == 0.0 )
    {
        return true;
    }
    
    xyzAxis[0] = 0;
    xyzAxis[1] = 0;
    xyzAxis[2] = 1;
    if( angleInDegrees( xyzAxis , axis ) == 0.0 )
    {
        return true;
    }
    xyzAxis[0] = 0;
    xyzAxis[1] = 0;
    xyzAxis[2] = -1;
    if( angleInDegrees( xyzAxis , axis ) == 0.0 )
    {
        return true;
    }
    
    xyzAxis[0] = 0;
    xyzAxis[1] = 1;
    xyzAxis[2] = 0;
    if( angleInDegrees( xyzAxis , axis ) == 0.0 )
    {
        return true;
    }
    xyzAxis[0] = 0;
    xyzAxis[1] = -1;
    xyzAxis[2] = 0;
    if( angleInDegrees( xyzAxis , axis ) == 0.0 )
    {
        return true;
    }
    
    return false;
}

void QMPRExtension::rotateMiddle( double degrees , double rotationAxis[3] ,  vtkPlaneSource* plane , vtkImageReslice *reslice )
{
    vtkMath::Normalize( rotationAxis );
    m_transform->Identity();
    m_transform->Translate( plane->GetCenter()[0], plane->GetCenter()[1], plane->GetCenter()[2] );
    m_transform->RotateWXYZ( degrees , rotationAxis );
    m_transform->Translate( -plane->GetCenter()[0], -plane->GetCenter()[1], -plane->GetCenter()[2] );
    // ara que tenim la transformaci�, l'apliquem als punts del pla ( origen, punt1 , punt2)
    double newPoint[3];
    m_transform->TransformPoint( plane->GetPoint1() , newPoint );
    plane->SetPoint1( newPoint );
    m_transform->TransformPoint( plane->GetPoint2() , newPoint );
    plane->SetPoint2( newPoint );
    m_transform->TransformPoint( plane->GetOrigin() , newPoint );
    plane->SetOrigin( newPoint );
    plane->Update();
}

void QMPRExtension::rotate( double degrees , double rotationAxis[3] ,  vtkPlaneSource* plane , vtkImageReslice *reslice )
{
//    Normalitzem l'eix de rotaci�, ser� molt millor per les operacions a fer
     vtkMath::Normalize( rotationAxis );

    if( isParallel( rotationAxis ) )
    {
        m_transform->Identity();
        m_transform->Translate( m_intersectionPoint[0], m_intersectionPoint[1], m_intersectionPoint[2] );
        m_transform->RotateWXYZ( degrees , rotationAxis );
        m_transform->Translate( -m_intersectionPoint[0], -m_intersectionPoint[1], -m_intersectionPoint[2] );
    }   
    else
    {   
    // (0)
        m_transform->Identity();
    // (1)
        m_transform->Translate( m_intersectionPoint[0], m_intersectionPoint[1], m_intersectionPoint[2] );
    // (2)
        double alpha, lp;
        lp = sqrt( rotationAxis[1]*rotationAxis[1] + rotationAxis[2]*rotationAxis[2] );
        alpha = asin( rotationAxis[1] / lp ) ;
//         alpha = acos( rotationAxis[2] / lp );
        m_transform->RotateX( -alpha * vtkMath::DoubleRadiansToDegrees()  ); // o RotateWXYZ( alpha , [1,0,0])
    // (3)
        double beta , l;
        l = sqrt( rotationAxis[0]*rotationAxis[0] + rotationAxis[1]*rotationAxis[1] + rotationAxis[2]*rotationAxis[2] );
        beta =  asin( rotationAxis[0] );
        m_transform->RotateY( beta * vtkMath::DoubleRadiansToDegrees()  ); // o RotateWXYZ( beta , [0,1,0])
    // (4)
        m_transform->RotateZ( degrees ); // o RotateWXYZ( degrees , [0,0,1])
    // (5)
        m_transform->RotateY( -beta * vtkMath::DoubleRadiansToDegrees() ); // o RotateWXYZ( -beta , [0,1,0])
    // (6)
        m_transform->RotateX( alpha * vtkMath::DoubleRadiansToDegrees()  ); // o RotateWXYZ( -alpha , [1,0,0])
    // (7)
        m_transform->Translate( -m_intersectionPoint[0], -m_intersectionPoint[1], -m_intersectionPoint[2] );
        
    }
    // ara que tenim la transformaci�, l'apliquem als punts del pla ( origen, punt1 , punt2)
    double newPoint[3];
    m_transform->TransformPoint( plane->GetPoint1() , newPoint );
    plane->SetPoint1( newPoint );
    m_transform->TransformPoint( plane->GetPoint2() , newPoint );
    plane->SetPoint2( newPoint );
    m_transform->TransformPoint( plane->GetOrigin() , newPoint );
    plane->SetOrigin( newPoint );
    
    plane->Update();
}

void QMPRExtension::readSettings()
{
    QSettings settings;
    settings.setPath("GGG", "StarViewer-App-MPR");
    settings.beginGroup("/StarViewer-App-MPR");
    
    QString str1 = settings.readEntry("/horizontalSplitter");
    QTextIStream in1(&str1);
    in1 >> *m_horizontalSplitter;
    QString str2 = settings.readEntry("/verticalSplitter");
    QTextIStream in2(&str2);
    in2 >> *m_verticalSplitter;
    
    settings.endGroup();
}

void QMPRExtension::writeSettings()
{
    QSettings settings;
    settings.setPath("GGG", "StarViewer-App-MPR");
    settings.beginGroup("/StarViewer-App-MPR");
    
    QString str;
    QTextOStream out1(&str);
    out1 << *m_horizontalSplitter;
    settings.writeEntry("/horizontalSplitter", str );
    QTextOStream out2(&str);
    out2 << *m_verticalSplitter;
    settings.writeEntry("/verticalSplitter", str );
    
    settings.endGroup();
}

};  // end namespace udg 

