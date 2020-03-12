#include "widget.h"
#include "ui_widget.h"
#include <Windows.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->rBtnYellow,SIGNAL(clicked()),this,SLOT(setTextFontColor()));
    connect(ui->rBtnBlue,SIGNAL(clicked()),this,SLOT(setTextFontColor()));
    connect(ui->rBtnRed,SIGNAL(clicked()),this,SLOT(setTextFontColor()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_checkBox_3_clicked(bool checked)
{
    QFont font=ui->plainTextEdit->font();
    font.setUnderline(checked);
    ui->plainTextEdit->setFont(font);
}

void Widget::on_checkBox_clicked(bool checked)
{
    QFont font=ui->plainTextEdit->font();
    font.setItalic(checked);
    ui->plainTextEdit->setFont(font);
}

void Widget::on_checkBox_2_clicked(bool checked)
{
    QFont font=ui->plainTextEdit->font();
    font.setBold(checked);
    ui->plainTextEdit->setFont(font);
}

void Widget::setTextFontColor()
{
    QPalette plet=ui->plainTextEdit->palette();
    if (ui->rBtnYellow->isChecked())
        plet.setColor(QPalette::Text,Qt::yellow);
    else if (ui->rBtnRed->isChecked())
       plet.setColor(QPalette::Text,Qt::red);
    else if (ui->rBtnBlue->isChecked())
        plet.setColor(QPalette::Text,Qt::blue);
    else
       plet.setColor(QPalette::Text,Qt::black);
    ui->plainTextEdit->setPalette(plet);
}
