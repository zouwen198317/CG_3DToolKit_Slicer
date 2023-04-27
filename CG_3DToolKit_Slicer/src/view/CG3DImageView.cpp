﻿#include "CG3DImageView.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QIcon>
#include <QDebug>
#include <Utils.h>
#include <CGVTKUtils.h>
#include <CGVTKHeader.h>
#include <CGPCLHeader.h>
#include <CGPointCloud.h>
#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation3D.h>
#include <vtkAngleWidget.h>
#include <vtkAngleRepresentation3D.h>
#include <vtkBoxWidget.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkPointPicker.h>


CG3DImageView::CG3DImageView(QWidget *parent) : CGBaseWidget(parent)
{
    InitUi();
    InitActors();
    InitTools();
    InitPointPick();
    InitConnections();
    GetCamera();
    ShowText2D();
    ShowText3D();
    setWindowTitle(tr(u8"3D  图像"));
    setWindowIcon(QIcon(":/res/icon/slicer.png"));
}

CG3DImageView::~CG3DImageView()
{

}

void CG3DImageView::OnUseTool()
{
    if (g_PointCloud == nullptr) return;

    RemoveTools();

    switch (m_CurrentToolType)
    {
    case DistanceTool:
        InitDistanceTool();
        break;

    case AngleTool:
        InitAngleTool();
        break;

    case BoxTool:
        InitBoxTool();
        break;

    default:
        break;
    }
    m_LastToolType = m_CurrentToolType;
    m_CGVTKWidget->update();

    IsTool = true;
}

void CG3DImageView::OnDelTool()
{
    RemoveTools();
}

void CG3DImageView::OnUpdatePoint(float x, float y, float z)
{
    char chrX[16], chrY[16], chrZ[16];
    sprintf(chrX, "%.4f", x);
    sprintf(chrY, "%.4f", y);
    sprintf(chrZ, "%.4f", z);
    std::string strX, strY, strZ;
    strX = "X: "; strX.append(chrX);
    strY = "Y: "; strY.append(chrY);
    strZ = "Z: "; strZ.append(chrZ);
    m_TextActor_X->SetInput(strX.c_str());
    m_TextActor_Y->SetInput(strY.c_str());
    m_TextActor_Z->SetInput(strZ.c_str());

    m_CGVTKWidget->update();
}

void CG3DImageView::InitUi()
{
    m_CGVTKWidget = new CGVTKWidget(this);

    double clr[3];
    QColor defaultColor(25, 50, 75);
    VTKUtils::vtkColor(defaultColor, clr);
    m_CGVTKWidget->defaultRenderer()->SetBackground(clr);
    m_CGVTKWidget->showOrientationMarker();
    m_CGVTKWidget->update();

    QGridLayout *pMainLayout = new QGridLayout();
    pMainLayout->addWidget(m_CGVTKWidget);

    setLayout(pMainLayout);
}

void CG3DImageView::InitConnections()
{
    connect(m_CGPointPicker, &CGVTKUtils::CGPointPickObserver::SignalPoint, this, &CG3DImageView::OnUpdatePoint);
}

void CG3DImageView::ShowText2D()
{
    m_TextActor_X->SetVisibility(0);
    m_TextActor_Y->SetVisibility(0);
    m_TextActor_Z->SetVisibility(0);

    m_TextActor_X->SetPosition(50, 700);
    m_TextActor_X->GetTextProperty()->SetFontSize(18);
    m_TextActor_X->GetTextProperty()->SetColor(1, 1, 0);

    m_TextActor_Y->SetPosition(50, 680);
    m_TextActor_Y->GetTextProperty()->SetFontSize(18);
    m_TextActor_Y->GetTextProperty()->SetColor(1, 1, 0);

    m_TextActor_Z->SetPosition(50, 660);
    m_TextActor_Z->GetTextProperty()->SetFontSize(18);
    m_TextActor_Z->GetTextProperty()->SetColor(1, 1, 0);

    m_CGVTKWidget->defaultRenderer()->AddActor(m_TextActor_X);
    m_CGVTKWidget->defaultRenderer()->AddActor(m_TextActor_Y);
    m_CGVTKWidget->defaultRenderer()->AddActor(m_TextActor_Z);

    m_CGVTKWidget->update();
}

