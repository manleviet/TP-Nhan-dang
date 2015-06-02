//
// Modification Porter stemmer algorithm
// http://www.tartarus.org/~martin/PorterStemmer
//

#ifndef STEMMING_H
#define STEMMING_H

#include <QString>
#include <QDebug>

class Stemming
{
public:
    Stemming();

    static QString stemmer(QString word);
};

#endif // STEMMING_H
