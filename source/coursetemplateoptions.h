//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef COURSETEMPLATEOPTIONS_H
#define COURSETEMPLATEOPTIONS_H

#include <QString>
#include <QObject>
#include "cglobaltracer.h"

class CourseTemplateOptions {

public:
    CourseTemplateOptions();

    QString course;
    QString database;
    QString subname;
    QString instruction;

    QString voiceNameQ;
    int voiceGainQ;
    double voiceTrimQ;

    QString voiceNameA;
    int voiceGainA;
    double voiceTrimA;

    struct
    {
        unsigned int oForce : 1; // force to create agian all course elements
        unsigned int oDouble : 1; // make also alternative course
        unsigned int oVoiceQ : 1; // make mp3 files (Questions)
        unsigned int oVoiceA : 1; // make mp3 files (Answers)
        unsigned int oImage : 1; // get Images
    } bit;

    bool fromString (const QString &line);
    QString toString () ;
    void clear();

private:
    void trace (const QString &text, const int & flags = traceLevel1|0);

};

#endif // COURSETEMPLATEOPTIONS_H
