#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <Windows.h>
#include "drvManagerNt.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = Q_NULLPTR);
    ~Widget();

private slots:

    void on_Open_File_clicked();

    void on_Install_clicked();

    void on_Start_clicked();

    void on_Close_clicked();

    void on_UnInstall_clicked();

private:
    Ui::Widget *ui;
	DWORD  dwRc = DRV_MANAGER_OK;
	CDrvManagerNt Drv;
};

#endif // WIDGET_H
