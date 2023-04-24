﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextCodec>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pMdiArea(new QMdiArea)
    , m_pStackedWidget(new QStackedWidget)
    , m_pCGProjectTreeView(new CGProjectTreeView)
    , m_pCGPropertiesView(new CGPropertiesView)
    , m_pCGDataTreeView(new CGDataTreeView)
    , m_pCGSubWindowWidget(new CGSubWindowWidget)
    , m_pCG2DImageView(new CG2DImageView)
    , m_pCG3DImageView(new CG3DImageView)
    , m_pCGProfileView(new CGProfileView)
    , m_pCGNodeView(new CGNodeView)
    , m_pCGUsersLoginDialog(new CGUsersLoginDialog)
    , m_pCGWaitingDialog(new CGWaitingDialog)
    , m_pCGAboutDialog(new CGAboutDialog)
    , m_pCGDisOrderDialog(new CGDisOrderDialog)
{
    ui->setupUi(this);
    ui->dock_console->setWidget(CGConsoleView::getInstance());
    ui->dock_project->setWidget(m_pCGProjectTreeView);
    ui->dock_properties->setWidget(m_pCGPropertiesView);
    ui->dock_data->setWidget(m_pCGDataTreeView);
    ui->action_console->setChecked(true);

    vtkObject::GlobalWarningDisplayOff();

    InitUi();
    InitConnections();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pCGViewRegulator;
}

void MainWindow::InitUi()
{
    m_pStackedWidget->addWidget(m_pCG2DImageView);
    m_pStackedWidget->addWidget(m_pCG3DImageView);
    m_pStackedWidget->addWidget(m_pCGNodeView);
    m_pStackedWidget->addWidget(m_pCGProfileView);
    m_pCGSubWindowWidget->setWidget(m_pStackedWidget);
    m_pMdiArea->addSubWindow(m_pCGSubWindowWidget);
    m_pCGSubWindowWidget->setWindowTitle(m_pCG2DImageView->windowTitle());
    m_pCGSubWindowWidget->showMaximized();

    setCentralWidget(m_pMdiArea);

    m_pCGViewRegulator = new CGViewRegulator();
    m_pCGViewRegulator->m_CG2DImageView = m_pCG2DImageView;
    m_pCGViewRegulator->m_CG3DImageView = m_pCG3DImageView;
    m_pCGViewRegulator->m_CGNodeView = m_pCGNodeView;
    m_pCGViewRegulator->m_CGProfileView = m_pCGProfileView;
}

void MainWindow::InitConnections()
{
    connect(m_pCGProjectTreeView->m_ProjectTree, &QTreeWidget::itemClicked, this, &MainWindow::OnProjectTreeItemSelected);
    connect(m_pCGDataTreeView->m_DataTree, &QTreeWidget::itemDoubleClicked, this, &MainWindow::OnDataTreeItemSelected);
    connect(m_pCGDataTreeView, &CGDataTreeView::Signal2DToolClear, m_pCG2DImageView, &CG2DImageView::OnDelTool);
    connect(m_pCGProfileView, &CGProfileView::SignalRequest, m_pCGViewRegulator, &CGViewRegulator::OnProfileViewRequest);
}

void MainWindow::QSS(const int Style)
{
    QFile styleFileAqua(":/qss/Aqua.qss");
    QFile styleFileMacOS(":/qss/MacOS.qss");
    QFile styleFileUbuntu(":/qss/Ubuntu.qss");
    QFile styleFileWindows(":/qss/Windows.qss");

    switch (Style) {
    // Windows
    case QSSStyle::WindowStyle:
        styleFileWindows.open(QFile::ReadOnly);
        if (styleFileWindows.isOpen())
        {
            QString setStyleSheet = QLatin1String(styleFileWindows.readAll());
            qApp->setStyleSheet(setStyleSheet);
        }
        styleFileWindows.close();
        break;
    // Aqua
    case QSSStyle::AquaStyle:
        styleFileAqua.open(QFile::ReadOnly);
        if (styleFileAqua.isOpen())
        {
            QString setStyleSheet = QLatin1String(styleFileAqua.readAll());
            qApp->setStyleSheet(setStyleSheet);
        }
        styleFileAqua.close();
        break;
    // MacOS
    case QSSStyle::MacOSStyle:
        styleFileMacOS.open(QFile::ReadOnly);
        if (styleFileMacOS.isOpen())
        {
            QString setStyleSheet = QLatin1String(styleFileMacOS.readAll());
            qApp->setStyleSheet(setStyleSheet);
        }
        styleFileMacOS.close();
        break;
    // Ubuntu
    case QSSStyle::UbuntuStyle:
        styleFileUbuntu.open(QFile::ReadOnly);
        if (styleFileUbuntu.isOpen())
        {
            QString setStyleSheet = QLatin1String(styleFileUbuntu.readAll());
            qApp->setStyleSheet(setStyleSheet);
        }
        styleFileUbuntu.close();
        break;
    // Windows
    default:
        styleFileWindows.open(QFile::ReadOnly);
        if (styleFileWindows.isOpen())
        {
            QString setStyleSheet = QLatin1String(styleFileWindows.readAll());
            qApp->setStyleSheet(setStyleSheet);
        }
        styleFileWindows.close();
        break;
    }
}

