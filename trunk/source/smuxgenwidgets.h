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

#include "courseoptions.h"
#include "supermemosql.h"
#include "globaltracer.h"
#include "imagedownloader.h"

/////////////////////////////////////////////////////////////////////////////
class FindToolbar : public QWidget
{
   Q_OBJECT
public:
   FindToolbar(QWidget *parent = 0);
   void setFindFocus();

private:
   QLineEdit *m_lineEdit;
   QPushButton *m_forwardButton;
   QPushButton *m_backwardButton;

signals:
   void findNext (const QString &text);
   void findPrev (const QString &text);

private slots:
   void nextSlot();
   void prevSlot();
};

/////////////////////////////////////////////////////////////////////////////
class OptionsPage : public QWidget
{
   Q_OBJECT
public:
   OptionsPage(QWidget *parent = 0);
   void setOptions(const CourseOptions &options);
   CourseOptions getOptions ();

   QComboBox *getVoiceComboA() const;
   void setVoiceComboA(QComboBox *value);

private:
   QCheckBox *m_oDoubleCheckBox;
   QCheckBox *m_oImageCheckBox;
   QCheckBox *m_oVoiceCheckBoxQ;
   QCheckBox *m_oVoiceCheckBoxA;

   QComboBox *m_userCombo;
   QComboBox *m_courseCombo;
   QLineEdit *m_subnameEdit;
   QLineEdit *m_instructionEdit;

   QComboBox *m_voiceComboQ;
   QSpinBox *m_voiceGainQ;
   QDoubleSpinBox *m_voiceTrimBeginQ;
   QLineEdit *m_voiceTesttextQ;
   QPushButton *m_voiceTestbuttonQ;

   QComboBox *m_voiceComboA;
   QSpinBox *m_voiceGainA;
   QDoubleSpinBox *m_voiceTrimBeginA;
   QLineEdit *m_voiceTesttextA;
   QPushButton *m_voiceTestbuttonA;

private:
   SuperMemoSQL m_superDb;
   void trace (const QString &text, const int & flags = traceLevel1|0);

   Phonon::AudioOutput *m_audioOutput;
   Phonon::MediaObject *m_mediaObject;

private slots:
   void userChanged (const QString &userName);
   void voiceCheckBoxChangedQ (int);
   void voiceCheckBoxChangedA (int);
   void voiceTestButtonTriggered ();
};

/////////////////////////////////////////////////////////////////////////////
class ConsolePage : public QWidget
{
   Q_OBJECT
public:
   ConsolePage(QWidget *parent = 0);

public slots:
   void traceSlot(const QString &txt, const int & flags);
   void traceLevelSlot ();

private:
   QTextEdit *m_consoleText;
   QMutex m_traceMutex;
   QCheckBox *m_traceLevel2; // checkbox for tracelevel2
   QCheckBox *m_traceLevel3; // checkbox for tracelevel3
};

/////////////////////////////////////////////////////////////////////////////
class ContentPage : public QWidget
{
   Q_OBJECT
public:
   ContentPage(QWidget *parent = 0);
   void setContent (const QStringList & content);
   QStringList getContent ();

signals:
   void contentChangedSignal ();

private:
   QPlainTextEdit *n_contentTextEdit;
   FindToolbar *m_findToolbar;

private slots:
   void findNext(const QString &txt);
   void findPrev(const QString &txt);

   void contentChangedSlot();
   void keyPressEvent (QKeyEvent * event);
};


#endif // CSMUXGENWIDGETS_H
