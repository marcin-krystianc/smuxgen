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
#include "coursetemplateoptions.h"
#include "ccodeeditor.h"
#include "csapi.h"

/////////////////////////////////////////////////////////////////////////////
cOptionsPage::cOptionsPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *configGroup = new QGroupBox(tr("Choose options:"));
    oForceCheckBox = new QCheckBox(tr("Regenerate"));
    oDoubleCheckBox = new QCheckBox(tr("Alternative"));
    oImageCheckBox = new QCheckBox(tr("Images"));
    oVoiceCheckBoxQ = new QCheckBox(tr("Lector (Questions)"));
    oVoiceCheckBoxA = new QCheckBox(tr("Lector (Answers)"));

    QGridLayout *checkLayout = new QGridLayout;
    checkLayout->addWidget(oForceCheckBox , 0 , 0);
    checkLayout->addWidget(oDoubleCheckBox , 0 , 1);
    checkLayout->addWidget(oImageCheckBox , 1 , 0);

    QLabel *fileLabel = new QLabel(tr("Database file:"));
    fileEdit = new QLineEdit;
    QPushButton *fileChooseButton = new QPushButton(tr("Choose"));
    fileEdit->setReadOnly(true);

    QGridLayout *fileLayout = new QGridLayout;
    fileLayout->addWidget(fileLabel, 0, 0);
    fileLayout->addWidget(fileChooseButton, 0, 1);
    fileLayout->addWidget(fileEdit, 1, 0, 1, 0);

    QLabel *courseLabel = new QLabel(tr("Course:"));
    courseCombo = new QComboBox;

    QHBoxLayout *courseLayout = new QHBoxLayout;
    courseLayout->addWidget(courseLabel);
    courseLayout->addWidget(courseCombo);

    QLabel *subnameLabel = new QLabel(tr("Lesson"));
    subnameEdit = new QLineEdit;

    QHBoxLayout *subnameLayout = new QHBoxLayout;
    subnameLayout->addWidget(subnameLabel);
    subnameLayout->addWidget(subnameEdit);

    QLabel *instructionLabel = new QLabel(tr("Instruction"));
    instructionEdit = new QLineEdit;

    QHBoxLayout *instructionLayout = new QHBoxLayout;
    instructionLayout->addWidget(instructionLabel);
    instructionLayout->addWidget(instructionEdit);

    voiceComboQ = new QComboBox;
    voiceComboQ->insertItems(0, getVoiceEngines());
    voiceGainQ = new QSpinBox;
    voiceTrimBeginQ = new QDoubleSpinBox;
    voiceTrimBeginQ->setSingleStep(0.1);
    voiceTrimBeginQ->setDecimals(1);

    voiceComboA = new QComboBox;
    voiceComboA->insertItems(0, getVoiceEngines());
    voiceGainA = new QSpinBox;
    voiceTrimBeginA = new QDoubleSpinBox;
    voiceTrimBeginA->setSingleStep(0.1);
    voiceTrimBeginA->setDecimals(1);

    voiceTesttextQ = new QLineEdit;
    voiceTestbuttonQ = new QPushButton(tr("Test this text below"));

    voiceTesttextA = new QLineEdit;
    voiceTestbuttonA = new QPushButton(tr("Test this text below"));

    voiceTesttextQ->setText("Test Testing Testen");
    QLabel *voiceIndexLabelQ = new QLabel(tr("Voice name"));
    QLabel *voiceGainLabelQ = new QLabel(tr("Gain [dB] "));
    QLabel *voiceTrimBeginLabelQ = new QLabel(tr("Trim [s]"));

    voiceTesttextA->setText("Test Testing Testen");
    QLabel *voiceIndexLabelA = new QLabel(tr("Voice name"));
    QLabel *voiceGainLabelA = new QLabel(tr("Gain [dB] "));
    QLabel *voiceTrimBeginLabelA = new QLabel(tr("Trim [s]"));

    QGridLayout *voiceLayoutQ = new QGridLayout;
    /*
 QFrame *frameQ = new QFrame;
 frameQ->setFrameStyle(QFrame::HLine);
 voiceLayoutQ->addWidget(frameQ , 0 , 0 , 1 , 0);
 */
    voiceLayoutQ->addWidget(oVoiceCheckBoxQ , 1 , 0 , 1 , 0);
    voiceLayoutQ->addWidget(voiceIndexLabelQ , 2 , 0);
    voiceLayoutQ->addWidget(voiceComboQ , 2 , 1);
    voiceLayoutQ->addWidget(voiceGainLabelQ , 3 , 0);
    voiceLayoutQ->addWidget(voiceGainQ , 3 , 1);
    voiceLayoutQ->addWidget(voiceTrimBeginLabelQ , 4 , 0);
    voiceLayoutQ->addWidget(voiceTrimBeginQ , 4 , 1);
    voiceLayoutQ->addWidget(voiceTestbuttonQ , 5 , 0 , 1 , 0);
    voiceLayoutQ->addWidget(voiceTesttextQ , 6 , 0 , 1 , 0);

    QGridLayout *voiceLayoutA = new QGridLayout;
    /*
 QFrame *frameA = new QFrame;
 frameA->setFrameStyle(QFrame::HLine);
 voiceLayoutA->addWidget(frameA , 0 , 0 , 1 , 0);
 */
    voiceLayoutA->addWidget(oVoiceCheckBoxA , 1 , 0 , 1 , 0);
    voiceLayoutA->addWidget(voiceIndexLabelA , 2 , 0);
    voiceLayoutA->addWidget(voiceComboA , 2 , 1);
    voiceLayoutA->addWidget(voiceGainLabelA , 3 , 0);
    voiceLayoutA->addWidget(voiceGainA , 3 , 1);
    voiceLayoutA->addWidget(voiceTrimBeginLabelA , 4 , 0);
    voiceLayoutA->addWidget(voiceTrimBeginA , 4 , 1);
    voiceLayoutA->addWidget(voiceTestbuttonA , 5 , 0 , 1 , 0);
    voiceLayoutA->addWidget(voiceTesttextA , 6 , 0 , 1 , 0);

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

    audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject (this);

    connect(fileChooseButton, SIGNAL(clicked()) , this , SLOT(fileButtonTriggered()));
    connect(fileEdit , SIGNAL(textChanged(const QString & )) , this , SLOT(fileEditChanged(const QString &)));
    connect(voiceTestbuttonQ, SIGNAL(clicked()) , this , SLOT(voiceTestButtonTriggered()));
    connect(voiceTestbuttonA, SIGNAL(clicked()) , this , SLOT(voiceTestButtonTriggered()));

    connect(oVoiceCheckBoxQ , SIGNAL(stateChanged (int )) , this , SLOT(voiceCheckBoxChangedQ(int)));
    connect(oVoiceCheckBoxA , SIGNAL(stateChanged (int )) , this , SLOT(voiceCheckBoxChangedA(int)));

    connect(mediaObject , SIGNAL(finished ()) , this , SLOT(testFileRemoveSlot()));

    voiceCheckBoxChangedQ(oVoiceCheckBoxQ->checkState());
    voiceCheckBoxChangedA(oVoiceCheckBoxA->checkState());
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceCheckBoxChangedQ (int state)
{
    switch (state)
    {
    case Qt::Unchecked:
        voiceComboQ-> setEnabled(false);
        voiceGainQ-> setEnabled(false);
        voiceTrimBeginQ-> setEnabled(false);
        voiceTesttextQ-> setEnabled(false);
        voiceTestbuttonQ-> setEnabled(false);
        break;

    case Qt::Checked:
        voiceComboQ-> setEnabled(true);
        voiceGainQ-> setEnabled(true);
        voiceTrimBeginQ-> setEnabled(true);
        voiceTesttextQ-> setEnabled(true);
        voiceTestbuttonQ-> setEnabled(true);
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceCheckBoxChangedA (int state)
{
    switch (state)
    {
    case Qt::Unchecked:
        voiceComboA-> setEnabled(false);
        voiceGainA-> setEnabled(false);
        voiceTrimBeginA-> setEnabled(false);
        voiceTesttextA-> setEnabled(false);
        voiceTestbuttonA-> setEnabled(false);
        break;

    case Qt::Checked:
        voiceComboA-> setEnabled(true);
        voiceGainA-> setEnabled(true);
        voiceTrimBeginA-> setEnabled(true);
        voiceTesttextA-> setEnabled(true);
        voiceTestbuttonA-> setEnabled(true);
        break;
    }
}
/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::setOptions(const CourseTemplateOptions &options)
{
    oForceCheckBox ->setCheckState(options.bit.oForce ? Qt::Checked : Qt::Unchecked);
    oDoubleCheckBox ->setCheckState(options.bit.oDouble ? Qt::Checked : Qt::Unchecked);
    oImageCheckBox ->setCheckState(options.bit.oImage ? Qt::Checked : Qt::Unchecked);
    oVoiceCheckBoxQ ->setCheckState(options.bit.oVoiceQ ? Qt::Checked : Qt::Unchecked);
    oVoiceCheckBoxA ->setCheckState(options.bit.oVoiceA ? Qt::Checked : Qt::Unchecked);

    fileEdit->setText(options.database);
    courseCombo->clear();
    courseCombo->insertItem(0, options.course);
    subnameEdit->setText(options.subname);
    instructionEdit->setText(options.instruction);

    voiceComboQ ->setCurrentIndex(getVoiceEngineIndex(options.voiceNameQ));
    voiceGainQ ->setValue(options.voiceGainQ);
    voiceTrimBeginQ->setValue(options.voiceTrimQ);

    voiceComboA ->setCurrentIndex(getVoiceEngineIndex(options.voiceNameA));
    voiceGainA ->setValue(options.voiceGainA);
    voiceTrimBeginA->setValue(options.voiceTrimA);

    fileEditChanged(fileEdit->text());
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceTestButtonTriggered ()
{
    CourseTemplateOptions options = getOptions();
    QStringList arguments; // filename, text, trim, gain

    if (sender() == voiceTestbuttonQ)
    {
        arguments.append("test");
        arguments.append(voiceTesttextQ->text());
        arguments.append(QString::number(getVoiceEngineIndex(options.voiceNameQ)+1));
        arguments.append(QString::number(options.voiceTrimQ));
        arguments.append(QString::number(options.voiceGainQ));
    }
    else if (sender() == voiceTestbuttonA)
    {
        arguments.append("test");
        arguments.append(voiceTesttextA->text());
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

    Phonon::createPath(mediaObject, audioOutput);
    mediaObject->setCurrentSource(Phonon::MediaSource("test.mp3"));
    mediaObject->play();

}
/////////////////////////////////////////////////////////////////////////////
CourseTemplateOptions cOptionsPage::getOptions()
{
    CourseTemplateOptions options;
    options.bit.oForce = oForceCheckBox->isChecked();
    options.bit.oDouble = oDoubleCheckBox->isChecked();
    options.bit.oImage = oImageCheckBox->isChecked();
    options.bit.oVoiceQ = oVoiceCheckBoxQ->isChecked();
    options.bit.oVoiceA = oVoiceCheckBoxA->isChecked();

    options.database = fileEdit->text();
    options.subname = subnameEdit->text();
    options.instruction = instructionEdit->text();

    options.course = courseCombo->currentText();

    options.voiceNameQ = voiceComboQ->currentText();
    options.voiceGainQ = voiceGainQ->value();
    options.voiceTrimQ = voiceTrimBeginQ->value();

    options.voiceNameA = voiceComboA->currentText();
    options.voiceGainA = voiceGainA->value();
    options.voiceTrimA = voiceTrimBeginA->value();

    return options;
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::fileButtonTriggered()
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
    fileEdit->setText(fileName);
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::fileEditChanged(const QString &fileName)
{
    if (!superMemoSQL.open(fileName))
    {
        QPalette palette = fileEdit->palette();
        palette.setColor(QPalette::Text, Qt::red ); // fg
        fileEdit->setPalette( palette );
        return;
    }

    fileEdit->setPalette( subnameEdit->palette() ); // fileEdit to default colour

    QString oldText = courseCombo->currentText();

    courseCombo->clear();

    QStringList courseList;
    superMemoSQL.getCourses(&courseList);

    for (int i = 0;i<courseList.count();++i)
        courseCombo->insertItem(0, courseList.at(i));

    int pos = courseCombo->findText(oldText);

    if (pos == -1) // text not found
    {
        courseCombo->insertItem(0, QIcon(":/images/warning.png"), oldText);
        courseCombo->setCurrentIndex(0);
    }
    else
        courseCombo->setCurrentIndex(pos);

    return;
}

////////////////////////////////////////////////////////////////////////////
void cOptionsPage::testFileRemoveSlot()
{
    mediaObject->setCurrentSource(Phonon::MediaSource("qwerty"));
    if (!QFile::remove("test.mp3"))
        trace(QString("cannot remove test.mp3 "), traceError);
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::trace (const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cConsolePage::cConsolePage(QWidget *parent)
    : QWidget(parent)
{
    consoleText = new QTextEdit;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout *bLayout = new QHBoxLayout;
    cTracelevel2 = new QCheckBox("Trace 2");
    cTracelevel3 = new QCheckBox("Trace 3");
    bLayout->addWidget(cTracelevel2);
    bLayout->addWidget(cTracelevel3);
    bLayout->addStretch(1);

    mainLayout->addWidget(consoleText);
    mainLayout->addLayout(bLayout);
    setLayout(mainLayout);

    consoleText->setReadOnly(true);

    connect(cTracelevel2 , SIGNAL(stateChanged (int )) , this , SLOT(traceLevelSlot()));
    connect(cTracelevel3 , SIGNAL(stateChanged (int )) , this , SLOT(traceLevelSlot()));
}
/////////////////////////////////////////////////////////////////////////////
void cConsolePage::traceLevelSlot ()
{
    unsigned int flags = traceError|traceWarning|traceLevel1;

    if (cTracelevel2->checkState() == Qt::Checked)
        flags |= traceLevel2;

    if (cTracelevel3->checkState() == Qt::Checked)
        flags |= traceLevel3;

    globalTracer.setTraceFlags(flags);

}
/////////////////////////////////////////////////////////////////////////////
void cConsolePage::traceSlot(const QString &txt, const int & flags)
{
    traceMutex.lock();

    QDateTime dateTime = QDateTime::currentDateTime ();

    consoleText->setTextColor(Qt::black);
    if (flags & traceWarning)
        consoleText->setTextColor(Qt::darkMagenta);
    if (flags & traceError)
        consoleText->setTextColor(Qt::red);

    consoleText->append(dateTime.toString("hh:mm:ss: ")+txt);

    traceMutex.unlock();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cContentPage::cContentPage(QWidget *parent)
    : QWidget(parent)
{
    contentTextEdit = new CodeEditor;
    findToolbar = new cFindToolbar;
    findToolbar->layout()->setMargin(0);
    //contentTextEdit->setAcceptRichText(false);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget( findToolbar);
    mainLayout->addWidget(contentTextEdit);
    setLayout(mainLayout);

    connect (contentTextEdit, SIGNAL(textChanged()), this, SLOT(contentChangedSlot()));

    connect (findToolbar , SIGNAL(findNext(const QString& )) , this, SLOT(findNext(const QString& )));
    connect (findToolbar , SIGNAL(findPrev(const QString& )) , this, SLOT(findPrev(const QString& )));

}
/////////////////////////////////////////////////////////////////////////////
void cContentPage::setContent (const QStringList & content)
{
    contentTextEdit->setPlainText(content.join(QString("\n")));
}


/////////////////////////////////////////////////////////////////////////////
QStringList cContentPage::getContent ()
{
    QString str = contentTextEdit->toPlainText();
    return str.split(QString("\n"));
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::contentChangedSlot()
{
    findToolbar->hide();
    emit contentChangedSignal();
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::keyPressEvent ( QKeyEvent * event )
{
    if ((event->modifiers()&Qt::ControlModifier) &&
            (event->key() == Qt::Key_F))
    {
        findToolbar->show();
        findToolbar->setFindFocus();
    }
    else
        QWidget::keyPressEvent(event);
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::findNext(const QString &txt )
{
    contentTextEdit->find(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::findPrev(const QString &txt )
{
    contentTextEdit->find(txt, QTextDocument::FindBackward);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cFindToolbar::cFindToolbar(QWidget *parent)
    : QWidget(parent)
{
    lineEdit = new QLineEdit;
    forwardButton = new QPushButton (QIcon(":/images/next.png"), "");
    backwardButton = new QPushButton (QIcon(":/images/prev.png"), "");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(new QLabel("Find:"));
    mainLayout->addWidget(lineEdit);
    mainLayout->addWidget(backwardButton);
    mainLayout->addWidget(forwardButton);
    setLayout(mainLayout);

    connect (lineEdit , SIGNAL(returnPressed()), this , SLOT(nextSlot()));
    connect (forwardButton , SIGNAL(clicked()) , this , SLOT(nextSlot()));
    connect (backwardButton , SIGNAL(clicked()) , this , SLOT(prevSlot()));

}

/////////////////////////////////////////////////////////////////////////////
void cFindToolbar::nextSlot()
{
    QString txt = lineEdit->text();
    if (txt.isEmpty())
        return;

    emit findNext(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cFindToolbar::prevSlot()
{
    QString txt = lineEdit->text();
    if (txt.isEmpty())
        return;

    emit findPrev(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cFindToolbar::setFindFocus ()
{
    lineEdit->setFocus();
}
