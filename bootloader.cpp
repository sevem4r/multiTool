#include "bootloader.h"
#include <QFile>
#include <QProcess>
#include <QDebug>

Bootloader::Bootloader( QWidget *parent ):
    parent( parent )
{

}

bool Bootloader::copyMakefile()
{
    QFile file_template( "Bootloader/templates/makefile" );
    if( !file_template.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return false;

    QFile file( "Bootloader/makefile" );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return false;


    QTextStream in( &file_template );
    QTextStream out( &file );
    QString line = in.readLine();
    while( !line.isNull() ){
        if( line.contains( "MCU =" ) ) line = "MCU = " + uC;
        else if( line.contains( "F_CPU =" ) ) line = "F_CPU = " + Frequency;
        else if( line.contains( "BAUD_RATE =" ) ) line = "BAUD_RATE = " + Baudrate;
        else if( line.contains( "BLS_START =" ) ) line = "BLS_START = " + BLS;
        else if( line.contains( "BOOT_WAIT =" ) ) line = "BOOT_WAIT = " + Wait;
        out << line << "\n";
        line = in.readLine();
    }

    file_template.close();
    file.close();

    return true;
}

bool Bootloader::copyMain()
{
    QFile file_template( "Bootloader/templates/main.c" );
    if( !file_template.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return false;

    QFile file( "Bootloader/main.c" );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
        return false;


    QTextStream in( &file_template );
    QTextStream out( &file );
    QString line = in.readLine();
    while( !line.isNull() ){
        out << line << "\n";
        line = in.readLine();
    }

    file_template.close();
    file.close();

    return true;
}

// SLOT[1]
void Bootloader::setuC( QString uc )
{
    uC = uc;
    qDebug() << uC;
}

// SLOT[2]
void Bootloader::setFrequency( QString frequency )
{
    Frequency = frequency;
    qDebug() << Frequency;
}

// SLOT[3]
void Bootloader::setBaudrate( QString baudrate )
{
    Baudrate = baudrate;
    qDebug() << Baudrate;
}

// SLOT[4]
void Bootloader::setBLS( QString bls )
{
    BLS = bls;
    qDebug() << BLS;
}

// SLOT[5]
void Bootloader::setWait( QString wait )
{
    Wait = wait;
    qDebug() << Wait;
}

// SLOT[6]
void Bootloader::compile()
{
    if( !copyMakefile() ) return;
    if( !copyMain() ) return;

    compilePrcocess.setWorkingDirectory( "Bootloader" );
    compilePrcocess.start( "make" );
    compilePrcocess.waitForFinished();

    emit compileOutput( compilePrcocess.readAllStandardOutput() );
}

// SLOT[7]
void Bootloader::upload()
{
    uploadProcess.setWorkingDirectory( "Bootloader" );
    QStringList arguments;
//    arguments << "-p" << uC << "-c" << "usbasp" << "-U" << "flash:w:Bootloader.hex:i" << "-F" << "-P" << "usb";
//    arguments << "-p" << uC << "-c" << "usbasp" << "-U" << "flash:w:Bootloader.hex:a" << "-F" << "-P" << "usb";
    arguments << "-p" << uC << "-c" << "usbasp" << "-U" << "flash:w:Bootloader.hex:a";
    uploadProcess.start( "avrdude", arguments );
    uploadProcess.waitForFinished();

    emit uploadOutput( uploadProcess.readAllStandardError() );
}
