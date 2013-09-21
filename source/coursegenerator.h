//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CCOURSEGENERATOR_H
#define CCOURSEGENERATOR_H

#include "coursetemplate.h"
#include "cglobaltracer.h"
#include "courseoptions.h"
#include "csupermemosql.h"

#include <QDomDocument>
#include <QObject>
#include <QThread>
#include <QMutex>

class CourseGenerator : public QThread
{
    Q_OBJECT
public:
    CourseGenerator();
    ~CourseGenerator();

    void build (const CourseTemplate &m_courseTemplate, bool rebuild);
    bool isFailed();

public slots:
    void stop();

signals:
    void progressSignal (const QString&);

private:
    volatile bool m_abortProces;
    SuperMemoSQL m_db;
    CourseTemplate m_courseTemplate;
    bool m_isFailed;
    bool m_rebuild;

    void trace (const QString &text, int flags = traceLevel1);
    QDomNode getNode (QDomNode &rootElement, QString nodeName, QDomDocument &doc, QString courseFileDirectory, QString type, int retID);
    void setDelete (QDomNode &topicNode);
    bool doDelete (int courseIDSQL, int paretntIDSQL, QDomNode &docElement, QString courseFileDirectory);
    bool generateCourseElement(int courseIDSQL, const QString &question, const QString &answer, const QString &topicName
                               , QDomNode &topicNode, int topicID, QDomDocument &doc, const QString &courseFileDirectory, bool bMode, int voiceIndexA, int voiceIndexQ, bool foreceRebuild);
    int writeDomDoucumentToFile (QDomDocument &document, QString path);
    QDomDocument createCourseItem (int templateId, QString chapter);
    bool checkIfNewAnswers(const QString &fileName, const QString &answers);
    QDomDocument createCourseItem (int templateId, QString chapter, QString title, QString question, QString answers, int ID, bool bMode);

protected:
    void run();
};

#endif // CCOURSEGENERATOR_H
