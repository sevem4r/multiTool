#include "datalogger.h"
#include <QFileDialog>
#include <QMainWindow>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

Datalogger::Datalogger( QWidget * parent ):
    parent( parent ),
    status( false ),
    fileStatus( false ),
    linesToLog( 1 ),
    linesLogged( 0 ),
    fileType( FileType::txt )
{

}

Datalogger::~Datalogger()
{
    file.close();
}

bool Datalogger::logStatus()
{
    return status;
}

//SLOT[1]
void Datalogger::setLogFile()
{
    QString selectedFilter = "Text Files (*.txt)";
    fileName = QFileDialog::getSaveFileName( parent, tr("Export As"), "", tr("Text Files (*.txt)"), &selectedFilter, QFileDialog::DontUseNativeDialog );


    if( fileName != "" ){
        if( selectedFilter == "Text Files (*.txt)" ){
            QStringList name = fileName.split('.');
                if(name.count() >= 1){
                fileName = name[0] + ".txt";
                fileType = FileType::txt;
            }
        }
        if( selectedFilter == "Comma-Separated Values (*csv)" ){
            QStringList name = fileName.split('.');
                if(name.count() >= 1){
                fileName = name[0] + ".csv";
                fileType = FileType::txt;
            }

            fileName += ".csv";
            fileType = FileType::csv;
        }

        emit logFilePath( fileName );

        file.setFileName( fileName );
    }
}

//SLOT[2]
void Datalogger::logString(const QString& data, EoL eol)
{
    if( status ){
        if( file.isOpen() ){

            QTextStream out( &file );
            if( fileType == FileType::txt ) out << data;
            if( fileType == FileType::csv ) out << "\"" + data + "\"";
            if( eol == EoL::Add ) out << "\n";

            linesLogged++;
            emit loadingProgress( linesLogged );

            if( linesLogged > linesToLog ){
                linesLogged = 0;
                emit loadingEnd();
            }
        }
    }else file.close();
}

void Datalogger::logByteArray(const QByteArray& data, EoL eol){
    if( status ){
        if( file.isOpen() ){
            QTextStream out( &file );
            if( fileType == FileType::txt ) out << data;
            if( fileType == FileType::csv ) out << "\"" + data + "\"";
            if( eol == EoL::Add ) out << "\n";

            linesLogged++;
            emit loadingProgress( linesLogged );

            if( linesLogged > linesToLog ){
                linesLogged = 0;
                emit loadingEnd();
            }
        }
    }
}

//SLOT[3]
bool Datalogger::logStart()
{
    if( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ){
        QMessageBox::critical( parent, tr( "Error" ), file.errorString() );
        return false;
    }

    status = true;
    emit loadingProgressMax( linesToLog );

    qDebug() << "Datalogger::logStart";

    return true;
}

//SLOT[4]
void Datalogger::logStop()
{
    status = false;

    file.close();

    qDebug() << "Datalogger::logStop";
}

//SLOT[5]
void Datalogger::logLines(int lines )
{
    linesToLog = lines;
}
