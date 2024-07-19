#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QWidget>
#include <QFile>

class Datalogger : public QObject
{
    Q_OBJECT

public:
    enum class FileType{
        txt,
        csv
    };

    enum class EoL{
        No,
        Add
    };

private:
    QWidget * parent;

    bool status;
    bool fileStatus;

    int linesToLog;
    int linesLogged;

    FileType fileType;

    QString fileName;
    QFile file;

public:
    explicit Datalogger( QWidget * parent = nullptr );
    ~Datalogger();

    bool logStatus();

signals:
    void logFilePath( QString& path );
    void loadingProgress( int val );
    void loadingProgressMax( int max );
    void loadingEnd();

public slots:
    void setLogFile();
    void logString(const QString& data, EoL eol);
    void logByteArray(const QByteArray& data, EoL eol);
    bool logStart();
    void logStop();
    void logLines( int lines );

};

#endif // DATALOGGER_H
