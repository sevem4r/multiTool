#include "serialport.h"

#include <QSerialPortInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>

SerialPort::SerialPort( QWidget *parent ):
    parent( parent )
{

}

void SerialPort::updateSettings( QString com, int baud )
{
    currentSettings.name = com;
    currentSettings.baudRate = baud;
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = QSerialPort::Data8;
    currentSettings.parity = QSerialPort::NoParity;
    currentSettings.stopBits = QSerialPort::StopBits::OneStop;
    currentSettings.flowControl = QSerialPort::NoFlowControl;

}


bool SerialPort::openSerialPort()
{

    setPortName( currentSettings.name );
    setBaudRate( currentSettings.baudRate );
    setDataBits( currentSettings.dataBits );
    setParity( currentSettings.parity );
    setStopBits( currentSettings.stopBits );
    setFlowControl( currentSettings.flowControl );
    if( open( QIODevice::ReadWrite ) ){
        emit serialPortStatus( QMainWindow::tr( "Serial status: Connected to %1" ).arg( currentSettings.name ) );
        return true;
    }else{
        emit serialPortStatus( QMainWindow::tr( "Serial status: Open error" ) );
        return false;
    }
}

void SerialPort::closeSerialPort()
{
    if( isOpen() ){
        close();
        emit serialPortStatus( QMainWindow::tr( "Serial status: Disconnected" ) );
    }
}

//SLOT[0]
void SerialPort::uploadData( QByteArray data )
{
    qDebug() << "<<" << data.length() << data;

    if( isOpen() ){
        write( data );
    }
}

////SLOT[1]
//void SerialPort::uploadData( QString data ){
//    if( isOpen() ){
//        write( data.toUtf8() );
//    }
//}

