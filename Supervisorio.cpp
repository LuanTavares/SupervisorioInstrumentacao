#include "Supervisorio.h"
#include "ui_Supervisorio.h"

Supervisorio::Supervisorio(QWidget *parent) : QMainWindow(parent), ui(new Ui::Supervisorio) {
    // Seta a User Interface
    ui->setupUi(this);

    // Busca as portas seriais disponíveis e mostra no combo box
    serial = new Serial();

    // Alimenta a lista de portas Seriais
    portasDisponiveis = serial->listaPortas();

    // Adiciona um valor em branco nesta lista, pois como a seleção da porta é pela alteração de índice do combo box
    // se tiver apenas 1 porta disponível não é possível mudar de índice, impedindo que o lógica de seleção de porta funcione
    portasDisponiveis.push_front("");

    // Para cada ítem da lista adiciona uma opção no combo box
    for(int i = 0 ; i < portasDisponiveis.size(); i++) {
        ui->comboBoxPortasSeriais->addItem(portasDisponiveis.at(i)); // Adiciona uma opção no combo box
    }

    // Define informações de tempo de aquisição e amostragem
    leituraDaSerial = new QTimer();
    atualizaTela = new QTimer();

    // Define a quantidade de amostras no gráfico e inicializa os vetores dos eixos x e y
    qtdPontosGrafico = 2000;
    for (int i=0; i < qtdPontosGrafico; i++) {
        xAxis << 0;
        yAxis << 0;
    }

    //
    customPlot = new QCustomPlot();
    customPlot->addGraph();
    ui->verticalLayout->addWidget(customPlot);

    // Inicializa Variáveis globais de controle
    fatorDeConversao = (float) (5.0/1023.0); // Fator de conversão para multiplicar pelo valor enviado pelo arduino
    isPortaSelecionada = false; // Inicializa como falso a variável que define que a porta serial está configurada corretamente
    indiceGrafico = 0; // Variável que controla onde inserir os valores lidos no gráfico
    stringArquivo.clear(); // Limpa o Vetor utilizado para gravar em arquivo

    connect(ui->comboBoxPortasSeriais, SIGNAL(currentIndexChanged(int)), this, SLOT(selecionaPortaSerial(int))); // Selecionar a porta serial escolhida no combo box
    connect(leituraDaSerial, SIGNAL(timeout()), this, SLOT(leDadosDaPortaSerial())); // Aquisição do sinal
    connect(atualizaTela, SIGNAL(timeout()), this, SLOT(atualizaDados())); // Amostragem
    connect(ui->pushButtonSalvar, SIGNAL(clicked()), this, SLOT(salvarEmArquivo())); // Salvar em Arquivo
    connect(ui->pushButtonIniciar, SIGNAL(clicked()), this, SLOT(iniciaLeitura())); // Inicia Leitura
    connect(ui->pushButtonParar, SIGNAL(clicked()), this, SLOT(paraLeitura())); // Para Leitura

}

Supervisorio::~Supervisorio() {
    delete ui;
    portaSelecionada.close();
}

void Supervisorio::selecionaPortaSerial(int porta) {
    // Conforme o indice que for passado por parâmetro, irá configurar a porta serial com os dados da porta selecionada no combo box
    portaSelecionada.setPortName(serial->selecionaPorta(porta).portName());
    portaSelecionada.setBaudRate(QSerialPort::Baud115200); // Velocidade da comunicação deve ser alta para evitar perda de dados
    portaSelecionada.setDataBits(QSerialPort::Data8);
    portaSelecionada.setParity(QSerialPort::NoParity);
    portaSelecionada.setStopBits(QSerialPort::OneStop);

    // Valida se com os dados configurados anteriormente consegue abrir a porta serial configurada
    // Se abrir seta a variável global que define que a porta serial está configurada corretamente
    // Se não abrir, informa o usuário
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
    // Se a porta serial estiver configurada corretamente lê os dados e alimenta o buffer
    // Se não avisa o usuário que nenhuma porta está configurada corretamente
    if (isPortaSelecionada) {
        stringLida += portaSelecionada.readAll();
    } else {
        std::cout << "Não há porta Serial Selecionada!" << std::endl;
    }

}

