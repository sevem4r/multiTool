#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QObject>
#include <QString>
#include <QStringList>

class Expression : public QObject
{
    Q_OBJECT

    QString m_data_to_parse;
    QString m_expression;

public:
    Expression();

    QStringList parse_data(const QString &data, QString eol);

signals:
    void ready(const QString& value);

public slots:
    void update_expression(const QString& expression);
    void prepare(const QString& data, QString eol);
};

#endif // EXPRESSION_H
