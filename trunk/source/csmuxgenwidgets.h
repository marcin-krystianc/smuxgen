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

#include "coursetemplateoptions.h"
#include "csupermemosql.h"
#include "cglobaltracer.h"

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

    private slots:
        void fileButtonTriggered ();
        void fileEditChanged (const QString &fileName);
        void voiceCheckBoxChanged (int);
        void voiceTestButtonTriggered ();
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
#endif // CSMUXGENWIDGETS_H
