
//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QtGui>
#include <QTextEdit>
#include <QColorGroup>
#include <QPalette>
#include <QDateTime>
#include <Phonon>

#include "csmuxgenwidgets.h"
#include "courseoptions.h"
#include "ccodeeditor.h"
#include "csapi.h"

/////////////////////////////////////////////////////////////////////////////
OptionsPage::OptionsPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *configGroup = new QGroupBox(tr("Choose options:"));
    m_oForceCheckBox = new QCheckBox(tr("Regenerate"));
    m_oDoubleCheckBox = new QCheckBox(tr("Alternative"));
    m_oImageCheckBox = new QCheckBox(tr("Images"));
    m_oVoiceCheckBoxQ = new QCheckBox(tr("Lector (Questions)"));
    m_oVoiceCheckBoxA = new QCheckBox(tr("Lector (Answers)"));
    
    QGridLayout *checkLayout = new QGridLayout;
    checkLayout->addWidget(m_oForceCheckBox , 0 , 0);
    checkLayout->addWidget(m_oDoubleCheckBox , 0 , 1);
    checkLayout->addWidget(m_oImageCheckBox , 1 , 0);
    
    QLabel *fileLabel = new QLabel(tr("Database file:"));
    m_fileEdit = new QLineEdit;
    QPushButton *fileChooseButton = new QPushButton(tr("Choose"));
    m_fileEdit->setReadOnly(true);
    
    QGridLayout *fileLayout = new QGridLayout;
    fileLayout->addWidget(fileLabel, 0, 0);
    fileLayout->addWidget(fileChooseButton, 0, 1);
    fileLayout->addWidget(m_fileEdit, 1, 0, 1, 0);
    
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
    m_voiceGainQ = new QSpinBox;
    m_voiceTrimBeginQ = new QDoubleSpinBox;
    m_voiceTrimBeginQ->setSingleStep(0.1);
    m_voiceTrimBeginQ->setDecimals(1);
    
    m_voiceComboA = new QComboBox;
    m_voiceComboA->insertItems(0, getVoiceEngines());
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
    /*
 QFrame *frameQ = new QFrame;
 frameQ->setFrameStyle(QFrame::HLine);
 voiceLayoutQ->addWidget(frameQ , 0 , 0 , 1 , 0);
 */
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
    /*
 QFrame *frameA = new QFrame;
 frameA->setFrameStyle(QFrame::HLine);
 voiceLayoutA->addWidget(frameA , 0 , 0 , 1 , 0);
 */
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
    configLayout->addLayout(fileLayout);
    configLayout->addSpacing(10);
    configLayout->addLayout(courseLayout);
    configLayout->addLayout(subnameLayout);
    configLayout->addLayout(instructionLayout);
    configLayout->addSpacing(10);
    configLayout->addLayout(checkLayout);
    configLayout->addSpacing(20);
    configLayout->addLayout(voiceLayoutQ);
    configLayout->addSpacing(20);
    configLayout->addLayout(voiceLayoutA);
    configLayout->addStretch(1);
    configGroup->setLayout(configLayout);
    
    setLayout(configLayout);
    
    m_audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
    m_mediaObject = new Phonon::MediaObject (this);
    
    connect(fileChooseButton, SIGNAL(clicked()) , this , SLOT(fileButtonTriggered()));
    connect(m_fileEdit , SIGNAL(textChanged(const QString & )) , this , SLOT(fileEditChanged(const QString &)));
    connect(m_voiceTestbuttonQ, SIGNAL(clicked()) , this , SLOT(voiceTestButtonTriggered()));
    connect(m_voiceTestbuttonA, SIGNAL(clicked()) , this , SLOT(voiceTestButtonTriggered()));
    
    connect(m_oVoiceCheckBoxQ , SIGNAL(stateChanged (int )) , this , SLOT(voiceCheckBoxChangedQ(int)));
    connect(m_oVoiceCheckBoxA , SIGNAL(stateChanged (int )) , this , SLOT(voiceCheckBoxChangedA(int)));
    
    connect(m_mediaObject , SIGNAL(finished ()) , this , SLOT(testFileRemoveSlot()));
    
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
    m_oForceCheckBox ->setCheckState(options.force ? Qt::Checked : Qt::Unchecked);
    m_oDoubleCheckBox ->setCheckState(options.bothDirections ? Qt::Checked : Qt::Unchecked);
    m_oImageCheckBox ->setCheckState(options.graphics ? Qt::Checked : Qt::Unchecked);
    m_oVoiceCheckBoxQ ->setCheckState(options.voiceQ ? Qt::Checked : Qt::Unchecked);
    m_oVoiceCheckBoxA ->setCheckState(options.voiceA ? Qt::Checked : Qt::Unchecked);
    
    m_fileEdit->setText(options.dbPath);
    m_courseCombo->clear();
    m_courseCombo->insertItem(0, options.courseName);
    m_subnameEdit->setText(options.subname);
    m_instructionEdit->setText(options.instruction);
    
    m_voiceComboQ ->setCurrentIndex(getVoiceEngineIndex(options.voiceNameQ));
    m_voiceGainQ ->setValue(options.voiceGainQ);
    m_voiceTrimBeginQ->setValue(options.voiceTrimQ);
    
    m_voiceComboA ->setCurrentIndex(getVoiceEngineIndex(options.voiceNameA));
    m_voiceGainA ->setValue(options.voiceGainA);
    m_voiceTrimBeginA->setValue(options.voiceTrimA);
    
    fileEditChanged(m_fileEdit->text());
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::voiceTestButtonTriggered ()
{
    CourseOptions options = getOptions();
    QStringList arguments; // filename, text, trim, gain
    
    if (sender() == m_voiceTestbuttonQ)
    {
        arguments.append("test");
        arguments.append(m_voiceTesttextQ->text());
        arguments.append(QString::number(getVoiceEngineIndex(options.voiceNameQ)+1));
        arguments.append(QString::number(options.voiceTrimQ));
        arguments.append(QString::number(options.voiceGainQ));
    }
    else if (sender() == m_voiceTestbuttonA)
    {
        arguments.append("test");
        arguments.append(m_voiceTesttextA->text());
        arguments.append(QString::number(getVoiceEngineIndex(options.voiceNameA)+1));
        arguments.append(QString::number(options.voiceTrimA));
        arguments.append(QString::number(options.voiceGainA));
    }
    else
        return;
    
    trace(QString("createMp3.bat ")+arguments.join(" "), traceLevel1);
    QProcess myProcess;
    
    myProcess.start("createMp3.bat", arguments );
    if (!myProcess.waitForStarted())
    {
        trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
        return;
    }
    myProcess.waitForFinished();
    if (myProcess.exitCode())
    {
        trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
        return;
    }
    
    Phonon::createPath(m_mediaObject, m_audioOutput);
    m_mediaObject->setCurrentSource(Phonon::MediaSource("test.mp3"));
    m_mediaObject->play();
    
}
/////////////////////////////////////////////////////////////////////////////
CourseOptions OptionsPage::getOptions()
{
    CourseOptions options;
    options.force = m_oForceCheckBox->isChecked();
    options.bothDirections = m_oDoubleCheckBox->isChecked();
    options.graphics = m_oImageCheckBox->isChecked();
    options.voiceQ = m_oVoiceCheckBoxQ->isChecked();
    options.voiceA = m_oVoiceCheckBoxA->isChecked();
    
    options.dbPath = m_fileEdit->text();
    options.subname = m_subnameEdit->text();
    options.instruction = m_instructionEdit->text();
    
    options.courseName = m_courseCombo->currentText();
    
    options.voiceNameQ = m_voiceComboQ->currentText();
    options.voiceGainQ = m_voiceGainQ->value();
    options.voiceTrimQ = m_voiceTrimBeginQ->value();
    
    options.voiceNameA = m_voiceComboA->currentText();
    options.voiceGainA = m_voiceGainA->value();
    options.voiceTrimA = m_voiceTrimBeginA->value();
    
    return options;
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::fileButtonTriggered()
{
    QFileDialog::Options opt = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    
    QString selectedFilter = "";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open database file"),
                                                    "",
                                                    tr("SuperMemo UX database (*.dat);;All Files (*)"),
                                                    &selectedFilter,
                                                    opt);
    if (fileName.isEmpty())
        return;
    m_fileEdit->setText(fileName);
}

