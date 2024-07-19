#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

class SerialPort : public QSerialPort
{
    Q_OBJECT

    QWidget * parent;

    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

    Settings currentSettings;

public:

    explicit SerialPort( QWidget * parent = nullptr );

    void updateSettings( QString com, int baud );

    bool openSerialPort();
    void closeSerialPort();

signals:
    void serialPortStatus( QString status );

public slots:
    void uploadData( QByteArray data );
//    void uploadData( QString data );
};

#endif // SERIALPORT_H
