#include "hexloader.h"
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>

Hexloader::Hexloader( QWidget *parent ):
    parent( parent ),
    timer( new QTimer() ),
    startUploadHex( false ),
    getInfo( false ),
    pageCnt( 0 ),
    hexLoaderState( 0 )
{
    connect( timer, &QTimer::timeout, this, &Hexloader::uploadTimeout );
}

void Hexloader::uploadProcess(const QString& data)
{
    hexLoaderData += data;


    if(getInfo){

        switch(hexLoaderState){

        case 0:
            if(hexLoaderData.contains("?")){    // ERROR - timeout, data not contained "?" -> RESTART
                emit upload("ui");
                hexLoaderState++;
                timer->start(5000);
            }
            hexLoaderData.clear();
            break;

        case 1:
            if(hexLoaderData.contains("*\r\n")){    // ERROR - timeout, data not contained "@" -> RESTART

                char sep = ',';
                QList<QString> list = hexLoaderData.split(sep);

                int len = list.at(0).length() - 3;
                QString str = list.at(0).mid(3, len);

                emit logClear();
                emit logColor(QColor(49, 79, 249));
                emit logAppend("uC: " + list.at(2));
                emit logAppend("Clock: " + list.at(3) + "Hz");
                emit logAppend("Page Size: " + str);
                emit logAppend("Signature: " + list.at(1));

                emit logButton(0);

                hexLoaderState = 0;
                getInfo = false;
                hexLoaderData.clear();
                timer->start(5000);
            }
            break;

        }

    }
    else{
        switch(hexLoaderState){

        case 0:
            if(hexLoaderData.contains("?")){    // ERROR - timeout, data not contained "?" -> RESTART
                hexLoaderState++;

                emit upload("uw");
                emit logColor(QColor( 49, 79, 249));
                emit logSet("Programming..");
                timer->start(5000);
            }
            hexLoaderData.clear();
            break;

        case 1:
            if(hexLoaderData.contains("@")){    // ERROR - timeout, data not contained "@" -> RESTART
                hexLoaderState++;

                if(pageCnt < pageToUpload){
                    emit upload("s");
                }
                else{
                    emit upload("\0");
                }

                timer->start(5000);
            }
            hexLoaderData.clear();

        [[clang::fallthrough]]; case 2:
            {
                if(pageCnt < pageToUpload){
                    hexLoaderState--;

                    QByteArray binPage = hexPage.mid(pageCnt * 64, 64);

                    pageCnt++;

                    emit upload(binPage);
                    emit loadingProgress(pageCnt);

                    qDebug() << pageCnt << "/" << pageToUpload << "size:" << binPage.size();
                }
                else{
                    hexLoaderState = 0;
                    pageCnt = 0;
                    startUploadHex = false;

                    emit logColor(QColor(41, 165, 0));
                    emit logSet("uC Programmed");
                    emit logButton(0);
                    timer->stop();
                }

//                QByteArray binPage = hexPage.mid(pageCnt * 64, 64);
//                emit upload(binPage);
//                emit loadingProgress(pageCnt);
//                pageCnt++;

//                qDebug() << pageCnt << "/" << pageToUpload << "size:" << binPage.size();

//                if(pageCnt > pageToUpload){
//                    hexLoaderState = 0;
//                    pageCnt = 0;
//                    startUploadHex = false;

//                    emit logColor(QColor(41, 165, 0));
//                    emit logSet("uC Programmed");
//                    emit logButton(0);
//                    timer->stop();
//                }
//                else{
//                    hexLoaderState--;
//                }

                break;
             }
        }
    }
}

void Hexloader::restart(){
    hexLoaderState = 0;
    pageCnt = 0;
    startUploadHex = false;
    hexLoaderData.clear();

    emit logColor( QColor( 206, 90, 18 ) );
    emit logSet( "Upload timeout.." );
    emit logButton( 0 );
    timer->stop();
}

bool Hexloader::status()
{
    return startUploadHex || getInfo;
}

void Hexloader::uploadHexFile( bool serialStatus )
{
    if( !getInfo ){

        if( !serialStatus ){
            emit logColor( QColor( 206, 90, 18 ) );
            emit logSet( "Open COM Port.." );
            emit logButton( 0 );
            return;
        }

        if( fileName == "" ){
            emit logColor( QColor( 206, 90, 18 ) );
            emit logSet( "Choose HEX File.." );
            emit logButton( 0 );
            return;
        }

        reloadHexFile( fileName );
        startUploadHex = true;
        timer->start( 5000 );
        emit logColor( QColor( 206, 90, 18 ) );
        emit logSet( "Press uC reset.." );
        emit logButton( 1 );
    }
}

void Hexloader::uCinfo( bool serialStatus )
{
    if( !startUploadHex ){

        if( !serialStatus ){
            emit logColor( QColor( 206, 90, 18 ) );
            emit logSet( "Open COM Port.." );
            emit logButton( 0 );
            return;
        }

        getInfo = true;
        emit logColor( QColor( 206, 90, 18 ) );
        emit logSet( "Press uC reset.." );
        emit logButton( 1 );
    }
}

void Hexloader::reloadHexFile(const QString& path){
    emit loadingFilePath(path);

    file.setFileName(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // ERROR - cannot open file
        QMessageBox::critical(parent, tr("Error"), file.errorString());
        return;
    }

    hexPage.clear();
    complementPage.clear();

    while(!file.atEnd()){
        QByteArray line = file.readLine();
        QByteArray data;

        // intel hex
        // :aabbbbcc[data 0..32]dd\n
        // : - start
        // a - bytes number
        // b - address
        // c - record type
        // d - CRC

        qDebug() << line << line.length();

        // data length
        int dataLen = line.length() - 12;

        if(dataLen){

            // get data
            data = line.mid(9, dataLen);

            // iterate through bytes (every two chars in hex)
            for(int i = 0; i < (dataLen / 2); i++){
                bool byteConvertStatus = false;

                // get byte in hex
                QByteArray byteInHex = data.mid(i * 2, 2);

                // convert hex byte to bin
                char byte = static_cast<char>(byteInHex.toInt(&byteConvertStatus, 16));

                hexPage.append(byte);

                if(complementPage.size() < 64){
                    complementPage.append(byte);
                }
            }
        }
    }

    int size = 64 - (hexPage.size() % 64);

    for(int i = 0; i < size; i++){
//        hexPage.append(complementPage[i]);
        hexPage.append('\0');
    }

    pageToUpload = hexPage.size() / 64;
    emit loadingProgress(0);
    emit loadingProgressMax(pageToUpload);

    file.close();

    // OK - file uploaded
}

//SLOT[1]
void Hexloader::loadHexFile()
{
    QStringList filesList = QFileDialog::getOpenFileNames( parent, tr( "Open Hex File" ), "", tr( "Hex Files (*.hex)" ), Q_NULLPTR, QFileDialog::DontUseNativeDialog );
    if( filesList.isEmpty() ){
        return;
    }

    fileName = filesList[0];
    reloadHexFile( fileName );
}

//SLOT[0] private
void Hexloader::uploadTimeout(){
    restart();
}