bool MainWindow::HandleOrderPointCloud()
{
    pcl::PointXYZRGB min_pt;
    pcl::PointXYZRGB max_pt;
    pcl::getMinMax3D(*g_PointCloud, min_pt, max_pt);

    int   rowNum = g_PointCloud->height;
    int   colNum = g_PointCloud->width;
    float XPitch = (max_pt.x - min_pt.x) / colNum;
    float YPitch = (max_pt.y - min_pt.y) / rowNum;

    std::thread([&] {
        CG::CreateImageALL(g_PointCloud, g_Image.DepthImage, g_Image.GrayImage, g_Image.IntensityImage, XPitch, YPitch, rowNum, colNum);
        CG::GrayMat2ColorMat(g_Image.GrayImage, g_Image.ColorImage);
        }).join();

    std::thread([&] {cv::imwrite("./Image/DepthImage.tiff", g_Image.DepthImage);}).detach();
    std::thread([&] {cv::imwrite("./Image/GrayImage.tiff", g_Image.GrayImage);}).detach();
    std::thread([&] {cv::imwrite("./Image/IntensityImage.tiff", g_Image.IntensityImage);}).detach();
    std::thread([&] {cv::imwrite("./Image/ColorImage.tiff", g_Image.ColorImage);}).detach();

    return true;
}

bool MainWindow::HandleDisOrderPointCloud()
{
    if (!m_pCGDisOrderDialog->exec())
        return false;

    std::vector<int> indices;
    pcl::removeNaNFromPointCloud(*g_PointCloud, *g_PointCloud, indices);
    pcl::PointXYZRGB min_p;
    pcl::PointXYZRGB max_p;
    pcl::getMinMax3D(*g_PointCloud, min_p, max_p);

    int   rowNum = m_pCGDisOrderDialog->m_Height;
    int   colNum = m_pCGDisOrderDialog->m_Width;
    float XPitch = (max_p.x - min_p.x) / colNum;
    float YPitch = (max_p.y - min_p.y) / rowNum;

    if (rowNum == 0 || colNum == 0) return false;

    std::thread([&] {
        CG::CreateImageALL(g_PointCloud, g_Image.DepthImage, g_Image.GrayImage, g_Image.IntensityImage, XPitch, YPitch, rowNum, colNum);
        CG::GrayMat2ColorMat(g_Image.GrayImage, g_Image.ColorImage);
        }).join();

    std::thread([&] {cv::imwrite("./Image/DepthImage.tiff", g_Image.DepthImage);}).detach();
    std::thread([&] {cv::imwrite("./Image/GrayImage.tiff", g_Image.GrayImage);}).detach();
    std::thread([&] {cv::imwrite("./Image/IntensityImage.tiff", g_Image.IntensityImage);}).detach();
    std::thread([&] {cv::imwrite("./Image/ColorImage.tiff", g_Image.ColorImage);}).detach();

    return true;
}

void MainWindow::Create2DTool(const QString toolname)
{
    int index = m_pCGDataTreeView->m_2DToolNames.indexOf(toolname);
    switch (index)
    {
    case CG2DImageView::ToolType::TwoPointLineTool:
        m_pCG2DImageView->m_CurrentToolType = CG2DImageView::ToolType::TwoPointLineTool;
        break;
    case CG2DImageView::ToolType::RectTool:
        m_pCG2DImageView->m_CurrentToolType = CG2DImageView::ToolType::RectTool;
        break;
    case CG2DImageView::ToolType::CircleTool:
        m_pCG2DImageView->m_CurrentToolType = CG2DImageView::ToolType::CircleTool;
        break;
    default:
        break;
    }
    m_pCG2DImageView->OnUseTool();
}

