#include "expression.h"

extern "C"{
#include "tinyexpr.h"
}

Expression::Expression()
{

}

QStringList Expression::parse_data(const QString &data, QString eol){

    QStringList frames;

    m_data_to_parse.append(data);

    if(eol != ""){
        while(m_data_to_parse.contains(eol)){

            QString frame = m_data_to_parse.section(eol, 0, 0);

            int releaseBufLen = frame.length() + eol.length();

            m_data_to_parse.remove(0, releaseBufLen);

            frames.append(frame);
        }
    }

    if(m_data_to_parse.length() > 256) m_data_to_parse.clear();

    return frames;
}

//SLOT[1]
void Expression::prepare(const QString& data, QString eol)
{
    QStringList frames = parse_data(data, eol);

    for(auto& frame : frames){

        char sep = ',';
        QList<QString> list = frame.split(sep);

//        QList<double> tokens;
//        for(int j=0; j<4; j++){
//            if(j < list.size()) tokens.append(list.at(j).toDouble());
//            else tokens.append(0);
//        }

//        emit ready((tokens[0] * 656471 - 19070470588) / 1000000);

        if(list.size()){
            QString eval = m_expression;

            eval.replace("x", list[0]);

            emit ready(list[0] + " = " + QString::number(te_interp(eval.toLocal8Bit().constData(), 0)));
        }
    }
}

//SLOT[2]
void Expression::update_expression(const QString& expression){
    m_expression = expression;
}
