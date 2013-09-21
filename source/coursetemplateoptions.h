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

class CourseOptions {

public:
    CourseOptions();

    QString m_courseName;
    QString m_dbPath;
    QString m_subname;
    QString m_instruction;

    QString m_voiceNameQ;
    int m_voiceGainQ;
    double m_voiceTrimQ;

    QString m_voiceNameA;
    int m_voiceGainA;
    double m_voiceTrimA;

    bool m_oForce; // force to create agian all course elements
    bool m_oBothDirections; // make also alternative course
    bool m_oVoiceQ; // make mp3 files (Questions)
    bool m_oVoiceA; // make mp3 files (Answers)
    bool m_oImage; // get Images

    bool fromString (const QString &line);
    static QString toString (const CourseOptions &options);
    void clear();

private:
    static void trace (const QString &text, const int & flags = traceLevel1|0);
};

#endif // COURSETEMPLATEOPTIONS_H