void CG3DImageView::ShowText3D()
{
    m_CGVTKWidget->update();
}

void CG3DImageView::ShowPCD()
{
    //! qDebug() << "ShowPCD "
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    CG::VTKPointCloudIntensity(g_PointCloud, actor);

    m_Actor = actor;
    m_CGVTKWidget->addActor3D(actor, QColor(25, 50, 75));
    m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_CGVTKWidget->update();
}

void CG3DImageView::ShowPointPickInfo(const bool enable)
{
    if (enable)
    {
        m_CGPointPicker->SetPickEnable(true);
        m_TextActor_X->SetVisibility(1);
        m_TextActor_Y->SetVisibility(1);
        m_TextActor_Z->SetVisibility(1);
    }
    else
    {
        m_CGPointPicker->SetPickEnable(false);
        m_TextActor_X->SetVisibility(0);
        m_TextActor_Y->SetVisibility(0);
        m_TextActor_Z->SetVisibility(0);
    }
    m_CGVTKWidget->update();
}

void CG3DImageView::LoadPCD(const std::string filename)
{
    //! qDebug() << "LoadPCD " << QString::fromStdString(filename);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    CG::LoadPCDFile(filename, actor);

    m_Actor = actor;
    m_CGVTKWidget->addActor3D(actor, QColor(25, 50, 75));
    m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_CGVTKWidget->update();
}

void CG3DImageView::LoadCSV(const std::string filename)
{
    //! qDebug() << "LoadCSV " << QString::fromStdString(filename);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    CG::LoadCSVFile(filename, actor);

    m_Actor = actor;
    m_CGVTKWidget->addActor3D(actor, QColor(25, 50, 75));
    m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_CGVTKWidget->update();
}

void CG3DImageView::LoadTXT(const std::string filename)
{
    //!qDebug() << "LoadTXT " << QString::fromStdString(filename);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    CG::LoadTXTFile(filename, actor);

    m_Actor = actor;
    m_CGVTKWidget->addActor3D(actor, QColor(25, 50, 75));
    m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_CGVTKWidget->update();
}

void CG3DImageView::ClearPointCloud()
{
    int num = m_CGVTKWidget->actors3d().count();
    for (int i = 0; i < num; ++i)
    {
        m_CGVTKWidget->defaultRenderer()->RemoveActor(m_CGVTKWidget->actors3d()[i]);
    }
    m_CGVTKWidget->update();
}

void CG3DImageView::ResetCameraParameter()
{
    m_CGVTKCamera->SetFocalPoint(pCameraFocalPoint);
    m_CGVTKCamera->SetPosition(pCameraPosition);
    m_CGVTKCamera->SetViewUp(pCameraViewUp);
    m_CGVTKCamera->SetClippingRange(pCameraClippingRange);
    m_CGVTKCamera->SetViewAngle(fovy);
}

void CG3DImageView::GetCameraParameter()
{
    GetCameraFocalPoint();
    GetCameraPosition();
    GetCameraViewUp();
    GetCameraClippingRange();
    GetCameraViewAngle();
}

void CG3DImageView::SetCameraParameter(double pos_x, double pos_y, double pos_z, double up_x, double up_y, double up_z)
{
    m_CGVTKCamera->SetPosition(pos_x, pos_y, pos_z);
    m_CGVTKCamera->SetViewUp(up_x, up_y, up_z);
}

vtkCamera* CG3DImageView::GetCamera()
{
    m_CGVTKCamera = m_CGVTKWidget->defaultRenderer()->GetActiveCamera();
    GetCameraParameter();
    return m_CGVTKCamera;
}

