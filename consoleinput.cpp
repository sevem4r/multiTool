#include "consoleinput.h"
#include <QDebug>

ConsoleInput::ConsoleInput( QWidget *parent ) :
    QPlainTextEdit( parent ),
    endOfLine( '\r' )
{
    document()->setMaximumBlockCount( 512 );
    QPalette p = palette();
    p.setColor( QPalette::Base, QColor( 1, 0, 58 ) );
    p.setColor( QPalette::Text, QColor( 255, 255, 255 ) );
    setPalette( p );

    appendPlainText( ">" );
}

void ConsoleInput::keyPressEvent( QKeyEvent *e ){

    switch( e->key() ){

    case Qt::Key_Backspace:
        if( textCursor().positionInBlock() > 1 ){
            QPlainTextEdit::keyPressEvent( e );
            cmd.remove( cmd.size() - 1, 1 );
        }
        break;

    case Qt::Key_Return:
        QPlainTextEdit::keyPressEvent( e );

        if( !cmdHistory.contains( cmd ) ) cmdHistory.append( cmd );
        cmdIndexMax = cmdHistory.size() - 1;
        cmdIndex = cmdHistory.size();

        qDebug() << cmd << endOfLine;

        emit getData( cmd + endOfLine.toUtf8() );
        cmd.clear();

        insertPlainText( ">" );
        break;

    case Qt::Key_Up:
        cmdIndex--;
        if( cmdIndex < 0 ) cmdIndex = 0;
        while( textCursor().positionInBlock() > 1 ) textCursor().deletePreviousChar();
        textCursor().insertText( cmdHistory[cmdIndex] );
        cmd.clear();
        cmd = cmdHistory[cmdIndex];
        break;

    case Qt::Key_Down:
        cmdIndex++;
        if( cmdIndex > cmdIndexMax ) cmdIndex = cmdIndexMax;
        while( textCursor().positionInBlock() > 1 ) textCursor().deletePreviousChar();
        textCursor().insertText( cmdHistory[cmdIndex] );
        cmd.clear();
        cmd = cmdHistory[cmdIndex];
        break;

    case Qt::Key_Left:
    case Qt::Key_Right:
    default:
        cmd += e->text();
        QPlainTextEdit::keyPressEvent( e );
    }
}

void ConsoleInput::mousePressEvent( QMouseEvent *e ){

    switch( e->button() ){
    case Qt::MouseButton::RightButton:
    case Qt::MouseButton::LeftButton:
    case Qt::MouseButton::MidButton:
        break;
    default:
        QPlainTextEdit::mousePressEvent( e );
    }

}

QString ConsoleInput::parseEndLine( QString end )
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

//SLOT[0]
void ConsoleInput::clearConsole()
{
    clear();
    insertPlainText( ">" );
}

//SLOT[1]
void ConsoleInput::setEndOfLine( QString eol ){
    endOfLine = parseEndLine( eol );
}
