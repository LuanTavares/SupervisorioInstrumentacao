#pragma once
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>

class Serial {

    QT_USE_NAMESPACE

public:
    Serial();
    ~Serial();
    QList <QString> listaPortas();                  // Método que retorna a lista com o nome das portas disponíveis
    QSerialPortInfo selecionaPorta(int porta);      // Método que seleciona a porta com base no valor recebido por parâmetro

private:
    QSerialPort portaSelecionada;                   // Variavel que recebe os dados da porta selecionada
    QList <QString> listaDePortas;                  // Lista que recebe o nome das portas dispoíveis
    QList <QSerialPortInfo> listaDePortasSeriais;   // Lista que recebe os dados das portas disponíveis


};
