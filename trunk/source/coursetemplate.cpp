#include "coursetemplate.h"
#include "coursetemplateoptions.h"
#include <QFile>
#include <QString>
#include <QTextStream>

/////////////////////////////////////////////////////////////////////////////
cCourseTemplate::cCourseTemplate()
{
}

/////////////////////////////////////////////////////////////////////////////
bool cCourseTemplate::open(const QString &fileName)
{
    this->content.clear();

    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        this->trace("cCourseTemplate::open - Cannot open file: "+fileName,traceError);
        return false;
    }

    QTextStream inputFileStream;
    inputFileStream.setDevice( &file );
    inputFileStream.setCodec("UTF-8");


    if (!this->options.fromString (inputFileStream.readLine()))
    {
        this->trace("cCourseTemplate::open - Error in options: "+fileName,traceError);
        return false;
    }

    while (1)
    {
        QString line=(inputFileStream.readLine()).trimmed();
        if (line.isNull()) break;  //end of file
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
        this->trace("cCourseTemplate::save - Cannot open file: "+fileName,traceError);
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
void cCourseTemplate::trace (const QString &text,const int & flags)
{
      globalTracer.trace(text,flags);
}
