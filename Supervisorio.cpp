#include "Supervisorio.h"
#include "ui_Supervisorio.h"

Supervisorio::Supervisorio(QWidget *parent) : QMainWindow(parent), ui(new Ui::Supervisorio) {
    ui->setupUi(this);
    serial = new Serial();
    portasDisponiveis = serial->listaPortas();

    portasDisponiveis.push_front("");

    for(int i = 0 ; i < portasDisponiveis.size(); i++) {
        ui->comboBoxPortasSeriais->addItem(portasDisponiveis.at(i));
    }

    leituraDaSerial = new QTimer();
    leituraDaSerial->start(5); // Tempo de Leitura
    qtdPontosGrafico = 200;

    atualizaTela = new QTimer();
    atualizaTela->start(5); // Tempo de Amostragem

    customPlot = new QCustomPlot();
    customPlot->addGraph();
    ui->verticalLayout->addWidget(customPlot);

    fatorDeConversao = (float) (5.0/1023.0);
    qDebug() << fatorDeConversao;

    isPortaSelecionada = false;

    indiceGrafico = 0;

    connect(ui->comboBoxPortasSeriais, SIGNAL(currentIndexChanged(int)), this, SLOT(selecionaPortaSerial(int)));
    connect(leituraDaSerial, SIGNAL(timeout()), this, SLOT(leDadosDaPortaSerial()));
    connect(atualizaTela, SIGNAL(timeout()), this, SLOT(atualizaDados()));
}

Supervisorio::~Supervisorio() {
    delete ui;
    portaSelecionada.close();
}

void Supervisorio::selecionaPortaSerial(int porta) {
    portaSelecionada.setPortName(serial->selecionaPorta(porta).portName());
    portaSelecionada.setBaudRate(QSerialPort::Baud115200);
    portaSelecionada.setDataBits(QSerialPort::Data8);
    portaSelecionada.setParity(QSerialPort::NoParity);
    portaSelecionada.setStopBits(QSerialPort::OneStop);
    if (portaSelecionada.open(QIODevice::ReadWrite)) {
        isPortaSelecionada = portaSelecionada.setBaudRate(QSerialPort::Baud115200) &
                             portaSelecionada.setStopBits(QSerialPort::OneStop) &
                             portaSelecionada.setDataBits(QSerialPort::Data8) &
                             portaSelecionada.setParity(QSerialPort::NoParity) &
                             portaSelecionada.setFlowControl(QSerialPort::NoFlowControl);
    } else {
        std::cout << "Não foi possível abrir a porta serial selecionada" << std::endl;
    }
}

void Supervisorio::leDadosDaPortaSerial() {
    if (isPortaSelecionada) {
        stringLida += portaSelecionada.readAll();
    } else {
        std::cout << "Não há porta Serial Selecionada!" << std::endl;
    }

}

void Supervisorio::plotaGrafico(float x, float y) {
    if ((indiceGrafico += x) <= qtdPontosGrafico+1) {
        if(yAxis.isEmpty() || xAxis.isEmpty()) {
            yAxis << y;
            xAxis << 0;
        } else {
            yAxis.push_back(y);
            xAxis.push_back(x+(xAxis.last()));
        }
    } else {
        yAxis.remove(0);
        xAxis.remove(0);
        yAxis.push_back(y);
        for (int i=0; i<xAxis.size(); i++) {
            xAxis.replace(i,(xAxis.at(i)-1));
        }
        xAxis.push_back(x+(xAxis.last()));
    }

    //qDebug() << y << " " << xAxis.last();
    customPlot->graph(0)->setData(xAxis,yAxis);
    customPlot->xAxis->setLabel("Tempo (ms)");
    customPlot->xAxis->setRange(0, qtdPontosGrafico);
    customPlot->yAxis->setRange(0,5);
    customPlot->yAxis->setLabel("Variação da Tensão");
    customPlot->replot();
}

void Supervisorio::atualizaDados() {
    if (isPortaSelecionada) {

        if (stringLida.size() > 0) {
            QString valorLido;

            int primeiroRegistro = stringLida.indexOf('[');
            int ultimoRegistro = stringLida.indexOf(']');
            if ((primeiroRegistro != -1) &&(ultimoRegistro != -1)) {
                int j=0;
                for (int i= primeiroRegistro+1; i<ultimoRegistro; i++) {
                    valorLido.insert(j,stringLida.at(i));
                    j++;
                }
                float valorConvertido = valorLido.toFloat();
                valorConvertido *= fatorDeConversao;
                //qDebug() << valorConvertido << "  " << fatorDeConversao;
                plotaGrafico(1,valorConvertido);
            }
            valorLido.clear();
            stringLida.remove(0,ultimoRegistro+1);
        }
    }
}
