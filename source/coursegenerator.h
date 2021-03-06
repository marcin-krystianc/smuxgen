//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef COURSEGENERATOR_H
#define COURSEGENERATOR_H

#include <QHeaderView>
#include <vector>

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
   static void DomDoucumentToFile(const QDomDocument &document, const QString &path);
   static void DomDoucumentFromFile(const QString &path, QDomDocument *document);
   static void generateCourseElement2(
        const QString &chapterName,
        const QString &instruction,
        const QString &question,
        const QString &answer,
        const QString &courseFileDirectory,
        int id,
        const QString &voiceNameA,
        int voiceGainA,
        double voiceTrimA,
        const QString &voiceNameQ,
        int voiceGainQ,
        double voiceTrimQ,
        bool graphics,
        const QString &searchUrl,
        const QString &regex);

   static QDomDocument createCourseItemDoc (QString chapterTitle, QString instruction, QString question, QString answers, int id, bool voiceA, bool voiceQ, bool graphics);
   static bool generateGraphics
   (
       const QStringList &filePaths,
       const QStringList &keywords,
       const QString &searchUrl,
       const QString &regex
    );

   static void trace (const QString &text, int flags = traceLevel1);

   QDomNode getNode (QDomDocument &doc, QDomNode &rootElement, const QString &nodeName, const QString &type, int nodeID);
   void doDelete(int courseId, int parentId, const QString &courseFileDirectory, const std::set<int> &validIds);
   bool generateCourseElement(int courseIDSQL, const QString &question, const QString &answer, const QString &topicName
                              , QDomNode &topicNode, int topicID, QDomDocument &doc, const QString &courseFileDirectory, bool bMode, int voiceIndexA, int voiceIndexQ, bool foreceRebuild);
   void buildTopic(const QString &courseName, const QString &topicName, const std::vector<QString> &questions, const std::vector<QString> &answers, const QString &voiceNameA, int voiceGainA, double voiceTrimA, const QString &voiceNameQ, int voiceGainQ, double voiceTrimQ);

   bool checkIfNewAnswers(const QString &filePath, const QString &answers);

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
