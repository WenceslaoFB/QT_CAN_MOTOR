#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QTime>
#include <QTimer>
#include <qmessagebox.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QByteArray>
//#include <QSerialPortInfo>
#include <QDebug>


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

    timerUSB = new QTimer(this);
    connect(timerUSB, &QTimer::timeout, this, &MainWindow::verificarYConectarUSB);
    timerUSB->start(1000); // Intervalo de tiempo en milisegundos

    serial = new QSerialPort(this);
    /*//serial->setPortName("COM4"); // Ajusta el nombre del puerto a tu puerto correcto.
        serial->setPortName("COM12");
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->open(QSerialPort::ReadWrite);
        serial->setDataTerminalReady(true);
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::OnQSerialPort1Rx);*/
    conectarMicro();

    inicio();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::conectarMicro(){
    // Enumerar los puertos disponibles y buscar tu dispositivo específico
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if ((info.description().contains("USB Serial Device")) && (info.serialNumber() == "")) {
            serial->setPort(info);
            serial->setBaudRate(QSerialPort::Baud115200); // Configura según tu dispositivo
            // Configura otros parámetros si es necesario

            if (serial->open(QSerialPort::ReadWrite)) {
                serial->setDataTerminalReady(true);
                qDebug() << "Conectado con éxito a" << info.portName();
                break; // Salir del bucle una vez conectado
            } else {
                qDebug() << "Error al abrir el puerto:" << serial->errorString();
            }
        }
    }
}

void MainWindow::verificarYConectarUSB() {

    if (serial->isOpen()) {
        // El dispositivo ya está conectado, no es necesario hacer nada.
        qDebug() << "Dispositivo ya conectado.";
    } else {
        // Intentar reconectar
        qDebug() << "Dispositivo desconectado. Intentando reconectar...";
        conectarMicro();
    }
    if (!serial->isOpen() || serial->error() == QSerialPort::ResourceError) {
        qDebug() << "Problema detectado en la conexión. Intentando reconectar...";
        serial->close(); // Cierra el puerto si está abierto
        conectarMicro(); // Intenta reconectar
    }
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

    //ui->lineEdit->setText(strhex);
}

void MainWindow::inicio(){

    crearArrayCMD(POSITION_MODE,ID_M_DIREC);
    EnviarComando(0x0B, POSITION_MODE, payloadCAN);

    crearArrayCMD(VELOCITY_MODE,ID_M_VEL);
    EnviarComando(0x0B, VELOCITY_MODE, payloadCAN);
}

