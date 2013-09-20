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
    this->ID = id;
    this->EXT = ext;
}

/////////////////////////////////////////////////////////////////////////////
cImageDownloadHelper::~cImageDownloadHelper()
{
    deleteFile(this->myFileName());
}

/////////////////////////////////////////////////////////////////////////////
QString cImageDownloadHelper::myFileName()
{
    return tmpDir+"IMG"+this->EXT+".jpg";
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloadHelper::getImage(const QString &url)
{
    this->url = url;
    trace(QString("cImageDownloadHelper::getImage id:")+QString::number(this->ID)+QString::fromUtf8(" url:")+this->url, traceLevel3);

    this->start();
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloadHelper::run()
{
    trace(QString("cImageDownloadHelper::run id:")+QString::number(this->ID)+QString::fromUtf8(" url:")+this->url, traceLevel3);

    QProcess myProcess;
    const int timeOut = -1; // no timeout
    QString fileName = this->myFileName();

    QStringList arguments;

    deleteFile(fileName);

    arguments.append(this->url+QString(" "));
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
        emit this->finished (true, pixmap, this->ID, this->url);
    }
    else
    {
        QPixmap pixmap;
        emit this->finished (false, pixmap, this->ID, this->url);
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
    this->ID = id;
    for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
    {
        imageDownloadHelper[i] = new cImageDownloadHelper(this->ID+QString::number(i), i);
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

    deleteFile(this->myFileName());
}

/////////////////////////////////////////////////////////////////////////////
QString cImageDownloader::myFileName ()
{
    return tmpDir+"HTML"+this->ID;
}


/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::trace (const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::getImages(const QString &keyWords)
{
    this->keyWords = keyWords;
    this->newTask = true;
    this->start();
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::run ()
{

    trace(QString("cImageDownloader::run id:")+this->ID+QString::fromUtf8(" keywords:")+this->keyWords, traceLevel3);

    QProcess myProcess;
    const int timeOut = -1; // no timeout
    QString gFileName = this->myFileName();

    while (1)
    {
        for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
            if (!imageDownloadHelper[i]->isRunning())
                imageDownloadHelper[i]->terminate();

        this->newTask = false;
        QStringList arguments;

        deleteFile(gFileName);

        arguments.append(getKeyWord(keyWords));
        arguments.append(gFileName);

        myProcess.start("getGoogleHtml.bat", arguments );
        if (!myProcess.waitForStarted())
            trace(QString("Error.waitForStarted :getGoogleHtml.bat ")+arguments.join(" "), traceError);
        myProcess.waitForFinished(timeOut);
        if (myProcess.exitCode())
            trace(QString("Error.exitCode :getGoogleHtml.bat ")+arguments.join(" "), traceError);

        this->urls = parseGoogleHtml(gFileName);

        trace(QString("cImageDownloader::run urls.count = ")+QString::number(this->urls.count()), traceLevel2);

        this->progressMax = this->urls.count();
        this->progressValue = 0;
        emit sProgressRange(this->progressValue, this->progressMax);

        while (!this->newTask)
        {
            for (int i = 0;i<cImageDownloader::maxHelpThreads;++i)
            {
                if (!imageDownloadHelper[i]->isRunning()
                        && (!this->urls.isEmpty()))
                {
                    imageDownloadHelper[i]->getImage(this->urls.takeFirst());
                }
            }
            msleep(333);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void cImageDownloader::helpThreadFinished(bool success, const QPixmap& pixmap, const QString &url)
{
    emit sProgressValue(++this->progressValue);
    if (success)
        emit this->signalImage (pixmap, url);
}

