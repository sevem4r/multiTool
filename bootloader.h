#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <QWidget>
#include <QProcess>

class Bootloader : public QObject
{
    Q_OBJECT

    QWidget *parent;

    QString uC;
    QString Frequency;
    QString Baudrate;
    QString BLS;
    QString Wait;

    QProcess compilePrcocess;
    QProcess uploadProcess;

    bool copyMakefile();
    bool copyMain();

public:
    explicit Bootloader( QWidget *parent = nullptr );

signals:
    void compileOutput( QString data );
    void uploadOutput( QString data );


public slots:
    void setuC( QString uc );
    void setFrequency( QString frequency );
    void setBaudrate( QString baudrate );
    void setBLS( QString bls );
    void setWait( QString wait );

    void compile();
    void upload();
};

#endif // BOOTLOADER_H
