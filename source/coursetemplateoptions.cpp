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
    clear();
}

/////////////////////////////////////////////////////////////////////////////
bool CourseTemplateOptions::fromString(const QString &line)
{
    QStringList chunks = line.split(" ", QString::SkipEmptyParts);
    int i = 0;
    while (i<chunks.count())
    {
        QString first = chunks.at(i++);

        if (first == "-Force") {
            m_oForce = true;
            continue;
        }

        if (first == "-Double") {
            m_oBothDirections = true;
            continue;
        }

        if (first == "-VoiceQ"){
            m_oVoiceQ = true;
            continue;
        }

        if (first == "-VoiceA"){
            m_oVoiceA = true;
            continue;
        }

        if (first == "-Image"){
            m_oImage = true;
            continue;
        }

        if (i == chunks.count()) {
            trace("cCourseTemplateOptions::fromString - Missing value after: "+first, traceError);
            return false;
        }


        QString second = chunks.at(i++);

        // joining chunks in quotes
        while (second.startsWith("\"")^second.endsWith("\"")) {
            if (i >= chunks.count()) {
                trace("cCourseTemplateOptions::fromString - wrong second parameter:"+second, traceError);
                return false; // wrong parameters
            }
            second += " ";
            second += chunks.at(i++);
        }


        if (first == "-course") {
            m_courseName = second.remove("\"");;
            continue;
        }

        if (first == "-database") {
            m_dbPath = second.remove("\"");
            continue;
        }

        if (first == "-subname") {
            m_subname = second.remove("\"");
            continue;
        }

        if (first == "-instruction") {
            m_instruction = second.remove("\"");
            continue;
        }

        if (first == "-trimQ") {
            m_voiceTrimQ = second.toDouble();
            continue;
        }

        if (first == "-vNameQ") {
            m_voiceNameQ = second.remove("\"");
            continue;
        }

        if (first == "-gainQ") {
            m_voiceGainQ = second.toInt();
            continue;
        }
        if (first == "-trimA") {
            m_voiceTrimA = second.toDouble();
            continue;
        }

        if (first == "-vNameA") {
            m_voiceNameA = second.remove("\"");
            continue;
        }

        if (first == "-gainA") {
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

    if ( m_oForce) ret+= "-Force ";
    if ( m_oBothDirections) ret+= "-Double ";
    if ( m_oVoiceQ) ret+= "-VoiceQ ";
    if ( m_oVoiceA) ret+= "-VoiceA ";
    if ( m_oImage) ret+= "-Image ";

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
    m_oBothDirections = false;
    m_oForce = false;
    m_oVoiceQ = false;
    m_oVoiceA = false;
    m_oImage = false;

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
