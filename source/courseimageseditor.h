//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
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

#include "coursetemplateoptions.h"
#include "csupermemosql.h"
#include "cglobaltracer.h"
#include "cimagedownloader.h"
#include "coursetemplate.h"

#ifndef COURSEIMAGESEDITOR_H
#define COURSEIMAGESEDITOR_H


/////////////////////////////////////////////////////////////////////////////
class cImageList : public QListWidget
{
    Q_OBJECT
    public:

        cImageList(QWidget *parent = 0, int maxCount = 128);
        void addPiece(const QPixmap &pixmap, const QString &hint="");
        void resetPosition();
    public slots:
        void addPieceSlot       (const QPixmap &, const QString &hint="");
        void setIconSizeSlot    (int size );

    private:
        int maxCount;
        int rowIndex;

        QString tileMimeFormat () {return QString::fromUtf8("image/x-smuxgen");}

        static const unsigned int tileSizeX = 100;
        static const unsigned int tileSizeY = 100;

        void trace (const QString &text, const int & flags = traceLevel1|0);

    protected:
        /*
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);
        */
        void startDrag(Qt::DropActions supportedActions);

};

/////////////////////////////////////////////////////////////////////////////
class cImageSearch : public QWidget
{
    Q_OBJECT
    public:
        cImageSearch(QWidget *parent = 0);
        ~cImageSearch();


    public slots:
        void setNewKeywordsChangedL   (const QString &txt);
        void setNewKeywordsChangedR   (const QString &txt);
        void newKeywordsChangedL   (const QString &txt);
        void newKeywordsChangedR   (const QString &txt);
        void newKeywordsL   ();
        void newKeywordsR   ();

    private:
        QLineEdit       *leftEdit;
        QLineEdit       *rightEdit;
        QProgressBar    *leftProgress;
        QProgressBar    *rightProgress;
        QTimer          *timerL;
        QTimer          *timerR;

        QSlider      *zoomSlider;

        QString textL;
        QString textR;

        cImageList  *imagelist;
        cImageDownloader *imageDownloader[2];
        void newKeywords    (const QString &txt, int id);

        void trace (const QString &text, const int & flags = traceLevel1|0);


    protected:

};

/////////////////////////////////////////////////////////////////////////////
class cImageButtonWidget : public QWidget
{
    Q_OBJECT
    public:
        cImageButtonWidget (QWidget *parent = 0);
        bool setFile (const QString &path);

    private:

        QLabel   *label;
        QPixmap pixmap;

        QString filePath;
        QString tileMimeFormat () {return QString::fromUtf8("image/x-smuxgen");}
        void trace (const QString &text, const int & flags = traceLevel1|0);
        void setPixmap (const QPixmap &pixmap);

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);
        void resizeEvent (QResizeEvent * event) ;
};

/////////////////////////////////////////////////////////////////////////////
class cImageTargetWidget : public QWidget
{
    Q_OBJECT
    public:
        cImageTargetWidget (QWidget *parent = 0);
        void setFiles (const QStringList &list);

    private:

        cImageButtonWidget   *imageButtonWidget[2][2];

    protected:
        void resizeEvent ( QResizeEvent * event ) ;
};

/////////////////////////////////////////////////////////////////////////////
class cReadyCourseElementList : public QWidget
{
    Q_OBJECT
    public:
        cReadyCourseElementList (QWidget *parent = 0);
        void clear();
        void addItem (const QString &text, const QStringList &imgData, const QStringList &mp3Data);

    signals:
        void elementSelectedImgSignal   (const QStringList &list);
        void elementSelectedMP3Signal   (const QStringList &list);

    private:
        QListWidget *listWidget;

    private slots:
        void itemActivatedSlot   ( QListWidgetItem * item );

    protected:
};

/////////////////////////////////////////////////////////////////////////////
class cMp3Widget : public QWidget
{
    Q_OBJECT
    public:
        cMp3Widget (QWidget *parent = 0);
        void setData (const QString &label, const QString &path);

    private:
        QLabel          *label;
        QPushButton     *playButton;
        QPushButton     *openButton;
        QString         filePath;

        Phonon::AudioOutput *audioOutput;
        Phonon::MediaObject *mediaObject;
    private slots:
       void play();
       void openFile();
};

/////////////////////////////////////////////////////////////////////////////
class cMp3TargetWidget : public QWidget
{
    Q_OBJECT
    public:
        cMp3TargetWidget (QWidget *parent = 0);
        void setData (const QStringList &list);

    private:
        cMp3Widget   *mp3Widget[4];

    private slots:
        void elementSelectedMp3Slot (const QStringList& mp3Data);
};

/////////////////////////////////////////////////////////////////////////////
// widget which allows to view and modify images for each exercise
class cCourseImageEditor : public QWidget
{
    Q_OBJECT
    public:
        cCourseImageEditor (QWidget *parent = 0);
        void workWith (const cCourseTemplate &courseTemplate);

    private:
        void clear ();
        void trace (const QString &text, const int & flags = traceLevel1|0);

        cSuperMemoSQL           database;

        cReadyCourseElementList *readyCourseElementList;
        cImageTargetWidget      *imageTargetWidget;
        cImageSearch            *imageSearch;
        cMp3TargetWidget        *mp3TargetWidget;

    private slots:
        void elementSelectedImgSlot (const QStringList &imgData);

protected:
};


#endif // COURSEIMAGESEDITOR_H
