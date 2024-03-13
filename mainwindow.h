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


private:
    Ui::MainWindow *ui;

    QSerialPort *serial;

    void OnQSerialPort1Rx();

    uint8_t TX[256], payload[8],RX[256],indiceRX_r=0,indiceRX_t=0;
    volatile _rx ringRx;
    volatile _tx ringTx;
};
#endif // MAINWINDOW_H
