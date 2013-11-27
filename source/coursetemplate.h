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

#include <QString>
#include <QImage>


struct ContentItem
{
   QString question;
   QString answer;
   QByteArray imgQuestion[2];
   QByteArray imgAnswer[2];
   QByteArray mp3Question;
   QByteArray mp3Answer;
};

ContentItem fromLegacyString(const QString &s);
QString toLegacyString(const ContentItem &item);

struct CourseTemplate
{
   static CourseTemplate fromFile(const QString &fileName);
   static bool toFile (const QString &fileName, const CourseTemplate &courseTemplate);
   static CourseTemplate importQA (const QString &fileName);
   static bool exportQA (const QString &fileName, const CourseTemplate &courseTemplate);

   CourseOptions options;
   std::vector<ContentItem> content;

private:
   static void trace (const QString &text, const int flags = traceLevel1|0);
};

#endif // COURSETEMPLATE_H
