#include "email.h"

Email::Email(QString filename, QString classe, QStringList voca, QVector<double> &totalEmailWordApprence)
{
    // khoi tao cac gia tri
    this->classe = classe;
    values = QVector<double>(voca.size(), 0);
    this->totalWord = 0;

    // load tu trong email
    QStringList wordList = loadWords(filename);
    // sap xep
    wordList.sort();
    // stemming
    QString word, stemWord;
    Stemming stem;
    for (int i = 0; i < wordList.size(); i++)
    {
        word = wordList[i];
        stemWord = stem.stemmer(word);
        if (word != stemWord)
        {
            wordList[i] = stemWord;
        }
    }
    // chuyen sang dang map, moi doi tuong se co so luong lan xuat hien tu do trong email
    QMap<QString, int> wordMap = convertListToMap(wordList);
    // xoa wordList de tiet kiem bo nho
    wordList.clear();

    // duyet qua tung tu trong email
    // kiem tra xem no co ton tai trong vocabulaire hay khong
    // neu co thi gan so luong xuat hien
    QMapIterator<QString, int> it(wordMap);
    int index;
    while (it.hasNext())
    {
        it.next();
        if (voca.contains(it.key()))
        {
            index = voca.indexOf(it.key());

            values[index] = it.value();
            totalEmailWordApprence[index]++;
            //qDebug() << values[index] << endl;
            totalWord += it.value();
        }
    }

    //qDebug() << totalWord << endl;

    //tinh tf
    for (int i = 0; i < values.size(); i++)
    {
        values[i] /= totalWord;
    }
}

QString Email::getData()
{
    QString data;
    for (int i = 0; i < values.size(); i++)
    {
        data += QString("%1,").arg(values.at(i));
    }

    data += classe;

    return data;
}

void Email::calculIDF(QVector<double> totalEmailWordApprence, double totalEmail)
{
    // tinh idf
    double idf;
    for (int i = 0; i < values.size(); i++)
    {
        idf = log(totalEmail / totalEmailWordApprence[i]);
        values[i] *= idf;
    }
}

QStringList Email::loadWords(QString filename)
{
    QStringList wordList, fields;
    QString line;

    // mo file
    QFile fileIn(filename);
    if (!fileIn.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Cannot find the file %1\n").arg(filename);
        return wordList;
    }

    // doc noi dung file
    QTextStream streamIn(&fileIn);
    while (!streamIn.atEnd())
    {
        // lire une ligne
        line = streamIn.readLine();
        line = line.toLower();
        line = line.remove(QRegExp("[0123456789]"));

        //qDebug() << line << endl;

        //line = line.replace(QRegExp("[0123456789]"), QString(" "));
        line = line.replace(QChar('_'), QString(" "));

        fields = line.split(QRegExp("\\W+"),
                            QString::SkipEmptyParts);

        /*foreach (QString str, fields)
        {
            qDebug() << str << " ";
        }*/

        wordList += fields;
    }

    return wordList;
}

QMap<QString, int> Email::convertListToMap(QStringList list)
{
    QMap<QString, int> wordMap;

    foreach (QString str, list)
    {
        if (wordMap.contains(str))
        {
            int value = wordMap[str];
            wordMap[str] = value + 1;
        }
        else
        {
            wordMap[str] = 1;
        }
    }

    return wordMap;
}
