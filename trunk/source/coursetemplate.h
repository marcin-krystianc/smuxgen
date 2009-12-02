#ifndef COURSETEMPLATE_H
#define COURSETEMPLATE_H

#include "coursetemplateoptions.h"

#include <QObject>
#include <QString>
#include <QStringList>

class cCourseTemplate
{

public:
    cCourseTemplate();

    bool open (const QString &fileName);    // open from file - false if not succeeded
    bool save (const QString &fileName);    // save to file - false if not succeeded

    cCourseTemplateOptions options;
    QStringList content;
private:

    void trace (const QString &text,const int & flags = traceLevel1|0);

};

#endif // COURSETEMPLATE_H
