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
            this->bit.oForce = true;
            continue;
        }

        if (first == QString::fromUtf8("-Double")) {
            this->bit.oDouble = true;
            continue;
        }

        if (first == QString::fromUtf8("-VoiceQ")){
            this->bit.oVoiceQ = true;
            continue;
        }

        if (first == QString::fromUtf8("-VoiceA")){
            this->bit.oVoiceA = true;
            continue;
        }

        if (first == QString::fromUtf8("-Image")){
            this->bit.oImage = true;
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
            this->course = second.remove("\"");;
            continue;
        }

        if (first == QString::fromUtf8("-database")) {
            this->database = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-subname")) {
            this->subname = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-instruction")) {
            this->instruction = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-trimQ")) {
            this->voiceTrimQ = second.toDouble();
            continue;
        }

        if (first == QString::fromUtf8("-vNameQ")) {
            this->voiceNameQ = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-gainQ")) {
            this->voiceGainQ = second.toInt();
            continue;
        }
        if (first == QString::fromUtf8("-trimA")) {
            this->voiceTrimA = second.toDouble();
            continue;
        }

        if (first == QString::fromUtf8("-vNameA")) {
            this->voiceNameA = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-gainA")) {
            this->voiceGainA = second.toInt();
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

    ret+= QString("-course ") +"\""+this->course +"\" ";
    ret+= QString("-database ") +"\""+this->database +"\" ";
    ret+= QString("-subname ") +"\""+this->subname +"\" ";
    ret+= QString("-instruction ") +"\""+this->instruction +"\" ";

    ret+= QString("-trimQ ") +QString::number(this->voiceTrimQ) +" ";
    ret+= QString("-vNameQ ") +"\""+this->voiceNameQ+"\" ";
    ret+= QString("-gainQ ") +QString::number(this->voiceGainQ) +" ";

    ret+= QString("-trimA ") +QString::number(this->voiceTrimA) +" ";
    ret+= QString("-vNameA ") +"\""+this->voiceNameA+"\" ";
    ret+= QString("-gainA ") +QString::number(this->voiceGainA) +" ";

    if ( this->bit.oForce) ret+= "-Force ";
    if ( this->bit.oDouble) ret+= "-Double ";
    if ( this->bit.oVoiceQ) ret+= "-VoiceQ ";
    if ( this->bit.oVoiceA) ret+= "-VoiceA ";
    if ( this->bit.oImage) ret+= "-Image ";

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
    this->bit.oDouble = false;
    this->bit.oForce = false;
    this->bit.oVoiceQ = false;
    this->bit.oVoiceA = false;
    this->bit.oImage = false;

    this->course.clear();
    this->database.clear();
    this->subname.clear();
    this->instruction.clear();

    voiceNameQ.clear();
    voiceGainQ = 0;
    voiceTrimQ = 0;

    voiceNameA.clear();
    voiceGainA = 0;
    voiceTrimA = 0;
}
