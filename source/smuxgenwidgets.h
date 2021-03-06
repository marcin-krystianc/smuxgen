//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CSMUXGENWIDGETS_H
#define CSMUXGENWIDGETS_H

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>
#include <QObject>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QTimer>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QPlainTextEdit>
#include <QMutex>

#include "courseoptions.h"
#include "supermemosql.h"
#include "globaltracer.h"
#include "coursetemplate.h"

class ContentTable;


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

   QLineEdit *m_graphicsSearchUrl;
   QLineEdit *m_graphicsRegex;

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
   void setContent (const std::vector<ContentItem> &content);
   std::vector<ContentItem> getContent();

signals:
   void contentChangedSignal ();

private:
   ContentTable *m_content;

private slots:
   void contentChangedSlot();
};


#endif // CSMUXGENWIDGETS_H
