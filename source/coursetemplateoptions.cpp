//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include "coursetemplateoptions.h"
#include "cglobaltracer.h"

#include <QString>
#include <QStringList>

/////////////////////////////////////////////////////////////////////////////
cCourseTemplateOptions::cCourseTemplateOptions()
{
}

/////////////////////////////////////////////////////////////////////////////
bool cCourseTemplateOptions::fromString(const QString &line)
{
    if (line.isEmpty())
    {
        trace("cCourseTemplateOptions::fromString - line is empty: ", traceError);
        return false;
    }

    QStringList chunks = line.split(" ", QString::SkipEmptyParts);

    int i = 0;
    while (i<chunks.count())
    {
        QString first = chunks.at(i++);

        if (first == QString::fromUtf8("-Force")) {
            bit.oForce = true;
            continue;
        }

        if (first == QString::fromUtf8("-Double")) {
            bit.oDouble = true;
            continue;
        }

        if (first == QString::fromUtf8("-VoiceQ")){
            bit.oVoiceQ = true;
            continue;
        }

        if (first == QString::fromUtf8("-VoiceA")){
            bit.oVoiceA = true;
            continue;
        }

        if (first == QString::fromUtf8("-Image")){
            bit.oImage = true;
            continue;
        }

        if (i == chunks.count())
        {
            trace("cCourseTemplateOptions::fromString - Missing value after: "+first, traceError);
            return false;
        }


        QString second = chunks.at(i++);

        // joining chunks in quotes
        while (second.startsWith("\"")^second.endsWith("\"")) // XOR
        {
            if (i >= chunks.count())
            {
                trace("cCourseTemplateOptions::fromString - wrong second parameter:"+second, traceError);
                return false; // wrong parameters
            }
            second += " ";
            second += chunks.at(i++);
        }


        if (first == QString::fromUtf8("-course")) {
            course = second.remove("\"");;
            continue;
        }

        if (first == QString::fromUtf8("-database")) {
            database = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-subname")) {
            subname = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-instruction")) {
            instruction = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-trimQ")) {
            voiceTrimQ = second.toDouble();
            continue;
        }

        if (first == QString::fromUtf8("-vNameQ")) {
            voiceNameQ = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-gainQ")) {
            voiceGainQ = second.toInt();
            continue;
        }
        if (first == QString::fromUtf8("-trimA")) {
            voiceTrimA = second.toDouble();
            continue;
        }

        if (first == QString::fromUtf8("-vNameA")) {
            voiceNameA = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-gainA")) {
            voiceGainA = second.toInt();
            continue;
        }

        trace("cCourseTemplateOptions::fromString - Unknown parameter:"+first, traceError);
        continue; // wrong parameters
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
QString cCourseTemplateOptions::toString()
{
    QString ret;

    ret+= QString("-course ") +"\""+course +"\" ";
    ret+= QString("-database ") +"\""+database +"\" ";
    ret+= QString("-subname ") +"\""+subname +"\" ";
    ret+= QString("-instruction ") +"\""+instruction +"\" ";

    ret+= QString("-trimQ ") +QString::number(voiceTrimQ) +" ";
    ret+= QString("-vNameQ ") +"\""+voiceNameQ+"\" ";
    ret+= QString("-gainQ ") +QString::number(voiceGainQ) +" ";

    ret+= QString("-trimA ") +QString::number(voiceTrimA) +" ";
    ret+= QString("-vNameA ") +"\""+voiceNameA+"\" ";
    ret+= QString("-gainA ") +QString::number(voiceGainA) +" ";

    if ( bit.oForce) ret+= "-Force ";
    if ( bit.oDouble) ret+= "-Double ";
    if ( bit.oVoiceQ) ret+= "-VoiceQ ";
    if ( bit.oVoiceA) ret+= "-VoiceA ";
    if ( bit.oImage) ret+= "-Image ";

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
void cCourseTemplateOptions::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}
/////////////////////////////////////////////////////////////////////////////
void cCourseTemplateOptions::clear()
{
    bit.oDouble = false;
    bit.oForce = false;
    bit.oVoiceQ = false;
    bit.oVoiceA = false;
    bit.oImage = false;

    course.clear();
    database.clear();
    subname.clear();
    instruction.clear();

    voiceNameQ.clear();
    voiceGainQ = 0;
    voiceTrimQ = 0;

    voiceNameA.clear();
    voiceGainA = 0;
    voiceTrimA = 0;
}
