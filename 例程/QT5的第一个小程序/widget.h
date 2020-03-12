#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_checkBox_3_clicked(bool checked);

    void on_checkBox_clicked(bool checked);

    void on_checkBox_2_clicked(bool checked);

    void setTextFontColor();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
