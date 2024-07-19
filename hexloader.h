#ifndef HEXLOADER_H
#define HEXLOADER_H

#include <QString>
#include <QWidget>
#include <QFile>
#include <QTimer>

class Hexloader : public QObject
{
    Q_OBJECT

    QWidget *parent;
    QTimer *timer;

    bool startUploadHex;
    bool getInfo;

    QString fileName;
    QFile file;

    QString hexLoaderData;
    QByteArray hexPage;
    QByteArray complementPage;

    int pageCnt;
    int pageToUpload;
    int hexLoaderState;

    void reloadHexFile( const QString& path );
    void restart();

public:
    explicit Hexloader( QWidget *parent = nullptr );

    void uploadProcess( const QString &data );
    bool status();

signals:
    void loadingFilePath( const QString& path );
    void loadingProgress( int val );
    void loadingProgressMax( int max );

    void upload( QByteArray data );

    void logClear();
    void logAppend( QString text );
    void logSet( QString text );
    void logColor( QColor color );
    void logButton( bool checked );

public slots:
    void loadHexFile();
    void uploadHexFile( bool serialStatus );
    void uCinfo( bool serialStatus );

private slots:
    void uploadTimeout();
};

#endif // HEXLOADER_H
