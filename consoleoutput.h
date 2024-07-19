#ifndef CONSOLEOUTPUT_H
#define CONSOLEOUTPUT_H

#include <QPlainTextEdit>
#include <QLineEdit>
#include <QRadioButton>

class ConsoleOutput : public QPlainTextEdit
{
    Q_OBJECT

    bool doClear;
    bool oneLine;
    QString consoleData;
    QString endOfLine;

    QWidget *parent;

    QString parseEndLine( QString end );

public:
    ConsoleOutput( QWidget *parent = nullptr );

    void flush();
    void put( const QString& data );

protected:
    void keyPressEvent( QKeyEvent *e ) override;
    void mousePressEvent( QMouseEvent *e ) override;

signals:
    void putOnGraph( const QString& data, QString eol );
    void putInFile( QString& data );

public slots:
    void setOneLineMode( bool oneLine );
    void setEndOfLine( QString eol );
    void clearConsole();

};

#endif // CONSOLEOUTPUT_H
