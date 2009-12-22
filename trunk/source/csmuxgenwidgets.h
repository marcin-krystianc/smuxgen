//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CSMUXGENWIDGETS_H
#define CSMUXGENWIDGETS_H

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

#include "coursetemplateoptions.h"
#include "csupermemosql.h"
#include "cglobaltracer.h"
#include "cimagedownloader.h"

/////////////////////////////////////////////////////////////////////////////
class cOptionsPage : public QWidget
{
    Q_OBJECT
    public:
        cOptionsPage(QWidget *parent = 0);
        void setOptions(const cCourseTemplateOptions &options);
        cCourseTemplateOptions getOptions ();

    private:
        QCheckBox *oForceCheckBox ;
        QCheckBox *oDoubleCheckBox;
        QCheckBox *oImageCheckBox ;
        QCheckBox *oVoiceCheckBox ;

        QLineEdit *fileEdit ;
        QComboBox *courseCombo;
        QLineEdit *subnameEdit;

        QSpinBox        *voiceIndex;
        QSpinBox        *voiceGain;
        QDoubleSpinBox  *voiceTrimBegin;

        QLineEdit       *voiceTesttext;
        QPushButton     *voiceTestbutton;
    private:
        cSuperMemoSQL superMemoSQL;
        void trace (const QString &text,const int & flags = traceLevel1|0);

        Phonon::AudioOutput *audioOutput;
        Phonon::MediaObject *mediaObject;

    private slots:
        void fileButtonTriggered ();
        void fileEditChanged (const QString &fileName);
        void voiceCheckBoxChanged (int);
        void voiceTestButtonTriggered ();
        void testFileRemoveSlot();
};

/////////////////////////////////////////////////////////////////////////////
class cConsolePage : public QWidget
{
    Q_OBJECT
    public:
        cConsolePage(QWidget *parent = 0);

    public slots:
        void traceSlot(const QString &txt,const int & flags);

    private:
        QTextEdit *consoleText;
        QMutex traceMutex;
};

/////////////////////////////////////////////////////////////////////////////
class cContentPage : public QWidget
{
    Q_OBJECT
    public:
        cContentPage(QWidget *parent = 0);
        void setContent (const QStringList & content);
        QStringList getContent ();
    signals:
        void contentChangedSignal ();

    private:
        QTextEdit *contentTextEdit;

    private slots:
        void contentChangedSlot();
};


/////////////////////////////////////////////////////////////////////////////
class cImageList : public QListWidget
{
    Q_OBJECT
    public:

        cImageList(QWidget *parent = 0,int maxCount = 128);
        void addPiece(const QPixmap &pixmap);
        void resetPosition();
    public slots:
        void addPieceSlot (const QPixmap &);

    private:
        int maxCount;
        int rowIndex;

        QString tileMimeFormat () {return QString::fromUtf8("image/x-smuxgen");}

        static const unsigned int tileSizeX = 100;
        static const unsigned int tileSizeY = 100;

        void trace (const QString &text,const int & flags = traceLevel1|0);

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);
        void startDrag(Qt::DropActions supportedActions);
};

/////////////////////////////////////////////////////////////////////////////
class cImageSearch : public QWidget
{
    Q_OBJECT
    public:
        cImageSearch(QWidget *parent = 0);

    public slots:
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

        QString textL;
        QString textR;

        cImageList  *imagelist;
        cImageDownloader *imageDownloader[2];
        void newKeywords    (const QString &txt,int id);

        void trace (const QString &text,const int & flags = traceLevel1|0);

    protected:

};

/////////////////////////////////////////////////////////////////////////////
class cImageWidget : public QWidget
{
    Q_OBJECT
    public:
        cImageWidget (QWidget *parent = 0);

    private:
        cImageSearch *imageSearch;
    protected:

};

#endif // CSMUXGENWIDGETS_H
