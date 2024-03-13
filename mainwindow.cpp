#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QTime>
#include <QTimer>
#include <qmessagebox.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ringTx.buf=TX;
    ringRx.buf=RX;
    ringTx.iW=0;
    ringTx.iR=0;
    ringRx.iW=0;
    ringRx.iR=0;
    ringRx.header=0;

    serial = new QSerialPort(this);
    //serial->setPortName("COM4"); // Ajusta el nombre del puerto a tu puerto correcto.
        serial->setPortName("COM6");
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->open(QSerialPort::ReadWrite);
        serial->setDataTerminalReady(true);
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::OnQSerialPort1Rx);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnQSerialPort1Rx(){

    QByteArray data = serial->readAll();
    if (data.isEmpty()) {
        return;
    }

    QString strhex;
    for (int i = 0; i < data.size(); i++)
        strhex = strhex + QString("%1").arg(static_cast<quint8>(data[i]), 2, 16, QChar('0')).toUpper();

    //strData = QString::fromLatin1(data);

    for (int i = 0; i < data.size(); i++)
    {
        ringRx.buf[ringRx.iW] = data[i];
        ringRx.iW++;
        //Decode();
    }

    ui->lineEdit->setText(strhex);
}

void MainWindow::crearArrayCMD(uint8_t cmd, uint8_t id){

    switch(cmd){

    case ENABLE:
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2B;
        payloadCAN[2] = 0x40;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x0F;
        payloadCAN[6] = 0x00;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
    break;
    case DISABLE:
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2B;
        payloadCAN[2] = 0x40;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x06;
        payloadCAN[6] = 0x00;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
        break;
    default:
    break;
    }

}
void MainWindow::EnviarComando(uint8_t length, uint8_t cmd, uint8_t payloadCAN[]){

    static uint8_t indTX = 0;

    TX[0] = 'U';
    TX[1] = 'N';
    TX[2] = 'E';
    TX[3] = 'R';
    TX[4] = length;
    TX[5] = 0x00;
    TX[6] = ':';
    /*
    switch (cmd)
    {
    case CMD_SENSORS:
    break;
    case CMD_ALIVE:
    TX[7] = CMD_ALIVE;
    break;
    default:
    // Manejar el error de comando no reconocido
    // ...
    return;
    }*/
    TX[7] = payloadCAN[0];
    TX[8] = payloadCAN[1];
    TX[9] = payloadCAN[2];
    TX[10] = payloadCAN[3];
    TX[11] = payloadCAN[4];
    TX[12] = payloadCAN[5];
    TX[13] = payloadCAN[6];
    TX[14] = payloadCAN[7];
    TX[15] = payloadCAN[8];


    // Calcular el checksum
    uint8_t cks = 0;
    cks=0;

    for(int i=0; i<TX[4]+6; i++){
    cks ^= TX[i];
    }
    TX[TX[4]+6] = cks;

    if(serial->isOpen()){
    serial->write((char*)TX, 7 + TX[4]);
    }
}


void MainWindow::on_pushButton_2_pressed()
{
    crearArrayCMD(ENABLE,ID_M_DIREC);
    EnviarComando(0x09, 0x00, payloadCAN);
}


void MainWindow::on_pushButton_10_pressed()
{
    crearArrayCMD(DISABLE,ID_M_DIREC);
    EnviarComando(0x09, 0x00, payloadCAN);
}

