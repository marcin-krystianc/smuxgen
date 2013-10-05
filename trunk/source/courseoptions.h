//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef COURSEOPTIONS_H
#define COURSEOPTIONS_H

#include <QString>
#include <QObject>
#include "globaltracer.h"

struct CourseOptions {
   CourseOptions();
   static CourseOptions fromString (const QString &line);
   static QString toString (const CourseOptions &options);

   bool bothDirections; // make also alternative course
   bool graphics; // get Images

   QString user;
   QString courseName;

   QString subname;
   QString instruction;

   QString voiceNameQ;
   int voiceGainQ;
   double voiceTrimQ;

   QString voiceNameA;
   int voiceGainA;
   double voiceTrimA;

private:
   static void trace (const QString &text, const int & flags = traceLevel1|0);
};

#endif