void MainWindow::crearArrayCMD(uint8_t cmd, uint8_t id){

    switch(cmd){ //la data se guarda en el mensaje poniendo el byte menos sig primero

    case VELOCITY_MODE: //coloca el motor en modo velocidad
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2F;
        payloadCAN[2] = 0x60;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x03;
        payloadCAN[6] = 0x00;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
    break;
    case POSITION_MODE://coloca el motor en modo posicion
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2F;
        payloadCAN[2] = 0x60;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x01;
        payloadCAN[6] = 0x00;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
    break;
    case ENABLE://envia la señal que habilita el motor
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
    case DISABLE://envia la señal que deshabilita el motor
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
    case TARGET_SPEED://envia la velocidad a la que se desea que vaya el motor
        payloadCAN[0] = id;
        payloadCAN[1] = 0x23;
        payloadCAN[2] = 0xFF;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = velocidad_cmd.u8[0];
        payloadCAN[6] = velocidad_cmd.u8[1];
        payloadCAN[7] = velocidad_cmd.u8[2];
        payloadCAN[8] = velocidad_cmd.u8[3];
        break;
    case TARGET_POS://envia la posicion a la que se desea que se desplace el motor
        payloadCAN[0] = id;
        payloadCAN[1] = 0x23;
        payloadCAN[2] = 0x7A;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = pos_cmd.u8[0];
        payloadCAN[6] = pos_cmd.u8[1];
        payloadCAN[7] = pos_cmd.u8[2];
        payloadCAN[8] = pos_cmd.u8[3];
        break;
    case INVERTIR_1://invierte el giro del motor
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2F;
        payloadCAN[2] = 0x60;
        payloadCAN[3] = 0x7E;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x01;
        payloadCAN[6] = 0x00;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
        break;
    case INVERTIR_2://vuelve a invertir el giro
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2F;
        payloadCAN[2] = 0x60;
        payloadCAN[3] = 0x7E;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x00;
        payloadCAN[6] = 0x00;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
        break;
    case READY_POS://PONER MOTOR DIR EN ALWAYS READY, necesario para indicar que el motor esta listo para recibir posicion
        payloadCAN[0] = id;
        payloadCAN[1] = 0x2B;
        payloadCAN[2] = 0x40;
        payloadCAN[3] = 0x60;
        payloadCAN[4] = 0x00;
        payloadCAN[5] = 0x3F;
        payloadCAN[6] = 0x10;
        payloadCAN[7] = 0x00;
        payloadCAN[8] = 0x00;
        break;
    default:
    break;
    }

}
void MainWindow::EnviarComando(uint8_t length, uint8_t cmd, uint8_t payloadCAN[]){

    TX[0] = 'U';
    TX[1] = 'N';
    TX[2] = 'E';
    TX[3] = 'R';
    TX[4] = length;
    TX[5] = 0x00;
    TX[6] = ':';
    TX[7] = cmd;
    TX[8] = payloadCAN[0];
    TX[9] = payloadCAN[1];
    TX[10] = payloadCAN[2];
    TX[11] = payloadCAN[3];
    TX[12] = payloadCAN[4];
    TX[13] = payloadCAN[5];
    TX[14] = payloadCAN[6];
    TX[15] = payloadCAN[7];
    TX[16] = payloadCAN[8];


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


void MainWindow::on_EN_DIR_pressed()
{
    crearArrayCMD(ENABLE,ID_M_DIREC);
    EnviarComando(0x0B, ENABLE, payloadCAN);
}


void MainWindow::on_DIS_DIR_pressed()
{
    crearArrayCMD(DISABLE,ID_M_DIREC);
    EnviarComando(0x0B, DISABLE, payloadCAN);
}


void MainWindow::on_EN_VEL_pressed()
{
    crearArrayCMD(ENABLE,ID_M_VEL);
    EnviarComando(0x0B, ENABLE, payloadCAN);
}


void MainWindow::on_DIS_VEL_pressed()
{
    crearArrayCMD(DISABLE,ID_M_VEL);
    EnviarComando(0x0B, DISABLE, payloadCAN);
}



void MainWindow::on_B_500_RPM_pressed()// la velocidad se calcula con la ecuacion "DEC=[(rpm* 512 * Encoder_Resolution)/1875]"
{
    vel_aux = ((500 * 512) * (10000.0/1875));
    velocidad_cmd.u32 = (uint32_t)vel_aux;

    //QString strTest2 = QString("%1").arg(vel_aux, 0, 'f', 2);
    //QString strTest2 = QString("%1").arg(velocidad_cmd.u32, 8, 16, QChar('0')).toUpper();
    //ui->text_vel->setText(strTest2);

    crearArrayCMD(TARGET_SPEED,ID_M_VEL);
    EnviarComando(0x0B, TARGET_SPEED, payloadCAN);
}


void MainWindow::on_B_1000_RPM_pressed()
{
    vel_aux = ((1000 * 512) * (10000.0/1875));
    velocidad_cmd.u32 = (uint32_t)vel_aux;

    crearArrayCMD(TARGET_SPEED,ID_M_VEL);
    EnviarComando(0x0B, TARGET_SPEED, payloadCAN);
}


void MainWindow::on_B_1500_RPM_pressed()
{
    vel_aux = ((1500 * 512) * (10000.0/1875));
    velocidad_cmd.u32 = (uint32_t)vel_aux;

    crearArrayCMD(TARGET_SPEED,ID_M_VEL);
    EnviarComando(0x0B, TARGET_SPEED, payloadCAN);
}


void MainWindow::on_B_2000_RPM_pressed()
{
    vel_aux = ((2000 * 512) * (10000.0/1875));
    velocidad_cmd.u32 = (uint32_t)vel_aux;

    crearArrayCMD(TARGET_SPEED,ID_M_VEL);
    EnviarComando(0x0B, TARGET_SPEED, payloadCAN);
}


void MainWindow::on_SLID_RPM_valueChanged(int RPM_slid)
{
    ui->spin_rpm->setValue(RPM_slid);
    //buscar forma de pasar el numero del slider a comando
    vel_aux = ((RPM_slid * 512) * (10000.0/1875));
    velocidad_cmd.u32 = (uint32_t)vel_aux;
    vel_slid = vel_aux;

    //QString strTest2 = QString("%1").arg(velocidad_cmd.u32, 8, 16, QChar('0')).toUpper();
    //ui->text_vel->setText(strTest2);
}


void MainWindow::on_POS_BUT_pressed()
{
    //tomar valor de lineEdit y pasarlo a comando
    pos_ing.f = (ui->LINE_POS->text()).toFloat();

    pos_aux = ((pos_ing.f/5.49316)*1000);

    pos_cmd.u32 = (uint32_t)pos_aux;

    //QString strTest = QString("%1").arg(pos_cmd.u32, 8, 16, QChar('0')).toUpper();

    //ui->text_pos->setText(strTest);

    crearArrayCMD(POSITION_MODE,ID_M_DIREC);
    EnviarComando(0x0B, 0x00, payloadCAN);
}


void MainWindow::on_vel_slid_bot_pressed()
{
    velocidad_cmd.u32 = (uint32_t)vel_slid;

    //QString strTest2 = QString("%1").arg(velocidad_cmd.u32, 8, 16, QChar('0')).toUpper();
    //ui->text_vel->setText(strTest2);
    crearArrayCMD(TARGET_SPEED,ID_M_VEL);
    EnviarComando(0x0B, TARGET_SPEED, payloadCAN);
}


void MainWindow::on_but_CMD_pressed()
{
    QString Index_Str = ui->line_Index->text();

    QString SubIndex_Str = ui->line_subIndex->text();

    //ui->line->setText(Index_Str + SubIndex_Str);

    switch(ui->box_ID->currentIndex()){
    case 0:
        ui->line->setText("01 " + Index_Str + SubIndex_Str);
    break;
    case 1:
        ui->line->setText("07 " + Index_Str + SubIndex_Str);
    break;
    }

    //bool ok;

    //int hex = Index_Str.toInt(&ok, 16);


/*
    if(Index_Str.length() % 2 != 0){
        //return 1;
    }

    QByteArray hexNs;

    for(int i = 0; i < Index_Str.length(); i+=2){
        QString hex = Index_Str.mid(i,2);

        bool ok;
        unsigned int hexN = hex.toUint(&ok, 16);

        if(!ok){
            //return 1;
        }

        hexNs.append(static_cast<char>(hexN));
    }
    //QString strTest2 = QString("%1").arg(hexNs, 8, 16, QChar('0')).toUpper();
    ui->line->setText(strTest2);*/

}

