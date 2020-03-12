#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("test");
    w.show();
    QFont font = a.font();
    font.setPointSize(12);//字体大小
    font.setFamily("Microsoft YaHei");//微软雅黑字体
    a.setFont(font);
    return a.exec();
}
