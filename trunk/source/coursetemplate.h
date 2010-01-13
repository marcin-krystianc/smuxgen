//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================

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
    void clear();

};

#endif // COURSETEMPLATE_H