void MainWindow::Create3DTool(const QString toolname)
{

}

void MainWindow::CreateProfileTool(const QString toolname)
{

}

void MainWindow::CreateMaths(const QString toolname)
{

}

void MainWindow::CreateLogics(const QString toolname)
{

}

void MainWindow::Create2DFuction(const QString toolname)
{

}

void MainWindow::Create3DFuction(const QString toolname)
{

}

void MainWindow::OnProjectTreeItemSelected(QTreeWidgetItem *item, int column)
{
    QString str = item->text(column);
    int index = m_pCGProjectTreeView->m_TreeItemNames.indexOf(str);

    switch (index)
    {
    case CGProjectTreeView::Window2D:
        m_pStackedWidget->setCurrentWidget(m_pCG2DImageView);
        m_pCGSubWindowWidget->setWindowTitle(m_pCG2DImageView->windowTitle());
        break;
    case CGProjectTreeView::Window3D:
        m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
        m_pCGSubWindowWidget->setWindowTitle(m_pCG3DImageView->windowTitle());
        break;
    case CGProjectTreeView::WindowNodeEdit:
        m_pStackedWidget->setCurrentWidget(m_pCGNodeView);
        m_pCGSubWindowWidget->setWindowTitle(m_pCGNodeView->windowTitle());
        break;
    case CGProjectTreeView::WindowProfile:
        m_pCGProfileView->Request();
        m_pStackedWidget->setCurrentWidget(m_pCGProfileView);
        m_pCGSubWindowWidget->setWindowTitle(m_pCGProfileView->windowTitle());
        break;
    default:
        break;
    }
    m_pCGSubWindowWidget->show();
}

void MainWindow::OnDataTreeItemSelected(QTreeWidgetItem *item, int column)
{
    QString strParent = item->parent()->text(0);
    QString strColumn = item->text(column);
    //qDebug() << "DataTreeItem: " << strParent << "  " << strColumn;
    int index = m_pCGDataTreeView->m_DataTreeNames.indexOf(strParent);

    switch (index)
    {
    case CGDataTreeView::DateTreeEnum::ToolBox2D:
        Create2DTool(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCG2DImageView);
        break;
    case CGDataTreeView::DateTreeEnum::ToolBox3D:
        Create3DTool(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
        break;
    case CGDataTreeView::DateTreeEnum::ToolBoxProfile:
        CreateProfileTool(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCGProfileView);
        break;
    case CGDataTreeView::DateTreeEnum::Maths:
        CreateMaths(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCGNodeView);
        break;
    case CGDataTreeView::DateTreeEnum::Logics:
        CreateLogics(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCGNodeView);
        break;
    case CGDataTreeView::DateTreeEnum::Fuction2D:
        Create2DFuction(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCGNodeView);
        break;
    case CGDataTreeView::DateTreeEnum::Fuction3D:
        Create3DFuction(strColumn);
        m_pStackedWidget->setCurrentWidget(m_pCGNodeView);
        break;
    default:
        break;
    }
}

void MainWindow::OnUsersLogin(const QString user)
{

}

void MainWindow::on_action_new_triggered()
{

}

void MainWindow::on_action_open_triggered()
{

}

void MainWindow::on_action_exit_triggered()
{
    close();
}

void MainWindow::on_action_open_Image_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr(u8"打开图像文件"), ".", "*.bmp *.png *.jpg *.jpeg *.tif *.tiff");

    if (FileName.isEmpty())
    {
        QMessageBox::information(this, tr(u8"信息"), tr(u8"请选择图像文件！"));
    }
    else
    {
        m_pCG2DImageView->LoadImages(FileName);
        m_pStackedWidget->setCurrentWidget(m_pCG2DImageView);
    }
}

