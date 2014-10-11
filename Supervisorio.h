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
    Ui::Supervisorio *ui;               // Definição da Interface de usuário
    QList <QString> portasDisponiveis;  // Lista que receberá o nome das portas disponíveis
    Serial * serial;                    // Classe que busca as portas disponíveis
    QSerialPort portaSelecionada;       // Variável que contem as informalçoes da porta selecionada
    QByteArray stringLida;              // Buffer de leitura da porta serial
    QByteArray stringArquivo;           // Buffer de armazenamento dos valores convertidos para gravar no arquivo
    QTimer * leituraDaSerial;           // Temporizador do tempo de leitura
    QTimer * atualizaTela;              // Temporizador do tempo de amostragem
    QVector <double> yAxis;             // Vetor com os valores do eixo y do gráfico
    QVector <double> xAxis;             // Vetor com os valores do eixo x do gráfico
    QCustomPlot * customPlot;           // Classe que implementa o gráfico
    int qtdPontosGrafico;               // Quantidade de amostras no gráfica
    int indiceGrafico;                  // Variavel que controla a inserção do valores no gráfico
    float fatorDeConversao;             // Fator de conversão de 0 a 1024 para 0 a 5
    bool isPortaSelecionada;            // Variável que define se a porta serial está configurada e pronta para ser usada ou não
    QString diretorio;                  // Armazena o diretório para gravar o arquivo com os valores captados
    QFile * arquivo;                    // Classe que gerencia o arquivo (Abre, Grava, Fecha)

    void plotaGrafico(float x, float y);    // Função que incrementa o vetor com o valor passado por parâmetro e mostra no gráfico

public slots:
    void selecionaPortaSerial(int porta);   // Slot responsável por selecionar a porta serial, com base com o que foi passado por parâmetro
    void leDadosDaPortaSerial();            // Slot responsável por ler os dados da porta serial
    void atualizaDados();                   // Slot responsável por processar os dados armazenados no buffer
    void salvarEmArquivo();                 // Slot responsável por Salvar o bufer com os valores convertidos no arquivo
    void iniciaLeitura();                   // Slot responsável por iniciar a leitura e processamento dos dados da porta serial
    void paraLeitura();                     // Slot responsável por parar a leitura e o processamento dos dados da porta serial
};

#endif // SUPERVISORIO_H
