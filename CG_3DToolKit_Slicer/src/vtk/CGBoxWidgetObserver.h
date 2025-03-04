﻿#ifndef BOXWIDGETOBSERVER_H
#define BOXWIDGETOBSERVER_H

#include "CGAbstractWidgetObserver.h"
#include <vtkSmartPointer.h>

class vtkPlanes;
namespace CGVTKUtils
{

class CGBoxWidgetObserver : public AbstractWidgetObserver
{
    Q_OBJECT

public:
    explicit CGBoxWidgetObserver(QObject* parent = nullptr);

signals:
    void planesChanged(vtkPlanes* planes);

protected:
    void Execute(vtkObject *caller, unsigned long eventId, void* callData);

    vtkSmartPointer<vtkPlanes> m_planes;
};

} // namespace CGVTKUtils
#endif // BOXWIDGETOBSERVER_H