void MainWindow::on_action_open_PointCloud_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr(u8"打开点云文件"), ".", "*.pcd *.csv *.txt");

    if (FileName.isEmpty())
    {
        QMessageBox::information(this, tr(u8"信息"), tr(u8"请选择点云文件！"));
    }
    else
    {
        QFileInfo Info(FileName);
        //中文路径
        QTextCodec *code = QTextCodec::codecForName("GB2312");
        std::string filename = code->fromUnicode(FileName).data();

        if (Info.suffix().toLower() == "pcd")
            m_pCG3DImageView->LoadPCD(filename);
        if (Info.suffix().toLower() == "csv")
            m_pCG3DImageView->LoadCSV(filename);
        if (Info.suffix().toLower() == "txt")
            m_pCG3DImageView->LoadTXT(filename);

        m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);

        //有序点云？无序点云？
        if (CG::IsOrderPointCloud())
        {
            if (HandleOrderPointCloud())
            {
                QImage qColorImage = CG::CVMat2QImage(g_Image.ColorImage);
                QPixmap qPixmap = QPixmap::fromImage(qColorImage, Qt::AutoColor);
                m_pCG2DImageView->LoadImages(qPixmap);
            }
        }
        else
        {
            if (HandleDisOrderPointCloud())
            {
                QImage qColorImage = CG::CVMat2QImage(g_Image.ColorImage);
                QPixmap qPixmap = QPixmap::fromImage(qColorImage, Qt::AutoColor);
                m_pCG2DImageView->LoadImages(qPixmap);
            }
        }
    }
}

void MainWindow::on_action_save_PointCloud_triggered()
{
    QString FileName = QFileDialog::getSaveFileName(this, tr(u8"保存点云文件"), ".", "*.pcd");

    if (FileName.isEmpty())
    {
       QMessageBox::information(this, tr(u8"信息"), tr(u8"请选择点云文件！"));
    }
    else
    {
        if (FileName.contains(".pcd") || FileName.contains(".PCD"))
        {
            ; //Nothing to do
        }
        else
        {
            FileName.append(".pcd");
        }

        if (g_PointCloud->empty()) return;
        //中文路径
        QTextCodec *code = QTextCodec::codecForName("GB2312");
        std::string filePCD = code->fromUnicode(FileName).data();
        pcl::io::savePCDFileBinaryCompressed(filePCD, *g_PointCloud);
    }
}

void MainWindow::on_action_ClearAll_triggered()
{
    m_pCG2DImageView->ClearImages();
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->ClearPointCloud();
}

void MainWindow::on_action_FullScreen_triggered()
{
    m_pCGSubWindowWidget->showFullScreen();
}

void MainWindow::on_action_Elevation_triggered()
{
    if (m_pCG3DImageView->m_CGVTKWidget->actors3d().isEmpty()) return;
    auto actor = m_pCG3DImageView->m_CGVTKWidget->actors3d().back();
    CG::VTKPointCloudElevation(g_PointCloud, actor);
    m_pCG3DImageView->m_CGVTKWidget->update();
}

void MainWindow::on_action_Depth_triggered()
{
    if (m_pCG3DImageView->m_CGVTKWidget->actors3d().isEmpty()) return;
    auto actor = m_pCG3DImageView->m_CGVTKWidget->actors3d().back();
    CG::VTKPointCloudGray(g_PointCloud, actor);
    m_pCG3DImageView->m_CGVTKWidget->update();
}

void MainWindow::on_action_Intensity_triggered()
{
    if (m_pCG3DImageView->m_CGVTKWidget->actors3d().isEmpty()) return;
    auto actor = m_pCG3DImageView->m_CGVTKWidget->actors3d().back();
    CG::VTKPointCloudIntensity(g_PointCloud, actor);
    m_pCG3DImageView->m_CGVTKWidget->update();
}

void MainWindow::on_action_Login_triggered()
{
    m_pCGUsersLoginDialog->exec();
}

void MainWindow::on_action_about_triggered()
{
    m_pCGAboutDialog->exec();
}

void MainWindow::on_action_Aqua_triggered()
{
    int Style = QSSStyle::AquaStyle;
    this->QSS(Style);
}

void MainWindow::on_action_MacOS_triggered()
{
    int Style = QSSStyle::MacOSStyle;
    this->QSS(Style);
}

void MainWindow::on_action_Ubuntu_triggered()
{
    int Style = QSSStyle::UbuntuStyle;
    this->QSS(Style);
}

void MainWindow::on_action_Windows_triggered()
{
    int Style = QSSStyle::WindowStyle;
    this->QSS(Style);
}