void Supervisorio::plotaGrafico(float x, float y) {
    // Cria um gráfico com base nos vetores X e Y

    // Incrementa variável de controle para que o valor repassado seja inserido na próxima posição do vetor
    indiceGrafico++;

    // Se a variável de controle for maior ou igual que a quantidade de amostras da tela, zera para substituir os valores desde o começo
    if (indiceGrafico >= qtdPontosGrafico) {
        indiceGrafico = 0;
    }

    // Substitui o valor anterior do vetor pelo valor atual no indice da veriável de controle
    yAxis.replace(indiceGrafico,y);
    xAxis.replace(indiceGrafico,indiceGrafico);

/*****************************************************************
    // Forma para plotar o gráfico igual ao osciloscópio

    if ((indiceGrafico += x) <= qtdPontosGrafico+1) { // Se a variável de controle mais 1 é menor que a quantidade de amostras
        if(yAxis.isEmpty() || xAxis.isEmpty()) {      // Verifica se os vetores estão vazios
            yAxis << y;                               // Se estiver insere o valor do y passado por parâmetro
            xAxis << 0;                               // Insere o valor 0 para o eixo x
        } else {                                      // Se os vetores não forem vazios
            yAxis.push_back(y);                       // Insere o valor passado por parâmetro depois da ultima posição
            xAxis.push_back(x+(xAxis.last()));        // Insere o valor da ultima posição + 1 depois da ultima posição
        }
    } else {                                          // Se a variálvel de controle mais 1 não for menor que a quantidade de amostras
        yAxis.remove(0);                              // Remove o primeiro valor do eixo y
        xAxis.remove(0);                              // Remove o primeiro valor do eixo x
        yAxis.push_back(y);                           // Insere na ultima posicao do vetor o valor para y passado por parâmetro
        for (int i=0; i<xAxis.size(); i++) {          // Para o valor do eixo x é necessário realizar um deslocamento para a esquerda dos valores anteriores
            xAxis.replace(i,(xAxis.at(i)-1));         // Move o valor da posição atual - 1 para a posição anterior
        }
        xAxis.push_back(x+(xAxis.last()));            // Insere o valor da ultima posição mais 1
    }
*****************************************************************/
    customPlot->graph(0)->setData(xAxis,yAxis); // Seta os eixos x e y com os vetores alimentados anteriormente
    customPlot->xAxis->setLabel("Tempo (ms)"); // Seta o nome do eixo x
    customPlot->xAxis->setRange(0, qtdPontosGrafico); // Seta o tamanho do gráfico no eixo x
    customPlot->yAxis->setLabel("Variação da Tensão"); // Seta o nome do eixo y
    customPlot->yAxis->setRange(0,5); // Seta tamanho do gráfico no eixo y
    customPlot->replot(); // Pinta o gráfico

}

