//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include "courseoptions.h"
#include "cglobaltracer.h"

#include <QString>
#include <QStringList>

/////////////////////////////////////////////////////////////////////////////
CourseOptions::CourseOptions() :
    bothDirections(false), voiceQ(false), voiceA(false), graphics(false),
    voiceGainQ(0), voiceTrimQ(0), voiceGainA(0), voiceTrimA(0)
{

}

/////////////////////////////////////////////////////////////////////////////
CourseOptions CourseOptions::fromString (const QString &line)
{
    CourseOptions options;
    QStringList chunks = line.split(" ", QString::SkipEmptyParts);
    int i = 0;
    while (i<chunks.count())
    {
        QString first = chunks.at(i++);

        if (first == "-Double") {
            options.bothDirections = true;
            continue;
        }

        if (first == "-VoiceQ"){
            options.voiceQ = true;
            continue;
        }

        if (first == "-VoiceA"){
            options.voiceA = true;
            continue;
        }

        if (first == "-Image"){
            options.graphics = true;
            continue;
        }

        if (i == chunks.count()) {
            trace("cCourseTemplateOptions::fromString - Missing value after: "+first, traceError);
            return options;
        }


        QString second = chunks.at(i++);

        // joining chunks in quotes
        while (second.startsWith("\"")^second.endsWith("\"")) {
            if (i >= chunks.count()) {
                trace("cCourseTemplateOptions::fromString - wrong second parameter:"+second, traceError);
                return options; // wrong parameters
            }
            second += " ";
            second += chunks.at(i++);
        }


        if (first == "-course") {
            options.courseName = second.remove("\"");;
            continue;
        }

        if (first == "-database") {
            options.dbPath = second.remove("\"");
            continue;
        }

        if (first == "-subname") {
            options.subname = second.remove("\"");
            continue;
        }

        if (first == "-instruction") {
            options.instruction = second.remove("\"");
            continue;
        }

        if (first == "-trimQ") {
            options.voiceTrimQ = second.toDouble();
            continue;
        }

        if (first == "-vNameQ") {
            options.voiceNameQ = second.remove("\"");
            continue;
        }

        if (first == "-gainQ") {
            options.voiceGainQ = second.toInt();
            continue;
        }
        if (first == "-trimA") {
            options.voiceTrimA = second.toDouble();
            continue;
        }

        if (first == "-vNameA") {
            options.voiceNameA = second.remove("\"");
            continue;
        }

        if (first == "-gainA") {
            options.voiceGainA = second.toInt();
            continue;
        }

        trace("cCourseTemplateOptions::fromString - Unknown parameter:"+first, traceError);
        continue; // wrong parameters
    }
    return options;
}

/////////////////////////////////////////////////////////////////////////////
QString CourseOptions::toString(const CourseOptions &options)
{
    QString ret;

    ret+= QString("-course ") +"\""+options.courseName +"\" ";
    ret+= QString("-database ") +"\""+options.dbPath +"\" ";
    ret+= QString("-subname ") +"\""+options.subname +"\" ";
    ret+= QString("-instruction ") +"\""+options.instruction +"\" ";

    ret+= QString("-trimQ ") +QString::number(options.voiceTrimQ) +" ";
    ret+= QString("-vNameQ ") +"\""+options.voiceNameQ+"\" ";
    ret+= QString("-gainQ ") +QString::number(options.voiceGainQ) +" ";

    ret+= QString("-trimA ") +QString::number(options.voiceTrimA) +" ";
    ret+= QString("-vNameA ") +"\""+options.voiceNameA+"\" ";
    ret+= QString("-gainA ") +QString::number(options.voiceGainA) +" ";

    if (options.bothDirections) ret+= "-Double ";
    if (options.voiceQ) ret+= "-VoiceQ ";
    if (options.voiceA) ret+= "-VoiceA ";
    if (options.graphics) ret+= "-Image ";

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
void CourseOptions::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

