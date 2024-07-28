#include <QApplication>
#include "floatingWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FloatingWindow window;
    window.show();
    return QApplication::exec();
}
