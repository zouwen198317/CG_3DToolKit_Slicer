﻿#include "CGImage3DSectionItemHorizontal.h"
#include <CGVTKHeader.h>
#include <CGVTKUtils.h>
#include <CGVTKWidget.h>
#include <vtkSphereSource.h>
#include <vtkPlaneSource.h>

CGImage3DSectionItemHorizontal::CGImage3DSectionItemHorizontal(QObject *parent) : QObject(parent)
{
    InitActor();
    InitConnections();
}

void CGImage3DSectionItemHorizontal::InitActor()
{
    m_Style = vtkSmartPointer<MouseEventInteractorStyle>::New();
    m_Style->setMoveActor(true);
    m_Plane = vtkSmartPointer<vtkActor>::New();
}

void CGImage3DSectionItemHorizontal::InitConnections()
{
    connect(m_Style, &MouseEventInteractorStyle::mouseMoved, this, &CGImage3DSectionItemHorizontal::OnPositionChange);
    connect(m_Style, &MouseEventInteractorStyle::mouseReleased, this, &CGImage3DSectionItemHorizontal::OnUpdate);
}

void CGImage3DSectionItemHorizontal::InitSectionItem()
{
    RemoveSectionItem();
    GetBounds(m_Actor);

    vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
    plane->SetOrigin(m_Bounds[0], m_Bounds[2] + (m_Bounds[3] - m_Bounds[2]) / 2, m_Bounds[4]);
    plane->SetPoint1(m_Bounds[0], m_Bounds[2] + (m_Bounds[3] - m_Bounds[2]) / 2, m_Bounds[5]);
    plane->SetPoint2(m_Bounds[1], m_Bounds[2] + (m_Bounds[3] - m_Bounds[2]) / 2, m_Bounds[4]);

    vtkSmartPointer<vtkPolyDataMapper> mapper_plane  = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_plane ->SetInputConnection(plane->GetOutputPort());
    m_Plane->SetMapper(mapper_plane);
    m_Plane->GetProperty()->SetColor(1, 0, 1);
    m_Plane->GetProperty()->SetOpacity(0.7);
    m_Plane->GetPosition(m_PlanePos);

    m_CGVTKWidget->defaultRenderer()->AddActor(m_Plane);
    m_CGVTKWidget->update();
}

void CGImage3DSectionItemHorizontal::SetInteractorStyleDefault()
{
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_Style->SetDefaultRenderer(m_CGVTKWidget->defaultRenderer());
    m_CGVTKWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    m_CGVTKWidget->update();
}

void CGImage3DSectionItemHorizontal::SetInteractorStyleMouseEvent()
{
    m_Style->m_SectionType = MouseEventInteractorStyle::SectionType::SectionItemHorizontal;
    m_Style->SetDefaultRenderer(m_CGVTKWidget->defaultRenderer());
    m_CGVTKWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_Style);
    m_CGVTKWidget->update();
}

void CGImage3DSectionItemHorizontal::RemoveSectionItem()
{
    SetInteractorStyleDefault();
    m_CGVTKWidget->defaultRenderer()->RemoveActor(m_Plane);
    m_CGVTKWidget->update();
}

void CGImage3DSectionItemHorizontal::OnPositionChange(double* pos)
{
    double posact = pos[1] + m_Bounds[2] + (m_Bounds[3] - m_Bounds[2]) / 2;

    //m_Plane->SetPosition(m_PlanePos[0], pos[1], m_PlanePos[2]);
    emit SignalPositionChange((posact - m_Bounds[2]) / (m_Bounds[3] - m_Bounds[2]));
}

void CGImage3DSectionItemHorizontal::OnUpdate()
{
    m_CGVTKWidget->update();
}

void CGImage3DSectionItemHorizontal::GetBounds(vtkSmartPointer<vtkActor> actor)
{
    actor->GetBounds(m_Bounds);
}

void CGImage3DSectionItemHorizontal::SetVTKWidget(CGVTKWidget *widget)
{
    m_CGVTKWidget = widget;
}

void CGImage3DSectionItemHorizontal::SetActor(vtkSmartPointer<vtkActor>actor)
{
    m_Actor = actor;
    m_Actor->SetPickable(0);
}
