//
//
// main.cpp
//
// Ce programme exporte les donnees en sous forme de arff
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
#include "email.h"

QString Usage = QString("Usage: exportArff [option] <path>\n\
Exporter les donnees en la forme de arff.\n\
Arguments:\n\
    -h : aide\n\
    -v <filename> : le fichier du vocabulaire\n\
    -s <filename> : le fichier arff");

//
// Prototypes des fonctions
//
void showUsage(bool);
QStringList loadVocabulaire(QString);
QStringList loadStopList(QString);
void writeToArff(QList<Email>, QStringList, QString);

int main(int argc, char *argv[])
{
    if (argc < 2) // manque les paramètres
    {
        showUsage(true); // afficher les informations d'aide
    }

    QString pathEmails = "";
    QString fileVoca = "";
    QString fileArff = "";

    // cette boucle lit les arguments passés
    int c;
    while ((c = getopt(argc, argv, "hv:s:")) != -1)
    {
        switch (c)
        {
            case 'h':
            {
                showUsage(false);
                break;
            }
            case 'v':
            {
                extern char *optarg;
                fileVoca = optarg;
                break;
            }
            /*case 'l':
            {
                extern char *optarg;
                fileStopList = optarg;
                break;
            }*/
            case 's':
            {
                extern char *optarg;
                fileArff = optarg;
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

    // load vocabulaire
    QStringList voca = loadVocabulaire(fileVoca);

    if (voca.isEmpty())
    {
        qDebug() << "Khong co co so du lieu";
        return 1;
    }

    // duyet qua thu muc
    QList<Email> emails;
    //QVector<double> totalWordOfEmail(voca.size(), 0);
    QVector<double> totalEmailWordApprence(voca.size(), 0);
    double totalEmail = 0;
    QDir dir(pathEmails);

    if (!dir.exists())
    {
        qDebug() << QString("Il n'existe pas le repertoire %1\n").arg(pathEmails);
        return 1;
    }

    dir.setFilter(QDir::Dirs);
    dir.setSorting(QDir::Name);

    QStringList dirList = dir.entryList();
    foreach (QString classe, dirList)
    {
        if ((classe == ".") || (classe == ".."))
            continue;

        QString subPath = pathEmails + QDir::separator() + classe;

        QDir dirEmails(subPath);

        dirEmails.setFilter(QDir::Files);
        dirEmails.setSorting(QDir::Name);

        QFileInfoList listEmails = dirEmails.entryInfoList();
        for (int i = 0; i < listEmails.size(); i++)
        {
            totalEmail++;

            QFileInfo fileInfo = listEmails.at(i);
            QString filename = fileInfo.filePath();

            qDebug() << QString("Lire le courrier %1").arg(filename);

            // Tao ra mot doi tuong Email
            Email email(filename, classe, voca, totalEmailWordApprence);

            emails.append(email);

            // can xu ly tf-idf
        }
    }

    // tinh idf
    for (int i = 0; i < emails.size(); i++)
    {
        Email email = emails.at(i);
        email.calculIDF(totalEmailWordApprence, totalEmail);
    }

    // ghi ra file arff
    if (fileArff.isEmpty())
    {
        fileArff = "exportArff.arff";
    }
    writeToArff(emails, voca, fileArff);

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

QStringList loadVocabulaire(QString filename)
{
    QStringList voca;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Cannot find the file %1\n").arg(filename);
        return voca;
    }

    QTextStream in(&file);
    QString line;
    while (!in.atEnd())
    {
        line = in.readLine();
        voca.append(line);
    }

    return voca;
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

void writeToArff(QList<Email> emails, QStringList voca, QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    // ecrire les informations sur le fichier
    QTextStream out(&file);

    // in phan header
    out << "@RELATION email" << endl;

    // in cac atrributes
    for (int i = 0; i < voca.size(); i++)
    {
        out << "@ATTRIBUTE " << voca.at(i) << " REAL" << endl;
    }

    // in class
    out << "@ATTRIBUTE class {1,2,3,4,5,6,7,8,9,10}" << endl;

    // in data
    out << "@DATA";

    for (int i = 0; i < emails.size(); i++)
    {
        out << endl;

        Email email = emails.at(i);

        out << email.getData();
    }
}
