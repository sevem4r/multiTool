#include "graph.h"

Graph::Graph():
    use( false ),
    samples( 0 ),
    xAxisSamples( 249 )
{

    scaleGraph[0] = 1.0;
    scaleGraph[1] = 1.0;
    scaleGraph[2] = 1.0;
    scaleGraph[3] = 1.0;

    addGraph();
    graph( 0 )->setPen( QPen( QColor( 255, 0, 0, 255 ) ) );
    graph( 0 )->setName( "1" );
    addGraph();
    graph( 1 )->setPen( QPen( QColor( 0, 255, 0, 255 ) ) );
    graph( 1 )->setName( "2" );
    addGraph();
    graph( 2 )->setPen( QPen( QColor( 0, 0, 255, 255 ) ) );
    graph( 2 )->setName( "3" );
    addGraph();
    graph( 3 )->setPen( QPen( QColor( 0, 255, 255, 255 ) ) );
    graph( 3 )->setName( "4" );
    legend->setVisible( true );
    axisRect()->insetLayout()->setInsetAlignment( 0, Qt::AlignLeft | Qt::AlignTop );
    xAxis->setRange( 0, xAxisSamples - 1 );
    yAxis->setRange( 0, 5000 );
}

Graph::~Graph(){

}

void Graph::setPlotColor( QString rgb, int plot )
{
    char sep = ',';
    QList<QString> list;
    QList<int> tokens;

    list = rgb.split( sep );
    for( int j=0; j<list.size(); j++ ) tokens.append( list.at( j ).toInt() );
    graph( plot )->setPen( QPen( QColor( tokens[0], tokens[1], tokens[2], tokens[3] ) ) );
}

void Graph::setPlotName( QString name, int plot )
{
    graph( plot )->setName( name );
}

void Graph::setPlotScale( double scale, int plot )
{
    scaleGraph[plot] = scale;
}

void Graph::setPlotRangeY( double min, double max )
{
    yAxis->setRange( min, max );
}
void Graph::setPlotRangeX( int range )
{
    xAxisSamples = range;
    xAxis->setRange( 0, xAxisSamples - 1 );
}

QStringList Graph::parseData( const QString &data, QString eol ){

    QStringList frames;

    dataToParse.append( data );

    if( eol != "" ){
        while( dataToParse.contains( eol ) ){

            QString frame = dataToParse.section( eol, 0, 0 );

            int releaseBufLen = frame.length() + eol.length();

            dataToParse.remove( 0, releaseBufLen );

            frames.append( frame );

        }
    }

    if( dataToParse.length() > 256 ) dataToParse.clear();

    return frames;
}

//SLOT[1]
void Graph::plotData( const QString& data, QString eol )
{
    if( use ){

        QStringList frames = parseData( data, eol );

        for( auto& frame : frames ){

            char sep = ',';
            QList<QString> list = frame.split( sep );

            QList<double> tokens;
            for( int j=0; j<4; j++ ){
                if( j < list.size() ) tokens.append( list.at( j ).toDouble() );
                else tokens.append( 0 );
            }


            if( samples > xAxisSamples ){
                for( int i=0; i<4; i++ )
                    samplesXGraph[i].remove( 0, 1 );
            }else{
                samplesY.append( samples );
                samples++;
            }

            for( int i=0; i<4; i++ ){
                samplesXGraph[i].append( tokens[i] * scaleGraph[i] );
                graph( i )->setData( samplesY, samplesXGraph[i] );
            }

            replot();
        }
    }
}

//SLOT[2]
void Graph::useGraph( bool use )
{
    this->use = use;
}
