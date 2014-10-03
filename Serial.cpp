#include "Serial.h"
#include <iostream>

Serial::Serial() {

}

Serial::~Serial() {

}

QList<QString> Serial::listaPortas() {
    int i = 0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString dialogStr = info.portName();
        listaDePortasSeriais.insert(i,info);
        listaDePortas.insert(i,dialogStr);
        i++;
    }
    return listaDePortas;
}

QSerialPortInfo Serial::selecionaPorta(int porta) {
    portaSelecionada.setPort(listaDePortasSeriais.at(porta-1));
    portaSelecionada.setBaudRate(QSerialPort::Baud38400);
    portaSelecionada.setDataBits(QSerialPort::Data8);
    portaSelecionada.setParity(QSerialPort::NoParity);
    portaSelecionada.setStopBits(QSerialPort::OneStop);
    if (portaSelecionada.open(QIODevice::ReadWrite)) {
        portaSelecionada.close();
        return listaDePortasSeriais.at(porta-1);
    }
}
