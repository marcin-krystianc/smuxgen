//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CCOURSEGENERATOR_H
#define CCOURSEGENERATOR_H

#include "coursetemplate.h"
#include "cglobaltracer.h"
#include "coursetemplateoptions.h"
#include "csupermemosql.h"

#include <QDomDocument>
#include <QObject>
#include <QThread>
#include <QMutex>

class cCourseGenerator : public QThread
{
    Q_OBJECT
    public:
        cCourseGenerator();
        ~cCourseGenerator();

        void generate (const cCourseTemplate &courseTemplate);
        int getStatus();

    public slots:
        void stop();

    signals:
        void progressSignal (const QString&);

    private:
        cSuperMemoSQL database;
        cCourseTemplate courseTemplate;

        bool abortProces;;
        QMutex mutex;

        void trace (const QString &text,const int & flags = traceLevel1|0);
        QDomNode getNode (QDomNode &rootElement,QString nodeName,QDomDocument &doc,QString courseFileDirectory,QString type,int retID);
        void setDelete (QDomNode &topicNode);
        bool doDelete (int courseIDSQL,int paretntIDSQL,QDomNode &docElement,QString courseFileDirectory);
        bool generateCourseElement(int courseIDSQL,QString question,QString answer,QString topicName,QDomNode &topicNode,int topicID,QDomDocument &doc,QString courseFileDirectory,bool bMode);
        int writeDomDoucumentToFile (QDomDocument &document,QString path);
        QDomDocument createCourseItem (int templateId,QString chapter);
        bool checkIfNewAnswers(QString fileName,QString answers);
        QDomDocument createCourseItem (int templateId,QString chapter,QString title,QString question,QString answers,int ID,bool bMode);
        int status;

    protected:
        void run();
};

#endif // CCOURSEGENERATOR_H
