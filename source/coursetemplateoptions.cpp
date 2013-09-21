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
CourseTemplateOptions::CourseTemplateOptions()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CourseTemplateOptions::fromString(const QString &line)
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
            m_courseName = second.remove("\"");;
            continue;
        }

        if (first == QString::fromUtf8("-database")) {
            m_dbPath = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-subname")) {
            m_subname = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-instruction")) {
            m_instruction = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-trimQ")) {
            m_voiceTrimQ = second.toDouble();
            continue;
        }

        if (first == QString::fromUtf8("-vNameQ")) {
            m_voiceNameQ = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-gainQ")) {
            m_voiceGainQ = second.toInt();
            continue;
        }
        if (first == QString::fromUtf8("-trimA")) {
            m_voiceTrimA = second.toDouble();
            continue;
        }

        if (first == QString::fromUtf8("-vNameA")) {
            m_voiceNameA = second.remove("\"");
            continue;
        }

        if (first == QString::fromUtf8("-gainA")) {
            m_voiceGainA = second.toInt();
            continue;
        }

        trace("cCourseTemplateOptions::fromString - Unknown parameter:"+first, traceError);
        continue; // wrong parameters
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
QString CourseTemplateOptions::toString()
{
    QString ret;

    ret+= QString("-course ") +"\""+m_courseName +"\" ";
    ret+= QString("-database ") +"\""+m_dbPath +"\" ";
    ret+= QString("-subname ") +"\""+m_subname +"\" ";
    ret+= QString("-instruction ") +"\""+m_instruction +"\" ";

    ret+= QString("-trimQ ") +QString::number(m_voiceTrimQ) +" ";
    ret+= QString("-vNameQ ") +"\""+m_voiceNameQ+"\" ";
    ret+= QString("-gainQ ") +QString::number(m_voiceGainQ) +" ";

    ret+= QString("-trimA ") +QString::number(m_voiceTrimA) +" ";
    ret+= QString("-vNameA ") +"\""+m_voiceNameA+"\" ";
    ret+= QString("-gainA ") +QString::number(m_voiceGainA) +" ";

    if ( bit.oForce) ret+= "-Force ";
    if ( bit.oDouble) ret+= "-Double ";
    if ( bit.oVoiceQ) ret+= "-VoiceQ ";
    if ( bit.oVoiceA) ret+= "-VoiceA ";
    if ( bit.oImage) ret+= "-Image ";

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
void CourseTemplateOptions::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}
/////////////////////////////////////////////////////////////////////////////
void CourseTemplateOptions::clear()
{
    bit.oDouble = false;
    bit.oForce = false;
    bit.oVoiceQ = false;
    bit.oVoiceA = false;
    bit.oImage = false;

    m_courseName.clear();
    m_dbPath.clear();
    m_subname.clear();
    m_instruction.clear();

    m_voiceNameQ.clear();
    m_voiceGainQ = 0;
    m_voiceTrimQ = 0;

    m_voiceNameA.clear();
    m_voiceGainA = 0;
    m_voiceTrimA = 0;
}