/////////////////////////////////////////////////////////////////////////////
void OptionsPage::fileEditChanged(const QString &fileName)
{
    if (!m_superDb.open(fileName))
    {
        QPalette palette = m_fileEdit->palette();
        palette.setColor(QPalette::Text, Qt::red ); // fg
        m_fileEdit->setPalette( palette );
        return;
    }
    
    m_fileEdit->setPalette( m_subnameEdit->palette() ); // fileEdit to default colour
    
    QString oldText = m_courseCombo->currentText();
    
    m_courseCombo->clear();
    
    QStringList courseList;
    m_superDb.getCourses(&courseList);
    
    for (int i = 0;i<courseList.count();++i)
        m_courseCombo->insertItem(0, courseList.at(i));
    
    int pos = m_courseCombo->findText(oldText);
    
    if (pos == -1) // text not found
    {
        m_courseCombo->insertItem(0, QIcon(":/images/warning.png"), oldText);
        m_courseCombo->setCurrentIndex(0);
    }
    else
        m_courseCombo->setCurrentIndex(pos);
    
    return;
}

////////////////////////////////////////////////////////////////////////////
void OptionsPage::testFileRemoveSlot()
{
    m_mediaObject->setCurrentSource(Phonon::MediaSource("qwerty"));
    if (!QFile::remove("test.mp3"))
        trace(QString("cannot remove test.mp3 "), traceError);
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
    
    connect(m_traceLevel2 , SIGNAL(stateChanged (int )) , this , SLOT(traceLevelSlot()));
    connect(m_traceLevel3 , SIGNAL(stateChanged (int )) , this , SLOT(traceLevelSlot()));
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
    n_contentTextEdit = new CodeEditor;
    m_findToolbar = new FindToolbar;
    m_findToolbar->layout()->setMargin(0);
    //contentTextEdit->setAcceptRichText(false);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget( m_findToolbar);
    mainLayout->addWidget(n_contentTextEdit);
    setLayout(mainLayout);
    
    connect (n_contentTextEdit, SIGNAL(textChanged()), this, SLOT(contentChangedSlot()));
    
    connect (m_findToolbar , SIGNAL(findNext(const QString& )) , this, SLOT(findNext(const QString& )));
    connect (m_findToolbar , SIGNAL(findPrev(const QString& )) , this, SLOT(findPrev(const QString& )));
    
}
/////////////////////////////////////////////////////////////////////////////
void ContentPage::setContent (const QStringList & content)
{
    n_contentTextEdit->setPlainText(content.join(QString("\n")));
}