void MainWindow::on_action_ZoomIn_triggered()
{
    if (m_pCG2DImageView->bGraphicsScene)
        m_pCG2DImageView->m_pGraphicsView->ZoomIn();
    m_pStackedWidget->setCurrentWidget(m_pCG2DImageView);
}

void MainWindow::on_action_ZoomOut_triggered()
{
    if (m_pCG2DImageView->bGraphicsScene)
        m_pCG2DImageView->m_pGraphicsView->ZoomOut();
    m_pStackedWidget->setCurrentWidget(m_pCG2DImageView);
}

void MainWindow::on_action_console_triggered(bool checked)
{
    if (checked)
            ui->dock_console->setVisible(true);
        else
            ui->dock_console->setVisible(false);
}

void MainWindow::on_action_SetViewTop_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewFront_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->m_CGVTKCamera->Elevation(-90);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewLeft_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->m_CGVTKCamera->Azimuth(-90);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewBack_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->m_CGVTKCamera->Elevation(90);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewRight_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->m_CGVTKCamera->Azimuth(90);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewBottom_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();
    m_pCG3DImageView->m_CGVTKCamera->Azimuth(180);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewIso1_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();

    double *pos = m_pCG3DImageView->GetCameraPosition();
    Eigen::Vector4d origin(pos[0], pos[1], pos[2], 1);
    Eigen::Vector4d conclusion(pos[0], pos[1], pos[2], 1);
    Eigen::Matrix4d rotateX = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d rotateY = Eigen::Matrix4d::Identity();

    double thetaX = 45;
    double thetaY = -45;
    rotateX(1, 1) = cos(thetaX / 180 * M_PI);
    rotateX(1, 2) = -sin(thetaX / 180 * M_PI);
    rotateX(2, 1) = sin(thetaX / 180 * M_PI);
    rotateX(2, 2) = cos(thetaX / 180 * M_PI);

    rotateY(0, 0) = cos(thetaY / 180 * M_PI);
    rotateY(0, 2) = sin(thetaY / 180 * M_PI);
    rotateY(2, 0) = -sin(thetaY / 180 * M_PI);
    rotateY(2, 2) = cos(thetaY / 180 * M_PI);

    conclusion = rotateX * rotateY *origin;

    m_pCG3DImageView->SetCameraParameter(conclusion[0], conclusion[1], conclusion[2], 0, 0, 1);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_SetViewIso2_triggered()
{
    m_pCG3DImageView->ResetCameraParameter();

    double *pos = m_pCG3DImageView->GetCameraPosition();
    Eigen::Vector4d origin(pos[0], pos[1], pos[2], 1);
    Eigen::Vector4d conclusion(pos[0], pos[1], pos[2], 1);
    Eigen::Matrix4d rotateX = Eigen::Matrix4d::Identity();
    Eigen::Matrix4d rotateY = Eigen::Matrix4d::Identity();

    double thetaX = -45;
    double thetaY = 45;
    rotateX(1, 1) = cos(thetaX / 180 * M_PI);
    rotateX(1, 2) = -sin(thetaX / 180 * M_PI);
    rotateX(2, 1) = sin(thetaX / 180 * M_PI);
    rotateX(2, 2) = cos(thetaX / 180 * M_PI);

    rotateY(0, 0) = cos(thetaY / 180 * M_PI);
    rotateY(0, 2) = sin(thetaY / 180 * M_PI);
    rotateY(2, 0) = -sin(thetaY / 180 * M_PI);
    rotateY(2, 2) = cos(thetaY / 180 * M_PI);

    conclusion = rotateX * rotateY *origin;

    m_pCG3DImageView->SetCameraParameter(conclusion[0], conclusion[1], conclusion[2], 0, 0, 1);
    m_pCG3DImageView->m_CGVTKWidget->defaultRenderer()->ResetCamera();
    m_pCG3DImageView->m_CGVTKWidget->update();
    m_pStackedWidget->setCurrentWidget(m_pCG3DImageView);
}

void MainWindow::on_action_dock_project_triggered()
{
    ui->dock_project->show();
}

void MainWindow::on_action_dock_properties_triggered()
{
    ui->dock_properties->show();
}

void MainWindow::on_action_dock_data_triggered()
{
    ui->dock_data->show();
}

void MainWindow::on_action_dock_console_triggered()
{
    ui->dock_console->show();
}
