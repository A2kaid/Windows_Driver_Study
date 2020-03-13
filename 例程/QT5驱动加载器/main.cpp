#include "widget.h"
#include <QApplication>
#include <QFileDialog>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    QFont font = a.font();
    font.setPointSize(13);
    font.setFamily("SimHei");
    a.setFont(font);
    return a.exec();
}
