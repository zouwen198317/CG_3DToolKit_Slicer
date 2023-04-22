﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <CGCommon.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMdiArea;
class QMdiSubWindow;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void InitUi();
    void InitConnections();
    void QSS(const int Style);

private:
    CGProjectTreeView *m_pCGProjectTreeView;
    CGPropertiesView *m_pCGPropertiesView;
    CGDataTreeView *m_pCGDataTreeView;
    CGSubWindowWidget *m_pCGSubWindowWidget;
    CG2DImageView *m_pCG2DImageView;
    CG3DImageView *m_pCG3DImageView;
    CGProfileView *m_pCGProfileView;
    CGNodeView *m_pCGNodeView;

    QMdiArea *m_pMdiArea;
    QStackedWidget *m_pStackedWidget;

    CGUsersLoginDialog *m_pCGUsersLoginDialog;
    CGWaitingDialog *m_pCGWaitingDialog;
    CGAboutDialog *m_pCGAboutDialog;

    CGViewRegulator *m_pCGViewRegulator;

private slots:
    void OnProjectTreeItemSelected(QTreeWidgetItem *item, int column);
    void OnUsersLogin(const QString user);

    void on_action_new_triggered();
    void on_action_open_triggered();
    void on_action_exit_triggered();

    void on_action_open_Image_triggered();
    void on_action_open_PointCloud_triggered();
    void on_action_save_PointCloud_triggered();

    void on_action_ClearAll_triggered();
    void on_action_FullScreen_triggered();
    void on_action_Elevation_triggered();
    void on_action_Depth_triggered();
    void on_action_Intensity_triggered();

    void on_action_Login_triggered();
    void on_action_about_triggered();

    void on_action_Aqua_triggered();
    void on_action_MacOS_triggered();
    void on_action_Ubuntu_triggered();
    void on_action_Windows_triggered();

    void on_action_ZoomIn_triggered();
    void on_action_ZoomOut_triggered();
    void on_action_console_triggered(bool checked);

    void on_action_SetViewTop_triggered();
    void on_action_SetViewFront_triggered();
    void on_action_SetViewLeft_triggered();
    void on_action_SetViewBack_triggered();
    void on_action_SetViewRight_triggered();
    void on_action_SetViewBottom_triggered();
    void on_action_SetViewIso1_triggered();
    void on_action_SetViewIso2_triggered();

private:
    enum QSSStyle
    {
        WindowStyle,
        AquaStyle,
        MacOSStyle,
        UbuntuStyle
    };

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
