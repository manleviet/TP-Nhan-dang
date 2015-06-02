#ifndef EMAIL_H
#define EMAIL_H

#include <QDebug>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QVector>
#include <QMap>
#include <QMapIterator>
#include "math.h"
#include "stemming.h"

class Email
{
public:
    Email(QString filename, QString classe, QStringList voca, QVector<double> &totalEmailWordApprence);

    QString getData();

    void calculIDF(QVector<double> totalEmailWordApprence, double totalEmail);

private:
    QString classe;
    QVector<double> values;
    double totalWord;

    QStringList loadWords(QString filename);
    QMap<QString, int> convertListToMap(QStringList list);
};

#endif // EMAIL_H
