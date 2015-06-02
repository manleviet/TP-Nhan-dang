//
//
// main.cpp
//
// Ce programme traverse les fichiers emails
// pour creer le vocabulaire
//
// LE Viet Man
// 14/12/10
//
//

#include <QDebug>
#include <QString>
#include <QMap>
#include <QMapIterator>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QList>
#include <QFile>
#include <QTextStream>
#include "stemming.h"

QString Usage = QString("Usage: createvoca [option] <path>\n\
Traverser les fichiers emails pour creer le vocabulaire.\n\
Arguments:\n\
    -h : aide\n\
    -s <filename> : le fichier du vocabulaire\n\
    -l <filename> : le fichier stocke la liste d'arrete");

//
// Prototypes des fonctions
//
void showUsage(bool);
QStringList loadStopList(QString);
QStringList loadWords(QString);

int main(int argc, char *argv[])
{
    if (argc < 2) // manque les paramètres
    {
        showUsage(true); // afficher les informations d'aide
    }

    QString pathEmails = "";
    QString fileVoca = "";
    QString fileStopList = "";

    // cette boucle lit les arguments passés
    int c;
    while ((c = getopt(argc, argv, "hs:l:")) != -1)
    {
        switch (c)
        {
            case 'h':
            {
                showUsage(false);
                break;
            }
            case 'o':
            {
                extern char *optarg;
                fileVoca = optarg;
                break;
            }
            case 'l':
            {
                extern char *optarg;
                fileStopList = optarg;
                break;
            }
            default:
                break;
        }
    }

    // lire le nom de fichier a la fin de la ligne de commande
    extern int optind;
    if (optind>=argc)
    {
        showUsage(true);
    }
    pathEmails = argv[optind];

    if (fileVoca.isEmpty())
    {
        fileVoca = "voca";
    }

    // load stopList
    QStringList stopList = loadStopList(fileStopList);

    // duyet qua thu muc
    QStringList wordList;
    QMap<QString, int> voca;
    Stemming stem;
    QString word,stemWord;
    QDir dir(pathEmails);

    if (!dir.exists())
    {
        qDebug() << QString("Il n'existe pas le repertoire %1\n").arg(pathEmails);
        return 1;
    }

    dir.setFilter(QDir::Dirs);
    dir.setSorting(QDir::Name);

    QStringList dirList = dir.entryList();
    foreach (QString subPath, dirList)
    {
        if ((subPath == ".") || (subPath == ".."))
            continue;

        subPath = pathEmails + QDir::separator() + subPath;

        QDir dirEmails(subPath);

        dirEmails.setFilter(QDir::Files);
        dirEmails.setSorting(QDir::Name);

        // khai bao
        QFileInfo fileInfo;
        QString filename;

        QFileInfoList listEmails = dirEmails.entryInfoList();
        for (int i = 0; i < listEmails.size(); i++)
        {
            fileInfo = listEmails.at(i);
            filename = fileInfo.filePath();

            qDebug() << QString("Lire le courrier %1").arg(filename);

            wordList = loadWords(filename);

            wordList.sort();

            //qDebug() << wordList.size() << endl;

            // xu ly wordList
            QString str;
            int i = 0;
            while (i < wordList.size())
            {
                str = wordList.at(i);
                if (stopList.contains(str))
                {
                    wordList.removeAt(i);
                }
                else
                {
                    i++;
                }
            }
            //qDebug() << wordList.size() << endl;

            // stemming
            /*for (int i = 0; i < wordList.size(); i++)
            {
                word = wordList[i];
                stemWord = stem.stemmer(word);
                if (word != stemWord)
                {
                    wordList[i] = stemWord;
                }
            }*/

            // tao ra vocabulaire
            foreach (QString str, wordList)
            {
                if (voca.contains(str))
                {
                    int value = voca[str];
                    voca[str] = value + 1;
                }
                else
                {
                    voca[str] = 1;
                }
            }

            wordList.clear();
        }
    }

    // giai phong stopList
    stopList.clear();

    qDebug() << "Vocabulaire co " << voca.size() << endl;

    // xoa nhung tu nho hon 3
    qDebug() << "Xoa nhung tu nho hon 3" << endl;
    QMapIterator<QString, int> it(voca);
    while (it.hasNext())
    {
        it.next();
        if (it.value() < 100)
        {
            voca.remove(it.key());
        }
    }

    qDebug() << "Con lai " << voca.size() << endl;

    // in voca
    QFile fileOut(fileVoca);
    if (!fileOut.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    QTextStream streamOut(&fileOut);

    //streamOut << voca.size() << endl;

    it = voca;
    while (it.hasNext())
    {
        it.next();
        streamOut << it.key() << endl;
        //streamOut << it.key() << " " << it.value() << endl;
    }

    return 0;
}

//
// La fonction sert à afficher les informations d'aide de ce programme
// @param :
//	bool erreur : true, cad il y a une erreur
//                          la fonction exécutera la commande exit(1)
//                    false, cad il n'y a pas d'erreur
//                           la fonction exécutera la commande exit(0)
//
void showUsage(bool erreur)
{
    qDebug() << Usage << endl;
    if (erreur)
    {
        exit(1);
    }
    else
    {
        exit(0);
    }
}

QStringList loadStopList(QString filename)
{
    QStringList fields;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Cannot find the file %1\n").arg(filename);
        return fields;
    }

    // doc noi dung file
    QTextStream in(&file);
    QString line = in.readLine();
    fields = line.split(",", QString::SkipEmptyParts);

    return fields;
}

QStringList loadWords(QString filename)
{
    QStringList voca;
    QString line;
    QStringList fields;

    // mo file
    QFile fileIn(filename);
    if (!fileIn.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Cannot find the file %1\n").arg(filename);
        return voca;
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

        voca += fields;
    }

    return voca;
}
