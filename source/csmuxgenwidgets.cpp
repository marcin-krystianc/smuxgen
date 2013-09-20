//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
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
    QGroupBox *configGroup      = new QGroupBox(tr("Choose options:"));
    oForceCheckBox   = new QCheckBox(tr("Regenerate"));
    oDoubleCheckBox  = new QCheckBox(tr("Alternative"));
    oImageCheckBox   = new QCheckBox(tr("Images"));
    oVoiceCheckBoxQ  = new QCheckBox(tr("Lector (Questions)"));
    oVoiceCheckBoxA  = new QCheckBox(tr("Lector (Answers)"));

    QGridLayout *checkLayout = new QGridLayout;
    checkLayout->addWidget(oForceCheckBox   ,0  ,0);
    checkLayout->addWidget(oDoubleCheckBox  ,0  ,1);
    checkLayout->addWidget(oImageCheckBox   ,1  ,0);

    QLabel *fileLabel   = new QLabel(tr("Database file:"));
    fileEdit            = new QLineEdit;
    QPushButton *fileChooseButton   = new QPushButton(tr("Choose"));
    fileEdit->setReadOnly(true);

    QGridLayout *fileLayout = new QGridLayout;
    fileLayout->addWidget(fileLabel, 0, 0);
    fileLayout->addWidget(fileChooseButton, 0, 1);
    fileLayout->addWidget(fileEdit, 1, 0,1,0);

    QLabel *courseLabel     = new QLabel(tr("Course:"));
    courseCombo             = new QComboBox;

    QHBoxLayout *courseLayout = new QHBoxLayout;
    courseLayout->addWidget(courseLabel);
    courseLayout->addWidget(courseCombo);

    QLabel *subnameLabel    = new QLabel(tr("Lesson"));
    subnameEdit             = new QLineEdit;

    QHBoxLayout *subnameLayout = new QHBoxLayout;
    subnameLayout->addWidget(subnameLabel);
    subnameLayout->addWidget(subnameEdit);

    QLabel *instructionLabel    = new QLabel(tr("Instruction"));
    instructionEdit             = new QLineEdit;

    QHBoxLayout *instructionLayout = new QHBoxLayout;
    instructionLayout->addWidget(instructionLabel);
    instructionLayout->addWidget(instructionEdit);

    voiceComboQ     = new QComboBox;
    voiceComboQ->insertItems(0,getVoices());
    voiceGainQ      = new QSpinBox;
    voiceTrimBeginQ = new QDoubleSpinBox;
    voiceTrimBeginQ->setSingleStep(0.1);
    voiceTrimBeginQ->setDecimals(1);

    voiceComboA     = new QComboBox;
    voiceComboA->insertItems(0,getVoices());
    voiceGainA      = new QSpinBox;
    voiceTrimBeginA = new QDoubleSpinBox;
    voiceTrimBeginA->setSingleStep(0.1);
    voiceTrimBeginA->setDecimals(1);

    voiceTesttextQ      = new QLineEdit;
    voiceTestbuttonQ    = new QPushButton(tr("Test this text below"));

    voiceTesttextA      = new QLineEdit;
    voiceTestbuttonA    = new QPushButton(tr("Test this text below"));

    voiceTesttextQ->setText("Test Testing Testen");
    QLabel *voiceIndexLabelQ     = new QLabel(tr("Voice name"));
    QLabel *voiceGainLabelQ      = new QLabel(tr("Gain [dB] "));
    QLabel *voiceTrimBeginLabelQ = new QLabel(tr("Trim [s]"));

    voiceTesttextA->setText("Test Testing Testen");
    QLabel *voiceIndexLabelA     = new QLabel(tr("Voice name"));
    QLabel *voiceGainLabelA      = new QLabel(tr("Gain [dB] "));
    QLabel *voiceTrimBeginLabelA = new QLabel(tr("Trim [s]"));

    QGridLayout *voiceLayoutQ = new QGridLayout;
    /*
    QFrame *frameQ = new QFrame;
    frameQ->setFrameStyle(QFrame::HLine);
    voiceLayoutQ->addWidget(frameQ                  ,0  ,0  ,1  ,0);
    */
    voiceLayoutQ->addWidget(oVoiceCheckBoxQ         ,1  ,0  ,1  ,0);
    voiceLayoutQ->addWidget(voiceIndexLabelQ        ,2  ,0);
    voiceLayoutQ->addWidget(voiceComboQ             ,2  ,1);
    voiceLayoutQ->addWidget(voiceGainLabelQ         ,3  ,0);
    voiceLayoutQ->addWidget(voiceGainQ              ,3  ,1);
    voiceLayoutQ->addWidget(voiceTrimBeginLabelQ    ,4  ,0);
    voiceLayoutQ->addWidget(voiceTrimBeginQ         ,4  ,1);
    voiceLayoutQ->addWidget(voiceTestbuttonQ        ,5  ,0  ,1  ,0);
    voiceLayoutQ->addWidget(voiceTesttextQ          ,6  ,0  ,1  ,0);

    QGridLayout *voiceLayoutA = new QGridLayout;
    /*
    QFrame *frameA = new QFrame;
    frameA->setFrameStyle(QFrame::HLine);
    voiceLayoutA->addWidget(frameA                  ,0  ,0  ,1  ,0);
    */
    voiceLayoutA->addWidget(oVoiceCheckBoxA         ,1  ,0  ,1  ,0);
    voiceLayoutA->addWidget(voiceIndexLabelA        ,2  ,0);
    voiceLayoutA->addWidget(voiceComboA             ,2  ,1);
    voiceLayoutA->addWidget(voiceGainLabelA         ,3  ,0);
    voiceLayoutA->addWidget(voiceGainA              ,3  ,1);
    voiceLayoutA->addWidget(voiceTrimBeginLabelA    ,4  ,0);
    voiceLayoutA->addWidget(voiceTrimBeginA         ,4  ,1);
    voiceLayoutA->addWidget(voiceTestbuttonA        ,5  ,0  ,1  ,0);
    voiceLayoutA->addWidget(voiceTesttextA          ,6  ,0  ,1  ,0);

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

    this->audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
    this->mediaObject = new Phonon::MediaObject (this);

    connect(fileChooseButton, SIGNAL(clicked())                     ,this ,SLOT(fileButtonTriggered()));
    connect(this->fileEdit  , SIGNAL(textChanged(const QString & )) ,this ,SLOT(fileEditChanged(const QString &)));
    connect(voiceTestbuttonQ, SIGNAL(clicked())                     ,this ,SLOT(voiceTestButtonTriggered()));
    connect(voiceTestbuttonA, SIGNAL(clicked())                     ,this ,SLOT(voiceTestButtonTriggered()));

    connect(this->oVoiceCheckBoxQ , SIGNAL(stateChanged (int )) ,this ,SLOT(voiceCheckBoxChangedQ(int)));
    connect(this->oVoiceCheckBoxA , SIGNAL(stateChanged (int )) ,this ,SLOT(voiceCheckBoxChangedA(int)));

    connect(this->mediaObject     , SIGNAL(finished ()) ,this ,SLOT(testFileRemoveSlot()));

    this->voiceCheckBoxChangedQ(this->oVoiceCheckBoxQ->checkState());
    this->voiceCheckBoxChangedA(this->oVoiceCheckBoxA->checkState());
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceCheckBoxChangedQ (int state)
{
    switch (state)
    {
        case Qt::Unchecked:
            voiceComboQ->       setEnabled(false);
            voiceGainQ->        setEnabled(false);
            voiceTrimBeginQ->   setEnabled(false);
            voiceTesttextQ->    setEnabled(false);
            voiceTestbuttonQ->  setEnabled(false);
            break;

        case Qt::Checked:
            voiceComboQ->       setEnabled(true);
            voiceGainQ->        setEnabled(true);
            voiceTrimBeginQ->   setEnabled(true);
            voiceTesttextQ->    setEnabled(true);
            voiceTestbuttonQ->  setEnabled(true);
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceCheckBoxChangedA (int state)
{
    switch (state)
    {
        case Qt::Unchecked:
            voiceComboA->       setEnabled(false);
            voiceGainA->        setEnabled(false);
            voiceTrimBeginA->   setEnabled(false);
            voiceTesttextA->    setEnabled(false);
            voiceTestbuttonA->  setEnabled(false);
            break;

        case Qt::Checked:
            voiceComboA->       setEnabled(true);
            voiceGainA->        setEnabled(true);
            voiceTrimBeginA->   setEnabled(true);
            voiceTesttextA->    setEnabled(true);
            voiceTestbuttonA->  setEnabled(true);
            break;
    }
}
/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::setOptions(const cCourseTemplateOptions &options)
{
    this->oForceCheckBox    ->setCheckState(options.bit.oForce ? Qt::Checked : Qt::Unchecked);
    this->oDoubleCheckBox   ->setCheckState(options.bit.oDouble ? Qt::Checked : Qt::Unchecked);
    this->oImageCheckBox    ->setCheckState(options.bit.oImage ? Qt::Checked : Qt::Unchecked);
    this->oVoiceCheckBoxQ   ->setCheckState(options.bit.oVoiceQ ? Qt::Checked : Qt::Unchecked);
    this->oVoiceCheckBoxA   ->setCheckState(options.bit.oVoiceA ? Qt::Checked : Qt::Unchecked);

    this->fileEdit->setText(options.database);
    this->courseCombo->clear();
    this->courseCombo->insertItem(0,options.course);
    this->subnameEdit->setText(options.subname);
    this->instructionEdit->setText(options.instruction);

    this->voiceComboQ    ->setCurrentIndex(getVoiceIndex(options.voiceNameQ));
    this->voiceGainQ     ->setValue(options.voiceGainQ);
    this->voiceTrimBeginQ->setValue(options.voiceTrimQ);

    this->voiceComboA    ->setCurrentIndex(getVoiceIndex(options.voiceNameA));
    this->voiceGainA     ->setValue(options.voiceGainA);
    this->voiceTrimBeginA->setValue(options.voiceTrimA);

    this->fileEditChanged(this->fileEdit->text());
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceTestButtonTriggered ()
{
    cCourseTemplateOptions options=this->getOptions();
    QStringList  arguments; // filename, text,trim,gain

    if (sender() ==voiceTestbuttonQ)
    {
        arguments.append("test");
        arguments.append(this->voiceTesttextQ->text());
        arguments.append(QString::number(getVoiceIndex(options.voiceNameQ)+1));
        arguments.append(QString::number(options.voiceTrimQ));
        arguments.append(QString::number(options.voiceGainQ));
    }
    else if (sender() ==voiceTestbuttonA)
    {
        arguments.append("test");
        arguments.append(this->voiceTesttextA->text());
        arguments.append(QString::number(getVoiceIndex(options.voiceNameA)+1));
        arguments.append(QString::number(options.voiceTrimA));
        arguments.append(QString::number(options.voiceGainA));
    }
    else
        return;

    trace(QString("createMp3.bat ")+arguments.join(" "),traceLevel1);
    QProcess myProcess;

    myProcess.start("createMp3.bat", arguments );
    if (!myProcess.waitForStarted())
    {
         trace(QString("Error:createMp3.bat ")+arguments.join(" "),traceError);
         return;
    }
    myProcess.waitForFinished();
    if (myProcess.exitCode())
    {
        trace(QString("Error:createMp3.bat ")+arguments.join(" "),traceError);
        return;
    }

    Phonon::createPath(mediaObject, audioOutput);
    mediaObject->setCurrentSource(Phonon::MediaSource("test.mp3"));
    mediaObject->play();

}
/////////////////////////////////////////////////////////////////////////////
cCourseTemplateOptions cOptionsPage::getOptions()
{
    cCourseTemplateOptions options;
    options.bit.oForce  = this->oForceCheckBox->isChecked();
    options.bit.oDouble = this->oDoubleCheckBox->isChecked();
    options.bit.oImage  = this->oImageCheckBox->isChecked();
    options.bit.oVoiceQ = this->oVoiceCheckBoxQ->isChecked();
    options.bit.oVoiceA = this->oVoiceCheckBoxA->isChecked();

    options.database    = this->fileEdit->text();
    options.subname     = this->subnameEdit->text();
    options.instruction = this->instructionEdit->text();

    options.course      = this->courseCombo->currentText();

    options.voiceNameQ  = this->voiceComboQ->currentText();
    options.voiceGainQ  = this->voiceGainQ->value();
    options.voiceTrimQ  = this->voiceTrimBeginQ->value();

    options.voiceNameA  = this->voiceComboA->currentText();
    options.voiceGainA  = this->voiceGainA->value();
    options.voiceTrimA  = this->voiceTrimBeginA->value();

    return options;
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::fileButtonTriggered()
{
    QFileDialog::Options opt = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;

    QString selectedFilter ="";
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Open database file"),
                                "",
                                tr("SuperMemo UX database (*.dat);;All Files (*)"),
                                &selectedFilter,
                                opt);
    if (fileName.isEmpty())
        return;
    this->fileEdit->setText(fileName);
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::fileEditChanged(const QString &fileName)
{
    if (!this->superMemoSQL.open(fileName))
    {
        QPalette palette = this->fileEdit->palette();
        palette.setColor(QPalette::Text, Qt::red ); // fg
        this->fileEdit->setPalette( palette );
        return;
    }

    this->fileEdit->setPalette( this->subnameEdit->palette() ); // fileEdit to default colour

    QString oldText=this->courseCombo->currentText();

    this->courseCombo->clear();
    QStringList courseList;
    this->superMemoSQL.getCourses(courseList);

    for (int i=0;i<courseList.count();++i)
        this->courseCombo->insertItem(0,courseList.at(i));

    int pos =this->courseCombo->findText(oldText);

    if (pos == -1)  // text not found
    {
        this->courseCombo->insertItem(0,QIcon(":/images/warning.png"),oldText);
        this->courseCombo->setCurrentIndex(0);
    }
    else
        this->courseCombo->setCurrentIndex(pos);

    return;
}

////////////////////////////////////////////////////////////////////////////
void cOptionsPage::testFileRemoveSlot()
{
    this->mediaObject->setCurrentSource(Phonon::MediaSource("qwerty"));
    if (!QFile::remove("test.mp3"))
      trace(QString("cannot remove test.mp3 "),traceError);
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::trace (const QString &text,const int & flags)
{
      globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cConsolePage::cConsolePage(QWidget *parent)
    : QWidget(parent)
{
    consoleText             = new QTextEdit;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout *bLayout    = new QHBoxLayout;
    cTracelevel2 = new QCheckBox("Trace 2");
    cTracelevel3 = new QCheckBox("Trace 3");
    bLayout->addWidget(cTracelevel2);
    bLayout->addWidget(cTracelevel3);
    bLayout->addStretch(1);

    mainLayout->addWidget(consoleText);
    mainLayout->addLayout(bLayout);
    setLayout(mainLayout);

    consoleText->setReadOnly(true);

    connect(cTracelevel2 , SIGNAL(stateChanged (int )) ,this ,SLOT(traceLevelSlot()));
    connect(cTracelevel3 , SIGNAL(stateChanged (int )) ,this ,SLOT(traceLevelSlot()));
}
/////////////////////////////////////////////////////////////////////////////
void cConsolePage::traceLevelSlot ()
{
    unsigned int flags= traceError|traceWarning|traceLevel1;

    if (this->cTracelevel2->checkState()==Qt::Checked)
        flags |= traceLevel2;

    if (this->cTracelevel3->checkState()==Qt::Checked)
        flags |= traceLevel3;

    globalTracer.setTraceFlags(flags);

}
/////////////////////////////////////////////////////////////////////////////
void cConsolePage::traceSlot(const QString &txt,const int & flags)
{
    this->traceMutex.lock();

    QDateTime dateTime=QDateTime::currentDateTime ();

    this->consoleText->setTextColor(Qt::black);
    if (flags & traceWarning)
        this->consoleText->setTextColor(Qt::darkMagenta);
    if (flags & traceError)
            this->consoleText->setTextColor(Qt::red);

    this->consoleText->append(dateTime.toString("hh:mm:ss: ")+txt);

    this->traceMutex.unlock();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cContentPage::cContentPage(QWidget *parent)
    : QWidget(parent)
{
    this->contentTextEdit   = new CodeEditor;
    this->findToolbar       = new cFindToolbar;
    this->findToolbar->layout()->setMargin(0);
    //this->contentTextEdit->setAcceptRichText(false);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget( this->findToolbar);
    mainLayout->addWidget(this->contentTextEdit);
    setLayout(mainLayout);

    connect (this->contentTextEdit,SIGNAL(textChanged()),this,SLOT(contentChangedSlot()));

    connect (this->findToolbar  ,SIGNAL(findNext(const QString& ))  ,this,SLOT(findNext(const QString& )));
    connect (this->findToolbar  ,SIGNAL(findPrev(const QString& ))  ,this,SLOT(findPrev(const QString& )));

}
/////////////////////////////////////////////////////////////////////////////
void cContentPage::setContent (const QStringList & content)
{
    this->contentTextEdit->setPlainText(content.join(QString("\n")));
}


/////////////////////////////////////////////////////////////////////////////
QStringList cContentPage::getContent ()
{
    QString str=this->contentTextEdit->toPlainText();
    return str.split(QString("\n"));
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::contentChangedSlot()
{
    this->findToolbar->hide();
    emit this->contentChangedSignal();
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::keyPressEvent ( QKeyEvent * event )
{
    if ((event->modifiers()&Qt::ControlModifier) &&
        (event->key()==Qt::Key_F))
    {
        this->findToolbar->show();
        this->findToolbar->setFindFocus();
    }
    else
        QWidget::keyPressEvent(event);
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::findNext(const QString &txt )
{
    this->contentTextEdit->find(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cContentPage::findPrev(const QString &txt )
{
    this->contentTextEdit->find(txt,QTextDocument::FindBackward);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cFindToolbar::cFindToolbar(QWidget *parent)
    : QWidget(parent)
{
    this->lineEdit      = new QLineEdit;
    this->forwardButton = new QPushButton (QIcon(":/images/next.png"),"");
    this->backwardButton= new QPushButton (QIcon(":/images/prev.png"),"");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(new QLabel("Find:"));
    mainLayout->addWidget(this->lineEdit);
    mainLayout->addWidget(this->backwardButton);
    mainLayout->addWidget(this->forwardButton);
    setLayout(mainLayout);

    connect (this->lineEdit         ,SIGNAL(returnPressed()),this   ,SLOT(nextSlot()));
    connect (this->forwardButton    ,SIGNAL(clicked())      ,this   ,SLOT(nextSlot()));
    connect (this->backwardButton   ,SIGNAL(clicked())      ,this   ,SLOT(prevSlot()));

}

/////////////////////////////////////////////////////////////////////////////
void cFindToolbar::nextSlot()
{
    QString txt = this->lineEdit->text();
    if (txt.isEmpty())
        return;

    emit findNext(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cFindToolbar::prevSlot()
{
    QString txt = this->lineEdit->text();
    if (txt.isEmpty())
        return;

    emit findPrev(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cFindToolbar::setFindFocus ()
{
    this->lineEdit->setFocus();
}
