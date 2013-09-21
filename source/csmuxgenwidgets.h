//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
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
#include <QToolButton>
#include <QPlainTextEdit>

#include "coursetemplateoptions.h"
#include "csupermemosql.h"
#include "cglobaltracer.h"
#include "cimagedownloader.h"

/////////////////////////////////////////////////////////////////////////////
class cFindToolbar : public QWidget
{
    Q_OBJECT
public:
    cFindToolbar(QWidget *parent = 0);
    void setFindFocus();
private:
    QLineEdit *lineEdit;
    QPushButton *forwardButton;
    QPushButton *backwardButton;

signals:
    void findNext (const QString &text);
    void findPrev (const QString &text);

private slots:
    void nextSlot();
    void prevSlot();

};

/////////////////////////////////////////////////////////////////////////////
class cOptionsPage : public QWidget
{
    Q_OBJECT
public:
    cOptionsPage(QWidget *parent = 0);
    void setOptions(const CourseTemplateOptions &options);
    CourseTemplateOptions getOptions ();

private:
    QCheckBox *oForceCheckBox ;
    QCheckBox *oDoubleCheckBox;
    QCheckBox *oImageCheckBox ;
    QCheckBox *oVoiceCheckBoxQ ;
    QCheckBox *oVoiceCheckBoxA ;

    QLineEdit *fileEdit ;
    QComboBox *courseCombo;
    QLineEdit *subnameEdit;
    QLineEdit *instructionEdit;

    QComboBox *voiceComboQ;
    QSpinBox *voiceGainQ;
    QDoubleSpinBox *voiceTrimBeginQ;
    QLineEdit *voiceTesttextQ;
    QPushButton *voiceTestbuttonQ;

    QComboBox *voiceComboA;
    QSpinBox *voiceGainA;
    QDoubleSpinBox *voiceTrimBeginA;
    QLineEdit *voiceTesttextA;
    QPushButton *voiceTestbuttonA;

private:
    SuperMemoSQL superMemoSQL;
    void trace (const QString &text, const int & flags = traceLevel1|0);

    Phonon::AudioOutput *audioOutput;
    Phonon::MediaObject *mediaObject;

private slots:
    void fileButtonTriggered ();
    void fileEditChanged (const QString &fileName);
    void voiceCheckBoxChangedQ (int);
    void voiceCheckBoxChangedA (int);
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
    void traceSlot(const QString &txt, const int & flags);
    void traceLevelSlot ();

private:
    QTextEdit *consoleText;
    QMutex traceMutex;
    QCheckBox *cTracelevel2; // checkbox for tracelevel2
    QCheckBox *cTracelevel3; // checkbox for tracelevel3

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
    QPlainTextEdit *contentTextEdit;
    cFindToolbar *findToolbar;
private slots:
    void findNext(const QString &txt);
    void findPrev(const QString &txt);

    void contentChangedSlot();
    void keyPressEvent ( QKeyEvent * event );

};


#endif // CSMUXGENWIDGETS_H