void Supervisorio::atualizaDados() {
    // Se a Porta serial foi configurada corretamente processa 1 valor
    if (isPortaSelecionada) {
        // Se o buffer com os valores lidos pela porta serial for maior que 0 processa 1 valor
        if (stringLida.size() > 0) {
            QString valorLido; // variável temporária que vai armazenar 1 valor do buffer

            int primeiroRegistro = stringLida.indexOf('['); // busca a primeira posição do [, identificador de início de um valor
            int ultimoRegistro = stringLida.indexOf(']'); // busca a primeira posição do ], identificador de fim de um valor

            // Se encontrou o identificador [ e encontrou o identificador ] e a posição o identificador ] é maior que a posição do identificador [
            if ((primeiroRegistro != -1) && (ultimoRegistro != -1) && (ultimoRegistro > primeiroRegistro)) {
                int j=0; // Variável que controla a posição na string temporária que o caractere atual será inserido

                // Percorre os caracteres entre os delimitadores [ e ] armazenando na variável temporária
                for (int i= primeiroRegistro+1; i<ultimoRegistro; i++) {
                    valorLido.insert(j,stringLida.at(i)); // Insere na variável temporária o caractere lido entre os delimitadores [] do buffer
                    j++;
                }

                // Converte o valor armazenado na variável temporária para float
                float valorConvertido = valorLido.toFloat();

                // Multiplica o valor convertido com o fator de conversão para gerar o valor entre 0 e 5
                valorConvertido *= fatorDeConversao;

                // Imprime o gráfico
                plotaGrafico(1,valorConvertido);

                // Armazena no buffer de gravação em arquivo o valor lido e convertido
                if (valorLido.size() > 0 && valorConvertido > 0) {
                    QString valorParaArquivo; // Variável temporária para converter o valor
                    valorParaArquivo.setNum(valorConvertido); // Converte de Float para String
                    stringArquivo += valorParaArquivo.toLatin1()+";\n"; // Incrementa o buffer com o a string convertida
                }
            }

            // Limpa as variáveis temporárias e remove do buffer o que já foi lido e convertido
            valorLido.clear(); // Limpa variável temporária usada para montar o valor

            // Tratamento de exceções para limpeza do buffer de leitura
            if (primeiroRegistro = -1)                                  // Se não encontrou o identificado [
                stringLida.remove(0,ultimoRegistro+1);                  // Exclui do buffer tudo que tem antes do delimitador ] pois houve alguma falha na comunicação
            else if (primeiroRegistro != -1 && ultimoRegistro != -1)    // Senão verifica se os dois delimitadores foram encontrados
                stringLida.remove(0,ultimoRegistro+1);                  // Se foram, exclui tudo que estava entre os delimitadores, incluindo os delimitadores
        }                                                               // Se não entrar em nenhuma das opções  é por que achou apenas o delimitador de inicio [
                                                                        // com isso o processamento acabou passando a leitura, então não limpa o buffer
                                                                        // e espera a próxima execução para ter ambos os delimitadores
    }
}

void Supervisorio::salvarEmArquivo() {
    // Solicita um diretório e um nome de arquivo para gravar o buffer com os valores convertidos
    diretorio = QFileDialog::getSaveFileName(this, tr("Abrir Arquivo"), QDir::currentPath(), tr("Files (*.txt)"));

    // Verifica se foi informado um diretório, se não foi, informa o usuário
    if (diretorio.size() > 0) {

        // Cria um arquivo no diretório
        arquivo = new QFile(diretorio);

        // Tenta abrir o arquivo, se conseguir grava o buffer com os valores convertidos, se não conseguir informa o usuário
        if (!arquivo->open(QIODevice::ReadWrite | QIODevice::Text)) {
            std::cout << "Não foi possível abrir o arquivo: " << diretorio.toStdString() << std::endl;
        } else {
            arquivo->write(stringArquivo); // Grava o buffer com os valores convertidos no arquivo
            arquivo->close(); // Fecha o arquivo
        }
    } else {
        std::cout << "Não foi possível localizar o diretório: "<< diretorio.toStdString() << std::endl;
    }
}

void Supervisorio::iniciaLeitura() {
    // Inicia os contadores para ler e processar os dados da porta serial
    stringLida.clear(); // Limpa o buffer de leitura
    atualizaTela->start(5); // Tempo de Amostragem
    leituraDaSerial->start(5); // Tempo de Leitura
}

void Supervisorio::paraLeitura() {
    // Para os contadores de leitura e processamento
    atualizaTela->stop();
    leituraDaSerial->stop();
    stringLida.clear(); // Limpa buffer de leitura
}

