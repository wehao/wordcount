#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <QString>
#include <QDir>
#include <QFutureWatcher>
#include <QFile>
#include <QTextStream>
#include <QFuture>
#include <qtconcurrentmap.h>

#include <QQmlApplicationEngine>

//search all files under the startDir with the filters
QList<QString> findFiles(QString startDir, QStringList  filters)
{
    QList<QString> retFiles;
    QDir dir(startDir);

    qDebug() << dir;

    foreach (QString name, dir.entryList(filters, QDir::Files)) {
        retFiles += startDir + '/' + name;
    }

    foreach(QString subdir, dir.entryList(QDir::AllDirs |QDir::NoDotAndDotDot)) {
        retFiles += findFiles(startDir + '/' + subdir, filters);
    }

    return retFiles;
}

//return the file name's word count
int wordcount(QString name)
{
    int ret = 0;
    QFile file(name);
    file.open(QFile::ReadOnly);
    QTextStream textStream(&file);
    while(textStream.atEnd() == false)
    {
         QString str = textStream.readLine();
         str = str.simplified();                            //
         int whiteCount = str.count(" ");           //
         ret += whiteCount + 1;
         if(*str.rbegin() == '-')
             ret --;
    }

    return ret;
}




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFutureWatcher<int> water;
    QFuture<int> future;


    QList<QString> temp = findFiles("./files", QStringList() << "*.txt");
    future = QtConcurrent::mapped(temp, wordcount);
    future.waitForFinished();
    water.setFuture(future);

    for(int i = 0; i < temp.count(); i++)
    {
        qDebug() << "the word count of " + temp[i] + " is " + QString::number(water.resultAt(i));
    }

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
