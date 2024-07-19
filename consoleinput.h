#ifndef CONSOLEINPUT_H
#define CONSOLEINPUT_H

#include <QPlainTextEdit>

class ConsoleInput : public QPlainTextEdit
{
    Q_OBJECT

    QList<QByteArray> cmdHistory;
    QByteArray cmd;
    QString endOfLine;
    int cmdIndex;
    int cmdIndexMax;

    QString parseEndLine( QString end );

signals:
    void getData( const QByteArray &data );

public:
    explicit ConsoleInput( QWidget *parent = nullptr );

//    void putData(const QString &data);

protected:
    void keyPressEvent( QKeyEvent *e ) override;
    void mousePressEvent( QMouseEvent *e ) override;

public slots:
    void clearConsole();
    void setEndOfLine( QString eol );
};

#endif // CONSOLEINPUT_H
