#pragma once
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>

class Serial {

    QT_USE_NAMESPACE

public:
    Serial();
    ~Serial();
    QList <QString> listaPortas();
    QSerialPortInfo selecionaPorta(int porta);

private:
    QSerialPort portaSelecionada;
    QList <QString> listaDePortas;
    QList <QSerialPortInfo> listaDePortasSeriais;


};
