#ifndef SUPERVISORIO_H
#define SUPERVISORIO_H

#include <QMainWindow>
#include <QTimer>
#include <QByteArray>
#include <iostream>
#include <QDebug>
#include <QFile>
#include "Serial.h"
#include "qcustomplot.h"

namespace Ui {
    class Supervisorio;
}

class Supervisorio : public QMainWindow {
    Q_OBJECT
    
public:
    explicit Supervisorio(QWidget *parent = 0);
    ~Supervisorio();
    
private:
    Ui::Supervisorio *ui;
    QList <QString> portasDisponiveis;
    Serial * serial;
    QSerialPort portaSelecionada;
    QByteArray stringLida;
    QByteArray stringArquivo;
    QTimer * leituraDaSerial;
    QTimer * atualizaTela;
    QVector <double> yAxis;
    QVector <double> xAxis;
    QCustomPlot * customPlot;
    int qtdPontosGrafico;
    int indiceGrafico;
    float fatorDeConversao;
    bool isPortaSelecionada;
    bool isLendoDados;
    QString diretorio;
    QFile * arquivo;

    void plotaGrafico(float x, float y);

public slots:
    void selecionaPortaSerial(int porta);
    void leDadosDaPortaSerial();
    void atualizaDados();
    void salvarEmArquivo();
    void iniciaLeitura();
    void paraLeitura();
};

#endif // SUPERVISORIO_H
