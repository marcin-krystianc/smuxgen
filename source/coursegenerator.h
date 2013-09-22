//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef COURSEGENERATOR_H
#define COURSEGENERATOR_H

#include "coursetemplate.h"
#include "globaltracer.h"
#include "courseoptions.h"
#include "supermemosql.h"

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
   QDomNode getNode (QDomNode &rootElement, const QString &nodeName, QDomDocument &doc, const QString &courseFileDirectory, const QString &type, int nodeID);
   void setDelete (QDomNode &topicNode);
   bool doDelete (int courseIDSQL, int paretntIDSQL, QDomNode &docElement, QString courseFileDirectory);
   bool generateCourseElement(int courseIDSQL, const QString &question, const QString &answer, const QString &topicName
                              , QDomNode &topicNode, int topicID, QDomDocument &doc, const QString &courseFileDirectory, bool bMode, int voiceIndexA, int voiceIndexQ, bool foreceRebuild);
   bool writeDomDoucumentToFile (const QDomDocument &document, const QString &path);
   QDomDocument createCourseItem (int templateId, const QString &chapter);
   bool checkIfNewAnswers(const QString &fileName, const QString &answers);
   QDomDocument createCourseItem (int templateId, QString chapter, QString title, QString question, QString answers, int ID, bool bMode);
   static void trace (const QString &text, int flags = traceLevel1);

   CourseTemplate m_courseTemplate;
   SuperMemoSQL m_db;
   volatile bool m_abortProces;
   bool m_isFailed;
   bool m_rebuild;

protected:
   void run();

private:

};

#endif // CCOURSEGENERATOR_H
