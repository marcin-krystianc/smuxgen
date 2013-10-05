//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QString>
#include <QDir>
#include <QPixmap>

#include "imagedownloader.h"
#include "globaltracer.h"
#include "globalsmuxgentools.h"

/////////////////////////////////////////////////////////////////////////////
ImageDownloadHelper::ImageDownloadHelper(const QString &ext, int id)
{
   m_id = id;
   m_ext = ext;
}

/////////////////////////////////////////////////////////////////////////////
ImageDownloadHelper::~ImageDownloadHelper()
{
   deleteFile(myFileName());
}

/////////////////////////////////////////////////////////////////////////////
QString ImageDownloadHelper::myFileName()
{
   return TMPDIR+"IMG"+m_ext+".jpg";
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloadHelper::getImage(const QString &url)
{
   m_url = url;
   trace(QString("cImageDownloadHelper::getImage id:")+QString::number(m_id)+QString::fromUtf8(" url:")+url, traceLevel3);

   start();
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloadHelper::run()
{
   trace(QString("cImageDownloadHelper::run id:")+QString::number(m_id)+QString::fromUtf8(" url:")+m_url, traceLevel3);


   QString fileName = myFileName();
   deleteFile(fileName);

   QStringList arguments;
   arguments.append(m_url+QString(" "));
   arguments.append(fileName+" ");
   runExternalTool("getImage.bat", arguments);

   if (!scalePicture(fileName, IMG_WIDTH, IMG_HEIGHT)) {
      trace(QString("Error:scalePicture ")+fileName, traceError);
      deleteFile(fileName);
   }

   if (checkIsFileOk(fileName)) {
      QPixmap pixmap(fileName);
      emit finished (true, pixmap, m_id, m_url);
   }
   else {
      QPixmap pixmap;
      emit finished (false, pixmap, m_id, m_url);
   };
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloadHelper::trace (const QString &text, const int & flags)
{
   globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ImageDownloader::ImageDownloader(const QString &id)
{
   m_id = id;
   for (int i = 0; i<ImageDownloader::m_maxHelpThreads; ++i)
   {
      m_imageDownloadHelper[i] = new ImageDownloadHelper(m_id+QString::number(i), i);
      connect(m_imageDownloadHelper[i], SIGNAL(finished(bool, const QPixmap& , int, const QString &)) , this , SLOT(helpThreadFinished(bool, const QPixmap& , const QString &)));
   }
}

/////////////////////////////////////////////////////////////////////////////
ImageDownloader::~ImageDownloader()
{
   deleteFile(myFileName());
}

/////////////////////////////////////////////////////////////////////////////
QString ImageDownloader::myFileName ()
{
   return TMPDIR+"HTML"+m_id;
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloader::trace (const QString &text, const int & flags)
{
   globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloader::getImages(const QString &keyWords)
{
   m_keyWords = keyWords;
   m_newTask = true;
   start();
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloader::run ()
{
   trace(QString("cImageDownloader::run id:")+m_id+QString::fromUtf8(" keywords:")+m_keyWords, traceLevel3);

   QString gFileName = myFileName();

   for (;;) {

      for (int i = 0; i<ImageDownloader::m_maxHelpThreads; ++i) {
         if (!m_imageDownloadHelper[i]->isRunning())
            m_imageDownloadHelper[i]->terminate();
      }

      m_newTask = false;
      deleteFile(gFileName);

      QStringList arguments;
      arguments.append(getKeyWord(m_keyWords));
      arguments.append(gFileName);
      runExternalTool("getGoogleHtml.bat", arguments);

      m_urls = parseGoogleHtml(gFileName);

      trace(QString("cImageDownloader::run urls.count = ")+QString::number(m_urls.count()), traceLevel2);

      m_progressMax = m_urls.count();
      m_progressValue = 0;
      emit sProgressRange(m_progressValue, m_progressMax);

      while (!m_newTask) {
         for (int i = 0; i<ImageDownloader::m_maxHelpThreads; ++i) {
            if (!m_imageDownloadHelper[i]->isRunning()
                && (!m_urls.isEmpty())) {
               m_imageDownloadHelper[i]->getImage(m_urls.takeFirst());
            }
         }
         msleep(333);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
void ImageDownloader::helpThreadFinished(bool success, const QPixmap& pixmap, const QString &url)
{
   emit sProgressValue(++m_progressValue);
   if (success)
      emit signalImage (pixmap, url);
}

