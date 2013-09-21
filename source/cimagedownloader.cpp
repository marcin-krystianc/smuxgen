//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QString>
#include <QDir>
#include <Qprocess>
#include <QPixmap>

#include "cimagedownloader.h"
#include "cglobaltracer.h"
#include "globalsmuxgentools.h"

/////////////////////////////////////////////////////////////////////////////
cImageDownloadHelper::cImageDownloadHelper(const QString &ext, int id)
{
    ID = id;
    EXT = ext;
}

/////////////////////////////////////////////////////////////////////////////
cImageDownloadHelper::~cImageDownloadHelper()
{
    deleteFile(myFileName());
}

/////////////////////////////////////////////////////////////////////////////
QString cImageDownloadHelper::myFileName()
{
    return tmpDir+"IMG"+EXT+".jpg";
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloadHelper::getImage(const QString &url)
{
    m_url = url;
    trace(QString("cImageDownloadHelper::getImage id:")+QString::number(ID)+QString::fromUtf8(" url:")+url, traceLevel3);

    start();
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloadHelper::run()
{
    trace(QString("cImageDownloadHelper::run id:")+QString::number(ID)+QString::fromUtf8(" url:")+m_url, traceLevel3);

    QProcess myProcess;
    const int timeOut = -1; // no timeout
    QString fileName = myFileName();

    QStringList arguments;

    deleteFile(fileName);

    arguments.append(m_url+QString(" "));
    arguments.append(fileName+" ");

    trace(QString("getImage.bat ")+arguments.join(" "), traceLevel3);

    myProcess.start("getImage.bat", arguments );
    if (!myProcess.waitForStarted())
        trace(QString("Error.waitForStarted() :getImage.bat ")+arguments.join(" "), traceError);
    myProcess.waitForFinished(timeOut);
    if (myProcess.exitCode())
        trace(QString("Error.exitCode() :getImage.bat ")+arguments.join(" "), traceError);

    if (!scalePicture(fileName, IMG_WIDTH, IMG_HEIGHT))
    {
        trace(QString("Error:scalePicture ")+fileName, traceError);
        deleteFile(fileName);
    }

    if (checkIsFileOk(fileName))
    {
        QPixmap pixmap(fileName);
        emit finished (true, pixmap, ID, m_url);
    }
    else
    {
        QPixmap pixmap;
        emit finished (false, pixmap, ID, m_url);
    };
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloadHelper::trace (const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cImageDownloader::cImageDownloader(const QString &id)
{
    ID = id;
    for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
    {
        imageDownloadHelper[i] = new cImageDownloadHelper(ID+QString::number(i), i);
        connect(imageDownloadHelper[i], SIGNAL(finished(bool, const QPixmap& , int, const QString &)) , this , SLOT(helpThreadFinished(bool, const QPixmap& , const QString &)));
    }

}

/////////////////////////////////////////////////////////////////////////////
cImageDownloader::~cImageDownloader()
{
    /*
 for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
 delete imageDownloadHelper[i];
 */

    deleteFile(myFileName());
}

/////////////////////////////////////////////////////////////////////////////
QString cImageDownloader::myFileName ()
{
    return tmpDir+"HTML"+ID;
}


/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::trace (const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::getImages(const QString &keyWords)
{
    m_keyWords = keyWords;
    newTask = true;
    start();
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::run ()
{

    trace(QString("cImageDownloader::run id:")+ID+QString::fromUtf8(" keywords:")+m_keyWords, traceLevel3);

    QProcess myProcess;
    const int timeOut = -1; // no timeout
    QString gFileName = myFileName();

    while (1)
    {
        for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
            if (!imageDownloadHelper[i]->isRunning())
                imageDownloadHelper[i]->terminate();

        newTask = false;
        QStringList arguments;

        deleteFile(gFileName);

        arguments.append(getKeyWord(m_keyWords));
        arguments.append(gFileName);

        myProcess.start("getGoogleHtml.bat", arguments );
        if (!myProcess.waitForStarted())
            trace(QString("Error.waitForStarted :getGoogleHtml.bat ")+arguments.join(" "), traceError);
        myProcess.waitForFinished(timeOut);
        if (myProcess.exitCode())
            trace(QString("Error.exitCode :getGoogleHtml.bat ")+arguments.join(" "), traceError);

        urls = parseGoogleHtml(gFileName);

        trace(QString("cImageDownloader::run urls.count = ")+QString::number(urls.count()), traceLevel2);

        progressMax = urls.count();
        progressValue = 0;
        emit sProgressRange(progressValue, progressMax);

        while (!newTask)
        {
            for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
            {
                if (!imageDownloadHelper[i]->isRunning()
                        && (!urls.isEmpty()))
                {
                    imageDownloadHelper[i]->getImage(urls.takeFirst());
                }
            }
            msleep(333);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::helpThreadFinished(bool success, const QPixmap& pixmap, const QString &url)
{
    emit sProgressValue(++progressValue);
    if (success)
        emit signalImage (pixmap, url);
}

