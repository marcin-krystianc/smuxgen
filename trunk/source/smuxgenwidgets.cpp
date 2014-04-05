
//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <stdexcept>
#include <QtGui>
#include <QTextEdit>
#include <QColorGroup>
#include <QPalette>
#include <QDateTime>
#include <Phonon>

#include "globalsmuxgentools.h"
#include "smuxgenwidgets.h"
#include "courseoptions.h"
#include "codeeditor.h"
#include "sapi.h"


/////////////////////////////////////////////////////////////////////////////
OptionsPage::OptionsPage(QWidget *parent)
   : QWidget(parent)
{
   QGroupBox *configGroup = new QGroupBox(tr("Choose options:"));
   m_oDoubleCheckBox = new QCheckBox(tr("Alternative"));
   m_oImageCheckBox = new QCheckBox(tr("Images"));
   m_oVoiceCheckBoxQ = new QCheckBox(tr("Lector (Questions)"));
   m_oVoiceCheckBoxA = new QCheckBox(tr("Lector (Answers)"));

   QVBoxLayout *checkLayout = new QVBoxLayout;
   checkLayout->addWidget(m_oDoubleCheckBox);
   checkLayout->addWidget(m_oImageCheckBox);

   QLabel *userLabel = new QLabel(tr("User:"));
   m_userCombo = new QComboBox;
   QStringList usersDbList;
   SuperMemoSQL::getAvailableDbList(&usersDbList);
   for (int i = 0; i<usersDbList.count(); ++i)
      m_userCombo->insertItem(0, usersDbList.at(i));
   m_userCombo->insertItem(0, "");

   QHBoxLayout *userLayout = new QHBoxLayout;
   userLayout->addWidget(userLabel);
   userLayout->addWidget(m_userCombo);

   QLabel *courseLabel = new QLabel(tr("Course:"));
   m_courseCombo = new QComboBox;
   QHBoxLayout *courseLayout = new QHBoxLayout;
   courseLayout->addWidget(courseLabel);
   courseLayout->addWidget(m_courseCombo);

   QLabel *subnameLabel = new QLabel(tr("Lesson"));
   m_subnameEdit = new QLineEdit;

   QHBoxLayout *subnameLayout = new QHBoxLayout;
   subnameLayout->addWidget(subnameLabel);
   subnameLayout->addWidget(m_subnameEdit);

   QLabel *instructionLabel = new QLabel(tr("Instruction"));
   m_instructionEdit = new QLineEdit;

   QHBoxLayout *instructionLayout = new QHBoxLayout;
   instructionLayout->addWidget(instructionLabel);
   instructionLayout->addWidget(m_instructionEdit);

   m_voiceComboQ = new QComboBox;
   m_voiceComboQ->insertItems(0, getVoiceEngines());
   m_voiceComboQ->insertItem(0, "");

   m_voiceGainQ = new QSpinBox;
   m_voiceTrimBeginQ = new QDoubleSpinBox;
   m_voiceTrimBeginQ->setSingleStep(0.1);
   m_voiceTrimBeginQ->setDecimals(1);

   m_voiceComboA = new QComboBox;
   m_voiceComboA->insertItems(0, getVoiceEngines());
   m_voiceComboA->insertItem(0, "");

   m_voiceGainA = new QSpinBox;
   m_voiceTrimBeginA = new QDoubleSpinBox;
   m_voiceTrimBeginA->setSingleStep(0.1);
   m_voiceTrimBeginA->setDecimals(1);

   m_voiceTesttextQ = new QLineEdit;
   m_voiceTestbuttonQ = new QPushButton(tr("Test this text below"));

   m_voiceTesttextA = new QLineEdit;
   m_voiceTestbuttonA = new QPushButton(tr("Test this text below"));

   m_voiceTesttextQ->setText("Test Testing Testen");
   QLabel *voiceIndexLabelQ = new QLabel(tr("Voice name"));
   QLabel *voiceGainLabelQ = new QLabel(tr("Gain [dB] "));
   QLabel *voiceTrimBeginLabelQ = new QLabel(tr("Trim [s]"));

   m_voiceTesttextA->setText("Test Testing Testen");
   QLabel *voiceIndexLabelA = new QLabel(tr("Voice name"));
   QLabel *voiceGainLabelA = new QLabel(tr("Gain [dB] "));
   QLabel *voiceTrimBeginLabelA = new QLabel(tr("Trim [s]"));

   QGridLayout *voiceLayoutQ = new QGridLayout;

   voiceLayoutQ->addWidget(m_oVoiceCheckBoxQ , 1 , 0 , 1 , 0);
   voiceLayoutQ->addWidget(voiceIndexLabelQ , 2 , 0);
   voiceLayoutQ->addWidget(m_voiceComboQ , 2 , 1);
   voiceLayoutQ->addWidget(voiceGainLabelQ , 3 , 0);
   voiceLayoutQ->addWidget(m_voiceGainQ , 3 , 1);
   voiceLayoutQ->addWidget(voiceTrimBeginLabelQ , 4 , 0);
   voiceLayoutQ->addWidget(m_voiceTrimBeginQ , 4 , 1);
   voiceLayoutQ->addWidget(m_voiceTestbuttonQ , 5 , 0 , 1 , 0);
   voiceLayoutQ->addWidget(m_voiceTesttextQ , 6 , 0 , 1 , 0);

   QGridLayout *voiceLayoutA = new QGridLayout;

   voiceLayoutA->addWidget(m_oVoiceCheckBoxA , 1 , 0 , 1 , 0);
   voiceLayoutA->addWidget(voiceIndexLabelA , 2 , 0);
   voiceLayoutA->addWidget(m_voiceComboA , 2 , 1);
   voiceLayoutA->addWidget(voiceGainLabelA , 3 , 0);
   voiceLayoutA->addWidget(m_voiceGainA , 3 , 1);
   voiceLayoutA->addWidget(voiceTrimBeginLabelA , 4 , 0);
   voiceLayoutA->addWidget(m_voiceTrimBeginA , 4 , 1);
   voiceLayoutA->addWidget(m_voiceTestbuttonA , 5 , 0 , 1 , 0);
   voiceLayoutA->addWidget(m_voiceTesttextA , 6 , 0 , 1 , 0);

   QVBoxLayout *configLayout = new QVBoxLayout;
   configLayout->addLayout(userLayout);
   configLayout->addLayout(courseLayout);
   configLayout->addLayout(subnameLayout);
   configLayout->addLayout(instructionLayout);
   configLayout->addSpacing(10);
   configLayout->addLayout(checkLayout);
   configLayout->addSpacing(10);
   configLayout->addLayout(voiceLayoutQ);
   configLayout->addSpacing(20);
   configLayout->addLayout(voiceLayoutA);
   configLayout->addStretch(1);
   configGroup->setLayout(configLayout);

   setLayout(configLayout);

   m_audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
   m_mediaObject = new Phonon::MediaObject (this);

   connect(m_userCombo , SIGNAL(currentIndexChanged(const QString &)) , this , SLOT(userChanged(const QString &)));
   connect(m_voiceTestbuttonQ, SIGNAL(clicked()) , this , SLOT(voiceTestButtonTriggered()));
   connect(m_voiceTestbuttonA, SIGNAL(clicked()) , this , SLOT(voiceTestButtonTriggered()));

   connect(m_oVoiceCheckBoxQ , SIGNAL(stateChanged (int)) , this , SLOT(voiceCheckBoxChangedQ(int)));
   connect(m_oVoiceCheckBoxA , SIGNAL(stateChanged (int)) , this , SLOT(voiceCheckBoxChangedA(int)));

   voiceCheckBoxChangedQ(m_oVoiceCheckBoxQ->checkState());
   voiceCheckBoxChangedA(m_oVoiceCheckBoxA->checkState());
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::voiceCheckBoxChangedQ (int state)
{
   switch (state)
   {
      case Qt::Unchecked:
         m_voiceComboQ-> setEnabled(false);
         m_voiceGainQ-> setEnabled(false);
         m_voiceTrimBeginQ-> setEnabled(false);
         m_voiceTesttextQ-> setEnabled(false);
         m_voiceTestbuttonQ-> setEnabled(false);
         break;

      case Qt::Checked:
         m_voiceComboQ-> setEnabled(true);
         m_voiceGainQ-> setEnabled(true);
         m_voiceTrimBeginQ-> setEnabled(true);
         m_voiceTesttextQ-> setEnabled(true);
         m_voiceTestbuttonQ-> setEnabled(true);
         break;
   }
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::voiceCheckBoxChangedA (int state)
{
   switch (state)
   {
      case Qt::Unchecked:
         m_voiceComboA-> setEnabled(false);
         m_voiceGainA-> setEnabled(false);
         m_voiceTrimBeginA-> setEnabled(false);
         m_voiceTesttextA-> setEnabled(false);
         m_voiceTestbuttonA-> setEnabled(false);
         break;

      case Qt::Checked:
         m_voiceComboA-> setEnabled(true);
         m_voiceGainA-> setEnabled(true);
         m_voiceTrimBeginA-> setEnabled(true);
         m_voiceTesttextA-> setEnabled(true);
         m_voiceTestbuttonA-> setEnabled(true);
         break;
   }
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::setOptions(const CourseOptions &options)
{
   m_oDoubleCheckBox->setCheckState(options.bothDirections ? Qt::Checked : Qt::Unchecked);
   m_oImageCheckBox->setCheckState(options.graphics ? Qt::Checked : Qt::Unchecked);
   m_oVoiceCheckBoxQ->setCheckState(!options.voiceNameQ.isEmpty() ? Qt::Checked : Qt::Unchecked);
   m_oVoiceCheckBoxA->setCheckState(!options.voiceNameA.isEmpty() ? Qt::Checked : Qt::Unchecked);

   m_courseCombo->clear();
   m_courseCombo->insertItem(0, options.courseName);
   m_subnameEdit->setText(options.subname);
   m_instructionEdit->setText(options.instruction);

   m_voiceComboQ->setCurrentIndex(m_voiceComboQ->findText(options.voiceNameQ));
   m_voiceGainQ->setValue(options.voiceGainQ);
   m_voiceTrimBeginQ->setValue(options.voiceTrimQ);

   m_voiceComboA->setCurrentIndex(m_voiceComboQ->findText(options.voiceNameA));
   m_voiceGainA->setValue(options.voiceGainA);
   m_voiceTrimBeginA->setValue(options.voiceTrimA);

   m_userCombo->setCurrentIndex(m_userCombo->findText(options.user));
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::voiceTestButtonTriggered ()
{
   CourseOptions options = getOptions();
   QString filePath = QDir::tempPath()+"\\test.mp3";

   if (sender() == m_voiceTestbuttonQ) {
      generateMp3 (filePath, m_voiceTesttextQ->text(), getVoiceEngineIndex(options.voiceNameQ)
                                    , options.voiceGainQ, options.voiceTrimQ);
   }
   else if (sender() == m_voiceTestbuttonA) {
      generateMp3 (filePath, m_voiceTesttextA->text(), getVoiceEngineIndex(options.voiceNameA)
                                    , options.voiceGainA, options.voiceTrimA);
   }
   else
      return;

   Phonon::createPath(m_mediaObject, m_audioOutput);
   m_mediaObject->setCurrentSource(Phonon::MediaSource(filePath));
   m_mediaObject->play();
}

/////////////////////////////////////////////////////////////////////////////
CourseOptions OptionsPage::getOptions()
{
   CourseOptions options;

   options.bothDirections = m_oDoubleCheckBox->isChecked();
   options.graphics = m_oImageCheckBox->isChecked();

   options.user = m_userCombo->currentText();
   options.subname = m_subnameEdit->text();
   options.instruction = m_instructionEdit->text();
   options.courseName = m_courseCombo->currentText();

   options.voiceNameQ = m_oVoiceCheckBoxQ->isChecked() ? m_voiceComboQ->currentText() : QString();
   options.voiceGainQ = m_voiceGainQ->value();
   options.voiceTrimQ = m_voiceTrimBeginQ->value();

   options.voiceNameA = m_oVoiceCheckBoxA->isChecked() ? m_voiceComboA->currentText() : QString();
   options.voiceGainA = m_voiceGainA->value();
   options.voiceTrimA = m_voiceTrimBeginA->value();

   return options;
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::userChanged(const QString &userName)
{
   m_userCombo->setPalette(m_subnameEdit->palette()); // fileEdit to default colour
   if (!m_superDb.openUser(userName)) {
      QPalette palette = m_userCombo->palette();
      palette.setColor(QPalette::Text, Qt::red); // fg
      m_userCombo->setPalette(palette);
      return;
   }

   QStringList courseList;
   try {
      m_superDb.getCourses(&courseList);
   }
   catch (const std::exception &e) {
      trace(e.what(), traceError);
   }

   QString oldText = m_courseCombo->currentText();
   m_courseCombo->clear();
   for (int i = 0; i<courseList.count(); ++i)
      m_courseCombo->insertItem(0, courseList.at(i));

   int pos = m_courseCombo->findText(oldText);

   if (pos == -1) { // text not found
      m_courseCombo->insertItem(0, QIcon(":/images/warning.png"), oldText);
      m_courseCombo->setCurrentIndex(0);
   }
   else
      m_courseCombo->setCurrentIndex(pos);

   return;
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::trace (const QString &text, const int & flags)
{
   globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ConsolePage::ConsolePage(QWidget *parent)
   : QWidget(parent)
{
   m_consoleText = new QTextEdit;
   QVBoxLayout *mainLayout = new QVBoxLayout;

   QHBoxLayout *bLayout = new QHBoxLayout;
   m_traceLevel2 = new QCheckBox("Trace 2");
   m_traceLevel3 = new QCheckBox("Trace 3");
   bLayout->addWidget(m_traceLevel2);
   bLayout->addWidget(m_traceLevel3);
   bLayout->addStretch(1);

   mainLayout->addWidget(m_consoleText);
   mainLayout->addLayout(bLayout);
   setLayout(mainLayout);

   m_consoleText->setReadOnly(true);

   connect(m_traceLevel2 , SIGNAL(stateChanged (int)) , this , SLOT(traceLevelSlot()));
   connect(m_traceLevel3 , SIGNAL(stateChanged (int)) , this , SLOT(traceLevelSlot()));
}

/////////////////////////////////////////////////////////////////////////////
void ConsolePage::traceLevelSlot ()
{
   unsigned int flags = traceError|traceWarning|traceLevel1;

   if (m_traceLevel2->checkState() == Qt::Checked)
      flags |= traceLevel2;

   if (m_traceLevel3->checkState() == Qt::Checked)
      flags |= traceLevel3;

   globalTracer.setTraceFlags(flags);
}

/////////////////////////////////////////////////////////////////////////////
void ConsolePage::traceSlot(const QString &txt, const int & flags)
{
   m_traceMutex.lock();

   QDateTime dateTime = QDateTime::currentDateTime ();

   m_consoleText->setTextColor(Qt::black);
   if (flags & traceWarning)
      m_consoleText->setTextColor(Qt::darkMagenta);
   if (flags & traceError)
      m_consoleText->setTextColor(Qt::red);

   m_consoleText->append(dateTime.toString("hh:mm:ss: ")+txt);

   m_traceMutex.unlock();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ContentPage::ContentPage(QWidget *parent)
   : QWidget(parent)
{
   m_content = new ContentTable;
   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addWidget(m_content);
   setLayout(mainLayout);

   connect (m_content, SIGNAL(contentChanged()), this, SLOT(contentChangedSlot()));
}

/////////////////////////////////////////////////////////////////////////////
void ContentPage::setContent (const std::vector<ContentItem> &content)
{
   m_content->fromCourseTemplate (content);
}

/////////////////////////////////////////////////////////////////////////////
std::vector<ContentItem> ContentPage::getContent ()
{
   std::vector<ContentItem> content;
   m_content->toCourseTemplate(&content);
   return content;
}

/////////////////////////////////////////////////////////////////////////////
void ContentPage::contentChangedSlot()
{
   emit contentChangedSignal();
}


