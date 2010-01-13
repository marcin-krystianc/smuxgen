//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <QImage>

#include "globalsmuxgentools.h"
#include "cglobaltracer.h"

const QString tmpDir = QString::fromUtf8("tmp")+QDir::separator();

const int IMG_WIDTH     = 320;
const int IMG_HEIGHT    = 320;

/////////////////////////////////////////////////////////////////////////////
QStringList parseGoogleHtml (const QString &fileName)
{
    QFile inputFile;
    QStringList retList;
    QTextStream inputFileStream;
    QString html;
    inputFile.setFileName(fileName);
    if (!inputFile.open(QIODevice::ReadOnly))  {
         globalTracer.trace(QString("Cannot open file: ")+fileName,traceError);
         return retList;
     }

    inputFileStream.setDevice( &inputFile );
    inputFileStream.setCodec("UTF-8");
    html=inputFileStream.readAll();

    int pos=0,leftPos,rightPos;

    QString  leftBound  ("/imgres?imgurl\\x3d");
    QString  rightBound ("\\x26");
    QString  tmp;

    while ((leftPos=html.indexOf(leftBound,pos)) != -1)
    {
     rightPos=html.indexOf(rightBound,leftPos);
     if (rightPos==-1)
         break;

     pos=rightPos;
     tmp = html.mid(leftPos+leftBound.length(),rightPos-leftPos-leftBound.length());
     if (tmp.indexOf("%")!=-1)
         continue;
     retList.append(tmp);
    }
    return retList;
}

/////////////////////////////////////////////////////////////////////////////
void deleteFile (const QString &fileName)
{
    QFile fileObject (fileName);
    globalTracer.trace(QString("deleteFile: ")+fileName,traceLevel3);
    fileObject.remove();
}

/////////////////////////////////////////////////////////////////////////////
QString getKeyWord (QString iString)
{
    static QString A=QString::fromUtf8("ĄĆŻŹŚŃÓŁĘąćżźśńółę!@#$%^&*()_-=+,./<>?;':\"[]\{}|");
    static QString B=QString::fromUtf8("ACZXSNOLEaczzsnole                               ");

    for (int i=A.length()-1;i>=0;--i)
        iString.replace(A.at(i),B.at(i));

    QStringList tmp=iString.split(" ",QString::SkipEmptyParts);

    if (tmp.count()==0)
        return QString::fromUtf8("");

    if (tmp.count() > 1)
        return (tmp.at(0)+QString(" ")+tmp.at(1));

    return tmp.at(0);
}

/////////////////////////////////////////////////////////////////////////////
bool scalePicture (QString path,int x,int y)
{
    QImage img;

    if (!img.load(path))
    {
        globalTracer.trace(QString("Error:scalePicture load:")+path,traceError);
        return false;
    }

    if (!img.scaled(x,y).save(path))
    {
        globalTracer.trace(QString("Error:scalePicture save:")+path,traceError);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool checkIsFileOk(const QString &fileName)
{
    QFile fileObject (fileName);
    if (fileObject.size()>0)
        return true;

    globalTracer.trace(QString("NOT OK: ")+fileName,traceLevel2);
    return false;
}

/////////////////////////////////////////////////////////////////////////////
QString getFileName (int i)
{
    QString name;
    name.fill('0',5);
    name+=QString::number(i);
    name+=".xml";
    name=name.right(9);
    return QString("item")+name;
}

/////////////////////////////////////////////////////////////////////////////
QString getMediaFileName (int i)
{
    QString name;
    name.fill('0',5);
    name+=QString::number(i);
    name=name.right(5);
    return name;
}
