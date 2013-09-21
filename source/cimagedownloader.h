//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#ifndef CIMAGEDOWNLOADER_H
#define CIMAGEDOWNLOADER_H

#include <QThread>
#include <QString>
#include <QPixmap>
#include <QMutex>

#include "cglobaltracer.h"


/////////////////////////////////////////////////////////////////////////////
class cImageDownloadHelper : public QThread
{
    Q_OBJECT
public:
    cImageDownloadHelper(const QString &ext, int id);
    ~cImageDownloadHelper();

    void getImage(const QString &url);

public slots:
    //void stop();

private:
    int m_id;
    QString m_url;
    QString m_ext;

    QString myFileName();
    void trace (const QString &text, const int & flags = traceLevel1|0);

protected:
    void run();

signals:
    void finished (bool success, const QPixmap& pixmap, int id, const QString &url);
};

/////////////////////////////////////////////////////////////////////////////
class cImageDownloader : public QThread
{
    Q_OBJECT
public:
    cImageDownloader(const QString &id = "");
    ~cImageDownloader();

    void getImages(const QString &keyWords);

public slots:
    void helpThreadFinished(bool success, const QPixmap& pixmap, const QString &url);

private:
    QString m_id;
    QString m_keyWords;
    QStringList m_urls;
    volatile bool m_newTask;

    int m_progressMax;
    int m_progressValue;

    static const int m_maxHelpThreads = 8;
    cImageDownloadHelper *m_imageDownloadHelper[m_maxHelpThreads];
    QString myFileName ();

    void trace (const QString &text, const int & flags = traceLevel1|0);

protected:
    void run();

signals:
    void signalImage (const QPixmap& pixmap, const QString &url);
    void sProgressRange (int min, int max);
    void sProgressValue (int value);
};


#endif // CIMAGEDOWNLOADER_H
