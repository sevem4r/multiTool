#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QLabel>
#include <QPlainTextEdit>

#include "consoleinput.h"
#include "consoleoutput.h"
#include "serialport.h"
#include "graph.h"
#include "expression.h"
#include "hexloader.h"
#include "bootloader.h"
#include "datalogger.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = nullptr );
    ~MainWindow();

signals:
    void getData( const QByteArray &data );

private slots:

//    void handleError( QSerialPort::SerialPortError error );

    // GRAPH
    void graphApplyPlotParam();

    // SERIAL
    void serialFillComPorts();
    void serialShowPortInfo( int idx );
    void serialManagePort();
    void serialGetData();
    void serialFlushData();

    //EXPRESSION
    void expressionReady(const QString& value);

    //HEXLOADER
    void hexloaderLogColor( QColor color );
    void hexloaderUploadHexFile();
    void hexloaderuCinfo();
    void hexloaderReset();

    //DATALOGGER
    void dataloggerStart();
//    void dataloggerStop();

private:

    void bootloaderFillOptions();
    void graphFillPlotParameters();
    void serialFillBaudRates();

    Ui::MainWindow *ui;
    QLabel *status;
    SerialPort * serial;
    ConsoleInput * consInput;
    ConsoleOutput * consOutput;
    ConsoleOutput* consExpression;
    Graph * graphPlot;
    Expression* expression;
    Hexloader * hexloader;
    Bootloader * bootloader;
    Datalogger * datalogger;

    QString serialData;
    int loggedSamples;
};

#endif // MAINWINDOW_H
