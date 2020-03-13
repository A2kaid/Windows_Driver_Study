#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
	setWindowIcon(QIcon(".\\Application.ico"));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_Open_File_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, QString("Open File"),
                                                        NULL,
                                                        QString("DriverFile (*.sys)"));
     QFile file(filepath);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
         QMessageBox::warning (this,tr("提示信息"),tr("文件打开失败"));
     }
     ui->File_Name->setText(filepath);
	 
	 QFileInfo fileinfo = QFileInfo(filepath);;
	 QString Drivername = fileinfo.fileName();

	 if (fileinfo.suffix() != tr("sys"))
	 {
		 ui->label->setText(tr("不是一个合法的sys文件"));
	 }
	 else
	 {
		 ui->label->setText(tr("就绪"));
		 Drivername = Drivername.replace(".sys", "");

		 const wchar_t * path = reinterpret_cast<const wchar_t *>(filepath.utf16());
		 const wchar_t * TargetName = reinterpret_cast<const wchar_t *>(Drivername.utf16());
		 Drv.SetTargetName(TargetName);
		 Drv.SetDriverPathName(path);
	 }
}

void Widget::on_Install_clicked()
{
    QString fileaddr = ui->File_Name->text();
    if (fileaddr.isEmpty())
    {
        QMessageBox::warning (this,tr("提示信息"),tr("请选择正确的驱动文件地址"));
    }
    else
    {
		dwRc = Drv.Install();
		if (DRV_MANAGER_OK != dwRc)
		{
			qDebug("dwRc = %x", dwRc);
			ui->label->setText(tr("驱动安装失败"));
			QString str1 = QString::number(dwRc);
			QMessageBox::warning(this, tr("提示信息"), str1);
		}
		else
		{
			ui->label->setText(tr("驱动安装成功"));
		}
    }
}

void Widget::on_Start_clicked()
{
    QString fileaddr = ui->File_Name->text();
    if (fileaddr.isEmpty())
    {
        QMessageBox::warning (this,tr("提示信息"),tr("请选择正确的驱动文件地址"));
    }
    else
    {
		dwRc = Drv.Run();
		if (DRV_MANAGER_OK != dwRc)
		{
			qDebug("dwRc = %x", dwRc);
			ui->label->setText(tr("驱动运行失败"));
		}
		else
		{
			ui->label->setText(tr("驱动运行成功"));
		}
    }
}

void Widget::on_Close_clicked()
{
    QString fileaddr = ui->File_Name->text();
    if (fileaddr.isEmpty())
    {
        QMessageBox::warning (this,tr("提示信息"),tr("请选择正确的驱动文件地址"));
    }
    else
    {
		dwRc = Drv.Stop();
		if (DRV_MANAGER_OK != dwRc)
		{
			qDebug("dwRc = %x",dwRc);
			ui->label->setText(tr("驱动停止失败,驱动可能没有运行成功"));
		}
		else
		{
			ui->label->setText(tr("驱动停止成功"));
		}
    }
}

void Widget::on_UnInstall_clicked()
{
    QString fileaddr = ui->File_Name->text();
    if (fileaddr.isEmpty())
    {
        QMessageBox::warning (this,tr("提示信息"),tr("请选择正确的驱动文件地址"));
    }
    else
    {
		dwRc = Drv.UnInstall();
		if (DRV_MANAGER_OK != dwRc)
		{
			qDebug("dwRc = %x",dwRc);
			ui->label->setText(tr("驱动卸载失败,驱动可能没有安装成功"));
		}
		else
		{
			ui->label->setText(tr("驱动卸载成功"));
		}
    }
}
