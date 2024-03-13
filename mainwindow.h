#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct {
    uint8_t *buf;
    uint8_t iW;
    uint8_t iR;
    uint8_t header;
    uint16_t timeout;
    uint8_t nBytes;
    uint8_t iData;
    uint8_t cks;
}__attribute__((packed, aligned(1))) _rx;

typedef struct {
    uint8_t *buf;
    uint8_t iW;
    uint8_t iR;
    uint8_t cks;
}__attribute__((packed, aligned(1))) _tx;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_2_pressed();

    void on_pushButton_10_pressed();

private:
    Ui::MainWindow *ui;

    QSerialPort *serial;

    void OnQSerialPort1Rx();
    void crearArrayCMD(uint8_t cmd, uint8_t id);
    void EnviarComando(uint8_t length, uint8_t cmd, uint8_t payloadCAN[]);

    uint8_t TX[256], payloadCAN[256],RX[256],indiceRX_r=0,indiceRX_t=0;
    volatile _rx ringRx;
    volatile _tx ringTx;

    #define ID_M_DIREC 10
    #define ID_M_VEL 20

    #define ENABLE 0x01
    #define DISABLE 0x02
    #define INVERTIR 0x03
};
#endif // MAINWINDOW_H
