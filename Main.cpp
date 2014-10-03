#include <QApplication>
#include <QtCore/QDebug>
#include "Supervisorio.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Supervisorio * supervisorio = new Supervisorio();
    supervisorio->show();

    return a.exec();
}

