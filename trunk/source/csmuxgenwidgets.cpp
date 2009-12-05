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


cOptionsPage::cOptionsPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *configGroup      = new QGroupBox(tr("Choose options:"));
    oForceCheckBox   = new QCheckBox(tr("Regenerate"));
    oDoubleCheckBox  = new QCheckBox(tr("Alternative"));
    oImageCheckBox   = new QCheckBox(tr("Images"));
    oVoiceCheckBox   = new QCheckBox(tr("Lector"));

    QGridLayout *checkLayout = new QGridLayout;
    checkLayout->addWidget(oForceCheckBox   ,0  ,0);
    checkLayout->addWidget(oDoubleCheckBox  ,0  ,1);
    checkLayout->addWidget(oImageCheckBox   ,1  ,0);
    checkLayout->addWidget(oVoiceCheckBox   ,1  ,1);

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


    voiceIndex     = new QSpinBox;
    voiceGain      = new QSpinBox;
    voiceTrimBegin = new QDoubleSpinBox;
    voiceTrimBegin->setSingleStep(0.1);
    voiceTrimBegin->setDecimals(1);

    voiceTesttext  = new QLineEdit;
    voiceTestbutton= new QPushButton(tr("Test this text below"));
    voiceTesttext->setText("Test Testing Testen");
    QLabel *voiceIndexLabel     = new QLabel(tr("Voice index"));
    QLabel *voiceGainLabel      = new QLabel(tr("Gain [dB] "));
    QLabel *voiceTrimBeginLabel = new QLabel(tr("Trim [s]"));

    QGridLayout *voiceLayout = new QGridLayout;

    voiceLayout->addWidget(voiceIndexLabel      ,0  ,0);
    voiceLayout->addWidget(voiceIndex           ,0  ,1);
    voiceLayout->addWidget(voiceGainLabel       ,1  ,0);
    voiceLayout->addWidget(voiceGain            ,1  ,1);
    voiceLayout->addWidget(voiceTrimBeginLabel  ,2  ,0);
    voiceLayout->addWidget(voiceTrimBegin       ,2  ,1);
    voiceLayout->addWidget(voiceTestbutton      ,3  ,0  ,1  ,0);
    voiceLayout->addWidget(voiceTesttext        ,4  ,0  ,1  ,0);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(fileLayout);
    configLayout->addSpacing(10);
    configLayout->addLayout(courseLayout);
    configLayout->addLayout(subnameLayout);
    configLayout->addSpacing(10);
    configLayout->addLayout(checkLayout);
    configLayout->addSpacing(20);
    configLayout->addLayout(voiceLayout);
    configLayout->addStretch(1);
    configGroup->setLayout(configLayout);

    setLayout(configLayout);

    this->audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
    this->mediaObject = new Phonon::MediaObject (this);

    connect(fileChooseButton, SIGNAL(clicked())                     ,this ,SLOT(fileButtonTriggered()));
    connect(this->fileEdit  , SIGNAL(textChanged(const QString & )) ,this ,SLOT(fileEditChanged(const QString &)));
    connect(voiceTestbutton , SIGNAL(clicked())                     ,this ,SLOT(voiceTestButtonTriggered()));

    connect(this->oVoiceCheckBox  , SIGNAL(stateChanged (int )) ,this ,SLOT(voiceCheckBoxChanged(int)));
    connect(this->mediaObject     , SIGNAL(finished ()) ,this ,SLOT(testFileRemoveSlot()));

    this->voiceCheckBoxChanged(this->oVoiceCheckBox->checkState());
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceCheckBoxChanged (int state)
{
    switch (state)
    {
        case Qt::Unchecked:
            voiceIndex->    setEnabled(false);
            voiceGain->     setEnabled(false);
            voiceTrimBegin->setEnabled(false);
            voiceTesttext-> setEnabled(false);
            voiceTestbutton->setEnabled(false);
            break;

        case Qt::Checked:
            voiceIndex->    setEnabled(true);
            voiceGain->     setEnabled(true);
            voiceTrimBegin->setEnabled(true);
            voiceTesttext-> setEnabled(true);
            voiceTestbutton->setEnabled(true);
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::setOptions(const cCourseTemplateOptions &options)
{
    this->oForceCheckBox    ->setCheckState(options.bit.oForce ? Qt::Checked : Qt::Unchecked);
    this->oDoubleCheckBox   ->setCheckState(options.bit.oDouble ? Qt::Checked : Qt::Unchecked);
    this->oImageCheckBox    ->setCheckState(options.bit.oImage ? Qt::Checked : Qt::Unchecked);
    this->oVoiceCheckBox    ->setCheckState(options.bit.oVoice ? Qt::Checked : Qt::Unchecked);

    this->fileEdit->setText(options.database);
    this->courseCombo->clear();
    this->courseCombo->insertItem(0,options.course);
    this->subnameEdit->setText(options.subname);

    this->voiceIndex    ->setValue(options.voiceIndex);
    this->voiceGain     ->setValue(options.voiceGain);
    this->voiceTrimBegin->setValue(options.voiceTrim);

    this->fileEditChanged(this->fileEdit->text());
}

/////////////////////////////////////////////////////////////////////////////
void cOptionsPage::voiceTestButtonTriggered ()
{
    cCourseTemplateOptions options=this->getOptions();

    QStringList  arguments; // filename, text,trim,gain
    arguments.append("test");
    arguments.append(this->voiceTesttext->text());
    arguments.append(QString::number(options.voiceIndex));
    arguments.append(QString::number(options.voiceTrim));
    arguments.append(QString::number(options.voiceGain));
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
    options.bit.oVoice  = this->oVoiceCheckBox->isChecked();

    options.database    = this->fileEdit->text();
    options.subname     = this->subnameEdit->text();
    options.course      = this->courseCombo->currentText();

    options.voiceIndex  = this->voiceIndex->value();
    options.voiceGain   = this->voiceGain->value();
    options.voiceTrim   = this->voiceTrimBegin->value();

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
    mainLayout->addWidget(consoleText);
    setLayout(mainLayout);

    consoleText->setReadOnly(true);
}

/////////////////////////////////////////////////////////////////////////////
void cConsolePage::traceSlot(const QString &txt,const int & flags)
{
    QDateTime dateTime=QDateTime::currentDateTime ();

    this->consoleText->setTextColor(Qt::black);
    if (flags & traceWarning)
        this->consoleText->setTextColor(Qt::darkMagenta);
    if (flags & traceError)
            this->consoleText->setTextColor(Qt::red);

    this->consoleText->append(dateTime.toString("hh:mm:ss: ")+txt);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cContentPage::cContentPage(QWidget *parent)
    : QWidget(parent)
{
    this->contentTextEdit   = new QTextEdit;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(this->contentTextEdit);
    setLayout(mainLayout);

    connect (this->contentTextEdit,SIGNAL(textChanged()),this,SLOT(contentChangedSlot()));
}
/////////////////////////////////////////////////////////////////////////////
void cContentPage::setContent (const QStringList & content)
{
    /*
    this->contentTextEdit->clear();
    for (int i=0;i<content.count();++i)
        this->contentTextEdit->append(content.at(i));
        */
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
    emit this->contentChangedSignal();
}
