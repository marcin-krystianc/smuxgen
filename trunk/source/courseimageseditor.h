//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QObject>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <Phonon>
#include <QListWidget>
#include <QTimer>
#include <QProgressBar>
#include <QToolButton>
#include <QLabel>
#include <QScrollBar>

#include "courseoptions.h"
#include "supermemosql.h"
#include "globaltracer.h"
#include "imagedownloader.h"
#include "coursetemplate.h"

#ifndef COURSEIMAGESEDITOR_H
#define COURSEIMAGESEDITOR_H


/////////////////////////////////////////////////////////////////////////////
class ImageList : public QListWidget
{
   Q_OBJECT
public:
   ImageList(QWidget *parent = 0, int m_maxCount = 128);
   void addPiece(const QPixmap &pixmap, const QString &hint = "");
   void resetPosition();

public slots:
   void addPieceSlot (const QPixmap &, const QString &hint = "");
   void setIconSizeSlot (int size);

private:
   QString tileMimeFormat () {return QString::fromUtf8("image/x-smuxgen"); }
   void trace (const QString &text, const int & flags = traceLevel1|0);

   int m_maxCount;
   int m_rowIndex;

   enum TILE_SIZE {
      e_tileSizeX = 100,
      e_tileSizeY = 100
   };

protected:
   void startDrag(Qt::DropActions supportedActions);
};

/////////////////////////////////////////////////////////////////////////////
class ImageSearch : public QWidget
{
   Q_OBJECT
public:
   ImageSearch(QWidget *parent = 0);
   ~ImageSearch();

public slots:
   void setNewKeywordsChangedL (const QString &txt);
   void setNewKeywordsChangedR (const QString &txt);
   void newKeywordsChangedL (const QString &txt);
   void newKeywordsChangedR (const QString &txt);
   void newKeywordsL ();
   void newKeywordsR ();

private:
   void newKeywords (const QString &txt, int id);
   void trace (const QString &text, const int & flags = traceLevel1|0);

   QLineEdit *m_leftEdit;
   QLineEdit *m_rightEdit;
   QProgressBar *m_leftProgress;
   QProgressBar *m_rightProgress;
   QTimer *m_timerL;
   QTimer *m_timerR;
   QSlider *m_zoomSlider;
   QString m_textL;
   QString m_textR;
   ImageList *m_imagelist;
   ImageDownloader *m_imageDownloader[2];
};

/////////////////////////////////////////////////////////////////////////////
class ImageButtonWidget : public QWidget
{
   Q_OBJECT
public:
   ImageButtonWidget (QWidget *parent = 0);
   bool setFile (const QString &path);

private:
   QLabel *m_label;
   QPixmap m_pixmap;

   QString m_filePath;
   QString tileMimeFormat () {return QString::fromUtf8("image/x-smuxgen"); }
   void trace (const QString &text, const int & flags = traceLevel1|0);
   void setPixmap (const QPixmap &m_pixmap);

protected:
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void resizeEvent (QResizeEvent * event) ;
};

/////////////////////////////////////////////////////////////////////////////
class ImageTargetWidget : public QWidget
{
   Q_OBJECT
public:
   ImageTargetWidget (QWidget *parent = 0);
   void setFiles (const QStringList &list);

private:
   ImageButtonWidget *imageButtonWidget[2][2];

protected:
   void resizeEvent (QResizeEvent * event) ;
};

/////////////////////////////////////////////////////////////////////////////
class ReadyCourseElementList : public QWidget
{
   Q_OBJECT
public:
   ReadyCourseElementList (QWidget *parent = 0);
   void clear();
   void addItem (const QString &text, const QStringList &imgData, const QStringList &mp3Data);

signals:
   void elementSelectedImgSignal (const QStringList &list);
   void elementSelectedMP3Signal (const QStringList &list);

private:
   QListWidget *m_listWidget;

private slots:
   void itemActivatedSlot (QListWidgetItem * item);
};

/////////////////////////////////////////////////////////////////////////////
class Mp3Widget : public QWidget
{
   Q_OBJECT
public:
   Mp3Widget (QWidget *parent = 0);
   void setData (const QString &label, const QString &path);

private:
   QLabel *m_label;
   QString m_filePath;
   QPushButton *m_playButton;
   QPushButton *m_openButton;
   Phonon::AudioOutput *m_audioOutput;
   Phonon::MediaObject *m_mediaObject;

private slots:
   void play();
   void openFile();
};

/////////////////////////////////////////////////////////////////////////////
class Mp3TargetWidget : public QWidget
{
   Q_OBJECT
public:
   Mp3TargetWidget (QWidget *parent = 0);
   void setData (const QStringList &list);

private:
   Mp3Widget *m_mp3Widget[4];

private slots:
   void elementSelectedMp3Slot (const QStringList& mp3Data);
};

/////////////////////////////////////////////////////////////////////////////
// widget which allows to view and modify images for each exercise
class CourseImageEditor : public QWidget
{
   Q_OBJECT
public:
   CourseImageEditor (QWidget *parent = 0);
   void workWith (const CourseTemplate &courseTemplate);

private:
   void clear ();
   void trace (const QString &text, const int & flags = traceLevel1|0);

   SuperMemoSQL m_database;
   ReadyCourseElementList *m_readyCourseElementList;
   ImageTargetWidget *m_imageTargetWidget;
   ImageSearch *m_imageSearch;
   Mp3TargetWidget *m_mp3TargetWidget;

private slots:
   void elementSelectedImgSlot (const QStringList &imgData);
};


#endif // COURSEIMAGESEDITOR_H
