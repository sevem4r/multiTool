#include "consoleoutput.h"

#include <QScrollBar>
#include <QStringList>
#include <QDebug>

ConsoleOutput::ConsoleOutput( QWidget *parent ) :
    doClear( false ),
    oneLine( false ),
    parent( parent )
{

    document()->setMaximumBlockCount( 512 );
    QPalette p = palette();
    p.setColor( QPalette::Base, QColor( 1, 0, 58 ) );
    p.setColor( QPalette::Text, QColor( 255, 255, 255 ) );
    setPalette( p );

    setReadOnly( true );

    endOfLine = "\\r\\n";
}

void ConsoleOutput::keyPressEvent( QKeyEvent *e ){

    switch( e->key() ){

    default:
        ;
    }
}

void ConsoleOutput::mousePressEvent( QMouseEvent *e ){

    switch( e->button() ){
    case Qt::MouseButton::RightButton:
    case Qt::MouseButton::LeftButton:
    case Qt::MouseButton::MidButton:
        break;
    default:
        QPlainTextEdit::mousePressEvent( e );
    }

}

void ConsoleOutput::put( const QString &data ){

    consoleData.append( data );

    QString endLine = parseEndLine( endOfLine );

    emit putOnGraph( data, endLine );

    if( endLine != "" ){
        static bool prevContainedEndLinePart = false;
        int index = consoleData.indexOf( endLine[0] );

        while( consoleData.length() > 0 ){

            index = consoleData.indexOf( endLine[0] );

            if( index == -1 ){  //NONE
                QString text = consoleData;
                insertPlainText( text );
                consoleData.remove( 0, consoleData.length() );
                break;
            }else if( index + endLine.length() <= consoleData.length() ){   //FULL
                if( prevContainedEndLinePart ){
                    insertPlainText( "\n" );
                    consoleData.remove( 0, index + endLine.length() );
                    prevContainedEndLinePart = false;
                }else{
                    QString text = consoleData.mid( 0, index );
                    insertPlainText( text + "\n" );
                    consoleData.remove( 0, index + endLine.length() );
                }
            }else{  //PARTIAL
                QString text = consoleData.mid( 0, index );
                insertPlainText( text );
                prevContainedEndLinePart = true;
                break;
            }

        }
    }

    QScrollBar *bar = verticalScrollBar();
    bar->setValue( bar->maximum() );
}

void ConsoleOutput::flush(){
    consoleData.clear();
}

QString ConsoleOutput::parseEndLine( QString end )
{
    QString parsedEnd;

    for( int i=0; i<end.length(); i++ ){
        if( end[i] == '\\' ){
            i++;
            if( i < end.length() ){
                if( end[i] == 'r' ) parsedEnd += '\r';
                if( end[i] == 'n' ) parsedEnd += '\n';
            }else{
                i--;
                parsedEnd += '\\';
            }
        }else{
            parsedEnd += end[i];
        }
    }

    return parsedEnd;
}


//SLOT[1]
void ConsoleOutput::setOneLineMode( bool oneLine )
{
    this->oneLine = oneLine;
}

//SLOT[2]
void ConsoleOutput::setEndOfLine( QString eol )
{
    endOfLine = eol;
}

//SLOT[3]
void ConsoleOutput::clearConsole()
{
    clear();
}