double *CG3DImageView::GetCameraFocalPoint()
{
    static double arg[3];
    m_CGVTKWidget->defaultRenderer()->GetActiveCamera()->GetFocalPoint(arg);
    pCameraFocalPoint = arg;
    return pCameraFocalPoint;
}

double *CG3DImageView::GetCameraPosition()
{
    static double arg[3];
    m_CGVTKWidget->defaultRenderer()->GetActiveCamera()->GetPosition(arg);
    pCameraPosition = arg;
    return pCameraPosition;
}

double *CG3DImageView::GetCameraViewUp()
{
    static double arg[3];
    m_CGVTKWidget->defaultRenderer()->GetActiveCamera()->GetViewUp(arg);
    pCameraViewUp = arg;
    return pCameraViewUp;
}

double *CG3DImageView::GetCameraClippingRange()
{
    static double arg[2];
    m_CGVTKWidget->defaultRenderer()->GetActiveCamera()->GetClippingRange(arg);
    pCameraClippingRange = arg;
    return pCameraClippingRange;
}

double CG3DImageView::GetCameraViewAngle()
{
    fovy = m_CGVTKWidget->defaultRenderer()->GetActiveCamera()->GetViewAngle();
    return fovy;
}

void CG3DImageView::InitDistanceTool()
{
    m_pDistanceWidgetTool->SetRepresentation(m_pDistanceRep);
    m_pDistanceWidgetTool->SetInteractor(m_CGVTKWidget->GetInteractor());
    m_pDistanceWidgetTool->On();
}

void CG3DImageView::InitAngleTool()
{
    m_pAngleWidgetTool->SetRepresentation(m_pAngleRep);
    m_pAngleWidgetTool->SetInteractor(m_CGVTKWidget->GetInteractor());
    m_pAngleWidgetTool->On();
}

void CG3DImageView::InitBoxTool()
{
    m_pBoxWidgetTool->SetInteractor(m_CGVTKWidget->GetInteractor());
    m_pBoxWidgetTool->SetProp3D(m_Actor);
    m_pBoxWidgetTool->PlaceWidget();
    m_pBoxWidgetTool->On();
}

vtkActor* CG3DImageView::GetActor() const
{
    return m_CGVTKWidget->actors3d().back();
}

void CG3DImageView::InitActors()
{
    CGVTKUtils::vtkInitOnce(m_Actor);
    CGVTKUtils::vtkInitOnce(m_TextActor_X);
    CGVTKUtils::vtkInitOnce(m_TextActor_Y);
    CGVTKUtils::vtkInitOnce(m_TextActor_Z);
}

void CG3DImageView::InitTools()
{
    CGVTKUtils::vtkInitOnce(m_pDistanceWidgetTool);
    CGVTKUtils::vtkInitOnce(m_pDistanceRep);
    CGVTKUtils::vtkInitOnce(m_pAngleWidgetTool);
    CGVTKUtils::vtkInitOnce(m_pAngleRep);
    CGVTKUtils::vtkInitOnce(m_pBoxWidgetTool);

    m_pBoxWidgetTool->SetPlaceFactor(1.0);
    m_pBoxWidgetTool->SetRotationEnabled(0);
}

void CG3DImageView::InitPointPick()
{
    vtkSmartPointer<vtkPointPicker> PointPicker = vtkSmartPointer<vtkPointPicker>::New();
    m_CGVTKWidget->GetInteractor()->SetPicker(PointPicker);

    m_CGPointPicker = new CGVTKUtils::CGPointPickObserver();
    m_CGPointPicker->SetPickEnable(false);
    m_CGVTKWidget->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, m_CGPointPicker);
}

void CG3DImageView::RemoveTools()
{
    if (IsTool)
    {
        switch (m_LastToolType)
        {
        case DistanceTool:
            m_pDistanceWidgetTool->Off();
            break;

        case AngleTool:
            m_pAngleWidgetTool->Off();
            break;

        case BoxTool:
            m_pBoxWidgetTool->Off();
            break;
        default:
            break;
        }
        m_CGVTKWidget->update();

        IsTool = false;
    }
}
