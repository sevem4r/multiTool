#ifndef GRAPH_H
#define GRAPH_H

#include "qcustomplot.h"

class Graph : public QCustomPlot
{
    Q_OBJECT

    bool use;
    int samples;
    int xAxisSamples;
    double scaleGraph[4];

    QVector<double> samplesXGraph[4];
    QVector<double> samplesY;

    QString dataToParse;

    QStringList parseData( const QString &data, QString eol );

public:
    Graph();
    ~Graph();

    enum{
        Plot1,
        Plot2,
        Plot3,
        Plot4
    };

    void setPlotColor( QString rgb, int plot );
    void setPlotName( QString name, int plot );
    void setPlotScale( double scale, int plot );
    void setPlotRangeY( double min, double max );
    void setPlotRangeX( int range );

public slots:
    void plotData( const QString& data, QString eol );
    void useGraph( bool use );

};

#endif // GRAPH_H