/////////////////////////////////////////////////////////////////////////////
QStringList ContentPage::getContent ()
{
    QString str = n_contentTextEdit->toPlainText();
    return str.split(QString("\n"));
}

/////////////////////////////////////////////////////////////////////////////
void ContentPage::contentChangedSlot()
{
    m_findToolbar->hide();
    emit contentChangedSignal();
}

/////////////////////////////////////////////////////////////////////////////
void ContentPage::keyPressEvent ( QKeyEvent * event )
{
    if ((event->modifiers()&Qt::ControlModifier) &&
            (event->key() == Qt::Key_F))
    {
        m_findToolbar->show();
        m_findToolbar->setFindFocus();
    }
    else
        QWidget::keyPressEvent(event);
}

/////////////////////////////////////////////////////////////////////////////
void ContentPage::findNext(const QString &txt )
{
    n_contentTextEdit->find(txt);
}

/////////////////////////////////////////////////////////////////////////////
void ContentPage::findPrev(const QString &txt )
{
    n_contentTextEdit->find(txt, QTextDocument::FindBackward);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FindToolbar::FindToolbar(QWidget *parent)
    : QWidget(parent)
{
    m_lineEdit = new QLineEdit;
    m_forwardButton = new QPushButton (QIcon(":/images/next.png"), "");
    m_backwardButton = new QPushButton (QIcon(":/images/prev.png"), "");
    
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(new QLabel("Find:"));
    mainLayout->addWidget(m_lineEdit);
    mainLayout->addWidget(m_backwardButton);
    mainLayout->addWidget(m_forwardButton);
    setLayout(mainLayout);
    
    connect (m_lineEdit , SIGNAL(returnPressed()), this , SLOT(nextSlot()));
    connect (m_forwardButton , SIGNAL(clicked()) , this , SLOT(nextSlot()));
    connect (m_backwardButton , SIGNAL(clicked()) , this , SLOT(prevSlot()));
    
}

/////////////////////////////////////////////////////////////////////////////
void FindToolbar::nextSlot()
{
    QString txt = m_lineEdit->text();
    if (txt.isEmpty())
        return;
    
    emit findNext(txt);
}

/////////////////////////////////////////////////////////////////////////////
void FindToolbar::prevSlot()
{
    QString txt = m_lineEdit->text();
    if (txt.isEmpty())
        return;
    
    emit findPrev(txt);
}

/////////////////////////////////////////////////////////////////////////////
void FindToolbar::setFindFocus ()
{
    m_lineEdit->setFocus();
}
