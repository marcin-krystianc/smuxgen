//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include "coursetemplate.h"
#include "coursetemplateoptions.h"
#include "globalsmuxgentools.h"
#include <QFile>
#include <QString>
#include <QTextStream>

/////////////////////////////////////////////////////////////////////////////
cCourseTemplate::cCourseTemplate()
{
    this->clear();
}

/////////////////////////////////////////////////////////////////////////////
bool cCourseTemplate::open(const QString &fileName)
{
    this->clear();

    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        this->trace("cCourseTemplate::open - Cannot open file: "+fileName, traceError);
        return false;
    }

    QTextStream inputFileStream;
    inputFileStream.setDevice( &file );
    inputFileStream.setCodec("UTF-8");


    if (!this->options.fromString (inputFileStream.readLine()))
    {
        this->trace("cCourseTemplate::open - Error in options: "+fileName, traceError);
        return false;
    }

    while (1)
    {
        QString line=(inputFileStream.readLine()).trimmed();
        if (line.isNull()) break; //end of file
        if (line.length()==0) continue;
        this->content.push_back(line);
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cCourseTemplate::save(const QString &fileName)
{
    QFile file (fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        this->trace("cCourseTemplate::save - Cannot open file: "+fileName, traceError);
        return false;
    }

    QTextStream outputFileStream;
    outputFileStream.setDevice( &file );
    outputFileStream.setCodec("UTF-8");

    outputFileStream<<this->options.toString()<<endl;

    for (int i=0;i<this->content.count();++i)
        outputFileStream<<this->content.at(i)<<endl;

    return true;
}
/////////////////////////////////////////////////////////////////////////////
bool cCourseTemplate::importQA (const QString &fileName)
{
    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        this->trace("cCourseTemplate::importQA - Cannot open file: "+fileName, traceError);
        return false;
    }
    QTextStream inputFileStream;
    inputFileStream.setDevice( &file );
    inputFileStream.setCodec("UTF-8");

    bool q(false);
    bool a(false);
    QString entry;
    while (1)
    {
        QString line=(inputFileStream.readLine()).trimmed();
        if (line.isNull()) break; //end of file
        if (line.length()==0) continue;

        if (line.startsWith(QString::fromUtf8("Q:"), Qt::CaseInsensitive))
        {
            if (q&&a)
                this->content.push_back(entry);

            a=false;
            entry=(line.remove(0, 2)).trimmed()+QString::fromUtf8(":");
            q=true;
            continue;
        }

        if (line.startsWith(QString::fromUtf8("A:"), Qt::CaseInsensitive)&&q)
        {
            if (a)
                entry+=QString::fromUtf8("|");

            entry +=(line.remove(0, 2)).trimmed();
            a=true;
            continue;
        }

        trace(QString::fromUtf8("Ignore line:")+line, traceWarning);

        continue;
    }
    if (q&&a)
        this->content.push_back(entry);

    return true;
}
/////////////////////////////////////////////////////////////////////////////
bool cCourseTemplate::exportQA (const QString &fileName)
{
    QFile file (fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        this->trace("cCourseTemplate::exportQA - Cannot open file: "+fileName, traceError);
        return false;
    }

    QTextStream outputFileStream;
    outputFileStream.setDevice( &file );
    outputFileStream.setCodec("UTF-8");

    for (int i=0;i<this->content.count();++i)
    {
        QString s=this->content.at(i);
        QStringList l1=s.split(":");
        if (l1.count()!=2)
            continue;

        QStringList l2=(l1.at(1)).split("|");
        if (l2.count()<1)
            continue;

        outputFileStream<<QString::fromUtf8("Q: ")<<getTextToPrint(l1.at(0))<<endl;
        for (int j=0;j<l2.count();++j)
            outputFileStream<<QString("A: ")<<getTextToPrint(l2.at(j))<<endl;

        outputFileStream<<endl;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
void cCourseTemplate::trace (const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void cCourseTemplate::clear()
{
    this->options.clear();
    this->content.clear();
}
