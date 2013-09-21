//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#ifndef COURSETEMPLATE_H
#define COURSETEMPLATE_H

#include "courseoptions.h"

#include <QObject>
#include <QString>
#include <QStringList>

class CourseTemplate
{

public:
   static CourseTemplate fromFile(const QString &fileName);
   static bool toFile (const QString &fileName, const CourseTemplate &courseTemplate);
   bool importQA (const QString &fileName); // import from Q&A file - false if not succeeded
   bool exportQA (const QString &fileName); // export to Q&A file - false if not succeeded

   CourseOptions m_options;
   QStringList m_content;
private:

   static void trace (const QString &text, const int flags = traceLevel1|0);
};

#endif // COURSETEMPLATE_H
