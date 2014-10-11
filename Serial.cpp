#include "Serial.h"
#include <iostream>

Serial::Serial() {

}

Serial::~Serial() {

}

QList<QString> Serial::listaPortas() {
    // Retorna uma lista com o nome das portas seriais disponíveis
    int i = 0;

    // Para cada porta disponível alimenta uma lista com as informações de cada porta e alimenta outra lista com o nome das portas
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString dialogStr = info.portName(); // Armazena temporariamente o nome das portas
        listaDePortasSeriais.insert(i,info); // Alimenta a lista de informação das portas disponíveis
        listaDePortas.insert(i,dialogStr);   // Alimenta a lista de nomes das portas disponíveis
        i++;
    }
    return listaDePortas;
}

QSerialPortInfo Serial::selecionaPorta(int porta) {
    // Seleciona na lista de informação das portas disponíveis a porta selecionada e retorna estas informações
    portaSelecionada.setPort(listaDePortasSeriais.at(porta-1)); // Alimenta a variável que recebe as configurações da porta serial
    portaSelecionada.setBaudRate(QSerialPort::Baud115200);
    portaSelecionada.setDataBits(QSerialPort::Data8);
    portaSelecionada.setParity(QSerialPort::NoParity);
    portaSelecionada.setStopBits(QSerialPort::OneStop);
    if (portaSelecionada.open(QIODevice::ReadWrite)) { // Testa se consegue abrir a porta selecionada
        portaSelecionada.close(); // Se conseguiu, fecha a porta
        return listaDePortasSeriais.at(porta-1); // E retorna a porta selecionada
    }
}
