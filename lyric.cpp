// main.cpp
#include <QApplication>
#include "LyricWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LyricWidget w;
    w.show();

    return a.exec();
}