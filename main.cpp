#include "MazeWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MazeWidget w;
    w.show();
    a.setWindowIcon(QIcon("s.ico"));
    return a.exec();
}
