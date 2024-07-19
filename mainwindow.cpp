#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QSpinBox>

/*
 * v1.02
 * - hexloader timeout
 * - hexloader auto file reopen
 * - hexloader reset commend line
 * - hexloader file dialog empty line assert
 * - serial connect/disconnect assert
 * - terminal separate eol
 *
 * v1.03
 * - expression tab
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow ),
    status( new QLabel ),
    loggedSamples(0)
{
    ui->setupUi( this );
    ui->statusBar->addWidget( status );

    QPixmap pix;
    if( pix.load( ":/img/link2.png" ) ){
        ui->lblLink->setPixmap( pix );
    }


    serial = new SerialPort();
    graphPlot = dynamic_cast<Graph *>( ui->tabWidgetGraph->widget( 0 ) );
    expression = new Expression();
    hexloader = new Hexloader( this );
    bootloader = new Bootloader( this );
    datalogger = new Datalogger( this );

    //CONSOLE
    consInput = ui->tabWidget->widget( 0 )->findChild<ConsoleInput *>( "consInput" );
    consOutput = ui->tabWidget->widget( 0 )->findChild<ConsoleOutput *>( "consOutput" );
    consExpression = ui->tabWidget->widget( 2 )->findChild<ConsoleOutput *>( "consExpression" );
    connect( consInput, &ConsoleInput::getData, serial, &SerialPort::uploadData );
    connect( ui->rbtnOneLineMode, &QRadioButton::clicked, consOutput, &ConsoleOutput::setOneLineMode );
    connect( ui->btnClearOutputConsol, &QPushButton::clicked, consOutput, &ConsoleOutput::clearConsole );
    connect( ui->btnClearInputConsol, &QPushButton::clicked, consInput, &ConsoleInput::clearConsole );
    connect( ui->leEndLine, &QLineEdit::textChanged, consOutput, &ConsoleOutput::setEndOfLine );
    connect( ui->leEolIn, &QLineEdit::textChanged, consInput, &ConsoleInput::setEndOfLine );


    //SERIAL
    connect( ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::serialFillComPorts );
    connect( ui->btnConnect, &QPushButton::clicked, this, &MainWindow::serialManagePort );
    connect( ui->cbCom, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, &MainWindow::serialShowPortInfo );
    connect( serial, &SerialPort::serialPortStatus, status, &QLabel::setText );
    connect( serial, &SerialPort::readyRead, this, &MainWindow::serialGetData );
    connect ( ui->leEndLine, &QLineEdit::textChanged, this, &MainWindow::serialFlushData );
//    connect( serial, &SerialPort::errorOccurred, this, &MainWindow::handleError );
    serialFillBaudRates();
    serialFillComPorts();
    status->setText( "Serial status: Disconnected" );

    //GRAPH
    connect( ui->btnApply1, &QPushButton::clicked, this, &MainWindow::graphApplyPlotParam );
    connect( consOutput, &ConsoleOutput::putOnGraph, graphPlot, &Graph::plotData );
    connect( ui->rbUseGraph, &QRadioButton::clicked, graphPlot, &Graph::useGraph );
    graphFillPlotParameters();

    //EXPRESSION
    connect(consOutput, &ConsoleOutput::putOnGraph, expression, &Expression::prepare);
    connect(expression, &Expression::ready, this, &MainWindow::expressionReady);
    connect(ui->leExpression, &QLineEdit::textChanged, expression, &Expression::update_expression);

    //HEXLOADER
    connect( ui->btnLoadHexFile, &QPushButton::clicked, hexloader, &Hexloader::loadHexFile );
    connect( hexloader, &Hexloader::loadingFilePath, ui->leHexFile, &QLineEdit::setText );
    connect( hexloader, &Hexloader::loadingProgress, ui->pbUploadHex, &QProgressBar::setValue );
    connect( hexloader, &Hexloader::loadingProgressMax, ui->pbUploadHex, &QProgressBar::setMaximum );
    connect( hexloader, &Hexloader::upload, serial, &SerialPort::uploadData );
    connect( hexloader, &Hexloader::logClear, ui->pteInfo, &QPlainTextEdit::clear );
    connect( hexloader, &Hexloader::logAppend, ui->pteInfo, &QPlainTextEdit::appendPlainText );
    connect( hexloader, &Hexloader::logSet, ui->pteInfo, &QPlainTextEdit::setPlainText );
    connect( hexloader, &Hexloader::logButton, ui->btnUploadHex, &QPushButton::setChecked );
    connect( hexloader, &Hexloader::logColor, this, &MainWindow::hexloaderLogColor );

    connect( ui->btnUploadHex, &QPushButton::clicked, this, &MainWindow::hexloaderUploadHexFile );
    connect( ui->btnInfo, &QPushButton::clicked, this, &MainWindow::hexloaderuCinfo );
    connect( ui->btnReset, &QPushButton::clicked, this, &MainWindow::hexloaderReset );

    //BOOTLOADER
    bootloaderFillOptions();
    connect( ui->cbBootuC, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [=]( int index ){ bootloader->setuC( ui->cbBootuC->itemData( index ).toString() ); } );
    connect( ui->cbBootFreq, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [=]( int index ){ bootloader->setFrequency( ui->cbBootFreq->itemData( index ).toString() ); } );
    connect( ui->cbBootBaud, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [=]( int index ){ bootloader->setBaudrate( ui->cbBootBaud->itemData( index ).toString() ); } );
    connect( ui->cbBootBLS, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [=]( int index ){ bootloader->setBLS( ui->cbBootBLS->itemData( index ).toString() ); } );
    connect( ui->cbBootWait, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, [=]( int index ){ bootloader->setWait( ui->cbBootWait->itemData( index ).toString() ); } );
    connect( ui->btnCompile, &QPushButton::clicked, bootloader, &Bootloader::compile );
    connect( ui->btnUpload, &QPushButton::clicked, bootloader, &Bootloader::upload );
    connect( bootloader, &Bootloader::compileOutput, ui->bootConsole, &QPlainTextEdit::insertPlainText );
    connect( bootloader, &Bootloader::uploadOutput, ui->bootConsole, &QPlainTextEdit::insertPlainText );
    connect( ui->bootConsole, &QPlainTextEdit::textChanged, this, [=](){ QScrollBar *bar = ui->bootConsole->verticalScrollBar(); bar->setValue( bar->maximum() ); } );

    //DATALOGGER
    connect( ui->btnLogPath, &QPushButton::clicked, datalogger, &Datalogger::setLogFile );
    connect( ui->btnLogStart, &QPushButton::clicked, this, &MainWindow::dataloggerStart );
    connect( datalogger, &Datalogger::logFilePath, ui->leLog, &QLineEdit::setText );

//    connect( ui->btnSetDLoggerFile, &QPushButton::clicked, datalogger, &Datalogger::setLogFile );
//    connect( ui->btnStartDLogger, &QPushButton::clicked, this, &MainWindow::dataloggerStart );
//    connect( consOutput, &ConsoleOutput::putInFile, datalogger, &Datalogger::log );
//    connect( datalogger, &Datalogger::logFilePath, ui->leDLoggerFile, &QLineEdit::setText );
//    connect( datalogger, &Datalogger::loadingProgress, ui->pbLogLines, &QProgressBar::setValue );
//    connect( datalogger, &Datalogger::loadingProgressMax, ui->pbLogLines, &QProgressBar::setMaximum );
//    connect( datalogger, &Datalogger::loadingEnd, this, &MainWindow::dataloggerStop );
//    connect( ui->sbLogLines, QOverload<int>::of( &QSpinBox::valueChanged ), datalogger, &Datalogger::logLines );

}

MainWindow::~MainWindow()
{
    delete ui;
    delete status;
    delete serial;
    delete consInput;
    delete consOutput;
    delete graphPlot;
    delete expression;
    delete hexloader;
    delete datalogger;
}


void MainWindow::serialGetData()
{
    serialData = serial->readAll();

    qDebug() << ">>" << serialData;

    if( hexloader->status() ){
        hexloader->uploadProcess( serialData );

    }else{
        consOutput->put( serialData );
        if(datalogger->logStatus()){
            datalogger->logString(serialData, Datalogger::EoL::No);

            ui->lblLogSampels->setText("Samples: " + QString::number(loggedSamples));

            loggedSamples++;
        }
    }
    serialData.clear();
}

//void MainWindow::handleError( QSerialPort::SerialPortError error )
//{
//    if( error == QSerialPort::ResourceError ){
//        QMessageBox::critical( this, tr( "Critical Error" ), serial->errorString() );
//        serial->closeSerialPort();
//        ui->btnConnect->setText( "Connect" );
//        ui->btnConnect->setChecked( 0 );
//    }
//}


//EXPRESSION
void MainWindow::expressionReady(const QString& value)
{
    consExpression->put(value + '\n');
}

//HEXLOADER SLOT[1]
void MainWindow::hexloaderLogColor( QColor color )
{
    QPalette p = ui->pteInfo->palette();
    p.setColor( QPalette::Text, color );
    ui->pteInfo->setPalette( p );
}

//HEXLOADER SLOT[2]
void MainWindow::hexloaderUploadHexFile()
{
    hexloader->uploadHexFile( serial->isOpen() );
}

//HEXLOADER SLOT[3]
void MainWindow::hexloaderuCinfo()
{
    hexloader->uCinfo( serial->isOpen() );
}

//HEXLOADER SLOT [4]
void MainWindow::hexloaderReset()
{
    QByteArray cmd = ui->leReset->text().toUtf8() + '\r';
    serial->uploadData( cmd );
}

//BOOTLOADER
void MainWindow::bootloaderFillOptions()
{
    ui->cbBootuC->addItem( QStringLiteral( "ATmega8" ), "atmega8" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega8A" ), "atmega8a" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega16" ), "atmega16" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega16A" ), "atmega16a" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega32" ), "atmega32" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega32A" ), "atmega32a" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega64" ), "atmega64" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega64A" ), "atmega64a" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega88" ), "atmega88" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega88P" ), "atmega88p" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega88PA" ), "atmega88pa" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega88PB" ), "atmega88pb" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega328" ), "atmega328" );
    ui->cbBootuC->addItem( QStringLiteral( "ATmega328P" ), "atmega328p" );
    ui->cbBootuC->setCurrentIndex( 0 );
    bootloader->setuC( ui->cbBootuC->itemData( 0 ).toString() );

    ui->cbBootFreq->addItem( QStringLiteral( "1 000 000" ), "1000000" );
    ui->cbBootFreq->addItem( QStringLiteral( "2 000 000" ), "2000000" );
    ui->cbBootFreq->addItem( QStringLiteral( "4 000 000" ), "4000000" );
    ui->cbBootFreq->addItem( QStringLiteral( "8 000 000" ), "8000000" );
    ui->cbBootFreq->addItem( QStringLiteral( "11 059 200" ), "11059200" );
    ui->cbBootFreq->addItem( QStringLiteral( "16 000 000" ), "16000000" );
    ui->cbBootFreq->setCurrentIndex( 0 );
    bootloader->setFrequency( ui->cbBootFreq->itemData( 0 ).toString() );

    ui->cbBootBaud->addItem( QStringLiteral( "9600" ), "9600" );
    ui->cbBootBaud->addItem( QStringLiteral( "19200" ), "19200" );
    ui->cbBootBaud->addItem( QStringLiteral( "38400" ), "38400" );
    ui->cbBootBaud->addItem( QStringLiteral( "57600" ), "57600" );
    ui->cbBootBaud->addItem( QStringLiteral( "115200" ), "115200" );
    ui->cbBootBaud->addItem( QStringLiteral( "230400" ), "230400" );
    ui->cbBootBaud->addItem( QStringLiteral( "250000" ), "250000" );
    ui->cbBootBaud->addItem( QStringLiteral( "500000" ), "500000" );
    ui->cbBootBaud->setCurrentIndex( 0 );
    bootloader->setBaudrate( ui->cbBootBaud->itemData( 0 ).toString() );

    ui->cbBootBLS->addItem( QStringLiteral( "0x1E00" ), "0x1E00" );
    ui->cbBootBLS->addItem( QStringLiteral( "0x3E00" ), "0x3E00" );
    ui->cbBootBLS->addItem( QStringLiteral( "0x7E00" ), "0x7E00" );
    ui->cbBootBLS->addItem( QStringLiteral( "0xFE00" ), "0xFE00" );
    ui->cbBootBLS->setCurrentIndex( 0 );
    bootloader->setBLS( ui->cbBootBLS->itemData( 0 ).toString() );

    ui->cbBootWait->addItem( QStringLiteral( "1" ), 1 );
    ui->cbBootWait->addItem( QStringLiteral( "2" ), 2 );
    ui->cbBootWait->addItem( QStringLiteral( "3" ), 3 );
    ui->cbBootWait->addItem( QStringLiteral( "4" ), 4 );
    ui->cbBootWait->addItem( QStringLiteral( "5" ), 5 );
    ui->cbBootWait->setCurrentIndex( 0 );
    bootloader->setWait( ui->cbBootWait->itemData( 0 ).toString() );
}


//SERIAL
void MainWindow::serialFillBaudRates()
{
    ui->cbBaud->addItem( QStringLiteral( "4800" ), QSerialPort::Baud4800 );
    ui->cbBaud->addItem( QStringLiteral( "9600" ), QSerialPort::Baud9600 );
    ui->cbBaud->addItem( QStringLiteral( "19200" ), QSerialPort::Baud19200 );
    ui->cbBaud->addItem( QStringLiteral( "38400" ), QSerialPort::Baud38400 );
    ui->cbBaud->addItem( QStringLiteral( "57600" ), QSerialPort::Baud57600 );
    ui->cbBaud->addItem( QStringLiteral( "74880" ), 74880 );
    ui->cbBaud->addItem( QStringLiteral( "115200" ), QSerialPort::Baud115200 );
    ui->cbBaud->addItem( QStringLiteral( "230400" ), 230400 );
    ui->cbBaud->addItem( QStringLiteral( "250000" ), 250000 );
    ui->cbBaud->addItem( QStringLiteral( "500000" ), 500000 );
    ui->cbBaud->addItem( QStringLiteral( "1000000" ), 1000000 );

    ui->cbBaud->setCurrentIndex( 1 );
}

//SERIAL SLOT[1]
void MainWindow::serialFillComPorts()
{
    static const char blankString[] = QT_TRANSLATE_NOOP( "SettingsDialog", "N/A" );

    ui->cbCom->clear();
    QString description;
    const auto infos = QSerialPortInfo::availablePorts();
    for( const QSerialPortInfo &info : infos ){
        QStringList list;
        description = info.description();
        list << info.portName()
             << ( !description.isEmpty() ? description : blankString );
        ui->cbCom->addItem( list.first(), list );
    }
}

//SERIAL SLOT[2]
void MainWindow::serialShowPortInfo( int idx )
{
    static const char blankString[] = QT_TRANSLATE_NOOP( "SettingsDialog", "N/A" );

    if( idx == -1 )
        return;

    const QStringList list = ui->cbCom->itemData( idx ).toStringList();
    ui->lblDevice->setText( QMainWindow::tr( "Device: %1" ).arg( list.count() > 1 ? list.at( 1 ) : QMainWindow::tr( blankString )) );

}

//SERIAL SLOT[3]
void MainWindow::serialManagePort(){

    if( serial->isOpen() ){
        serial->closeSerialPort();
        ui->btnConnect->setText( "Connect" );
        ui->btnConnect->setChecked( 0 );
    }else{
        serial->updateSettings( ui->cbCom->currentText(), ui->cbBaud->currentText().toInt() );
        if( serial->openSerialPort() ){
            ui->btnConnect->setText( "Disconnect" );
            ui->btnConnect->setChecked( 1 );
        }else{
            ui->btnConnect->setChecked( 0 );
        }
    }

}

//SERIAL SLOT[4]
void MainWindow::serialFlushData()
{
    serialData.clear();
}


//GRAPH SLOT[1]
void MainWindow::graphFillPlotParameters()
{
    ui->leName1->setText( "1" );
    ui->leName2->setText( "2" );
    ui->leName3->setText( "3" );
    ui->leName4->setText( "4" );

    ui->cbColor1->addItem( "Red", QString( "255,0,0,255" ) );
    ui->cbColor1->addItem( "Green", QString( "0,255,0,255" ) );
    ui->cbColor1->addItem( "Blue", QString( "0,0,255,255" ) );
    ui->cbColor1->addItem( "Cyan", QString( "0,255,255,255" ) );
    ui->cbColor1->setCurrentIndex( 0 );

    ui->cbColor2->addItem( "Red", QString( "255,0,0,255" ) );
    ui->cbColor2->addItem( "Green", QString( "0,255,0,255" ) );
    ui->cbColor2->addItem( "Blue", QString( "0,0,255,255" ) );
    ui->cbColor2->addItem( "Cyan", QString( "0,255,255,255" ) );
    ui->cbColor2->setCurrentIndex( 1 );

    ui->cbColor3->addItem( "Red", QString( "255,0,0,255" ) );
    ui->cbColor3->addItem( "Green", QString( "0,255,0,255" ) );
    ui->cbColor3->addItem( "Blue", QString( "0,0,255,255" ) );
    ui->cbColor3->addItem( "Cyan", QString( "0,255,255,255" ) );
    ui->cbColor3->setCurrentIndex( 2 );

    ui->cbColor4->addItem( "Red", QString( "255,0,0,255" ) );
    ui->cbColor4->addItem( "Green", QString( "0,255,0,255" ) );
    ui->cbColor4->addItem( "Blue", QString( "0,0,255,255" ) );
    ui->cbColor4->addItem( "Cyan", QString( "0,255,255,255" ) );
    ui->cbColor4->setCurrentIndex( 3 );

}

//GRAPH
void MainWindow::graphApplyPlotParam()
{
    graphPlot->setPlotColor( ui->cbColor1->itemData( ui->cbColor1->currentIndex() ).toString(), Graph::Plot1 );
    graphPlot->setPlotColor( ui->cbColor2->itemData( ui->cbColor2->currentIndex() ).toString(), Graph::Plot2 );
    graphPlot->setPlotColor( ui->cbColor3->itemData( ui->cbColor3->currentIndex() ).toString(), Graph::Plot3 );
    graphPlot->setPlotColor( ui->cbColor4->itemData( ui->cbColor4->currentIndex() ).toString(), Graph::Plot4 );

    graphPlot->setPlotName( ui->leName1->text(), Graph::Plot1 );
    graphPlot->setPlotName( ui->leName2->text(), Graph::Plot2 );
    graphPlot->setPlotName( ui->leName3->text(), Graph::Plot3 );
    graphPlot->setPlotName( ui->leName4->text(), Graph::Plot4 );

    graphPlot->setPlotRangeY( ui->sbMin1->value(), ui->sbMax1->value() );

    graphPlot->setPlotRangeX( ui->sbXAxisSamples->value() );

    graphPlot->replot();

    graphPlot->setPlotScale( ui->dsbScale1->value(), Graph::Plot1 );
    graphPlot->setPlotScale( ui->dsbScale2->value(), Graph::Plot2 );
    graphPlot->setPlotScale( ui->dsbScale3->value(), Graph::Plot3 );
    graphPlot->setPlotScale( ui->dsbScale4->value(), Graph::Plot4 );

}

//DATALOGGER
void MainWindow::dataloggerStart()
{
    if(!datalogger->logStatus()){
        datalogger->logStart();
        loggedSamples = 0;
        ui->lblLogSampels->setText("Samples: " + QString::number(loggedSamples));
        ui->btnLogStart->setText( "Stop" );
    }else{
        datalogger->logStop();
        ui->btnLogStart->setText( "Start" );
    }
}

////DATALOGGER
//void MainWindow::dataloggerStart()
//{
////    if( !datalogger->logStatus() ){
////        if( datalogger->logStart() ){
////            qDebug() << "start";
////            ui->btnStartDLogger->setText( "Stop" );
////            ui->btnStartDLogger->setChecked( 1 );
////        }else{
////            ui->btnStartDLogger->setChecked( 0 );
////        }
////    }

//    if( !datalogger->logStatus() ){
//        if( datalogger->logStart() ){
//            qDebug() << "start";
//            ui->btnStartDLogger->setText( "Stop" );
//            ui->btnStartDLogger->setChecked( 1 );
//        }
//    }else{
//        datalogger->logStop();
//        ui->btnStartDLogger->setText( "Start" );
//        ui->btnStartDLogger->setChecked( 0 );
//    }

//}

////DATALOGGER
//void MainWindow::dataloggerStop()
//{
//    if( datalogger->logStatus() ){
//        datalogger->logStop();
//        ui->btnStartDLogger->setText( "Start" );
//        ui->btnStartDLogger->setChecked( 0 );
//    }
//}
