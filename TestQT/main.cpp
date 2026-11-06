#include "interfacescan.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InterfaceScan w;
    w.show();
    return QApplication::exec();
}
