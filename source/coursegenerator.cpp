//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QProcess>
#include <QSqlError>
#include <QSqlRecord>
#include <QTextStream>
#include <QDate>
#include <QImage>
#include <QSqlQuery>

#include <algorithm>
#include <set>

#include "coursegenerator.h"
#include "globalsmuxgentools.h"
#include "sapi.h"


/////////////////////////////////////////////////////////////////////////////
CourseGenerator::CourseGenerator() :
   m_abortProces(false), m_isFailed(false),m_rebuild(false)
{

}

/////////////////////////////////////////////////////////////////////////////
CourseGenerator::~CourseGenerator()
{
   m_abortProces = true;
   wait();
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::stop()
{
   m_abortProces = true;
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::trace (const QString &text, int flags)
{
   globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::build (const CourseTemplate &courseTemplate, bool rebuild)
{
   m_courseTemplate = courseTemplate;
   m_abortProces = false;
   m_rebuild = rebuild;
   start();
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::buildTopic(const QString &courseName, const QString &topicName, const std::vector<QString> &questions, const std::vector<QString> &answers, int voiceIndexA, int voiceIndexQ)
{
   int courseId;
   QString courseMasterDir;
   if (!m_db.getCourseDetails (courseName, &courseId, &courseMasterDir))
      return false;

   QString courseDocumentPath = courseMasterDir+"\\override\\course.xml";
   QString courseDocumentDir = QFileInfo(courseDocumentPath).dir().path();

   // create media dir if doesn't exist
   QDir(courseDocumentDir).mkdir("media");

   // get root document
   QDomDocument courseDoc("");
   if (!DomDoucumentFromFile (courseDocumentPath, &courseDoc))
      return false;

   std::set<int> topicElementsIds;

   int topicId;
   if (!m_db.addItem(topicName, courseId, 0, &topicId))
      return true;

   QDomElement rootElement = courseDoc.documentElement();
   QDomNode topicNode = getNode (courseDoc, rootElement, topicName, "pres", topicId);
   // topicNode.clear();

   for (size_t i = 0; i < questions.size(); ++i) {

      if (m_abortProces)
         return false;

      emit progressSignal(QString::number(i+1)+"/"+QString::number(questions.size())+" "+topicName+"@"+courseName+":"
                          + questions[i]);

      int itemId;
      if (!m_db.addItem(getTextToPrint(questions[i]), courseId, topicId, &itemId))
         return false;

      getNode (courseDoc, topicNode, getTextToPrint(questions[i]), "pres", itemId);

      generateCourseElement2(topicName, m_courseTemplate.options.instruction, questions[i], answers[i], courseDocumentDir
                            , itemId, voiceIndexA, voiceIndexQ, false);

/*
      generateCourseElement(courseId, questions[i], answers[i]
                            , topicName, topicNode, topicId, courseDoc, courseDocumentDir
                            , false, voiceIndexA, voiceIndexQ, m_rebuild);
                            */

   }

   DomDoucumentToFile(courseDoc, courseDocumentPath);
   return true;
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::run ()
{
   m_isFailed = true;

   if (!m_db.open(m_courseTemplate.options.dbPath))
      return;

   std::vector<QString> questions;
   std::vector<QString> answers;

   for (int i = 0; i < m_courseTemplate.content.count(); ++i) {
      QString line = m_courseTemplate.content.at(i);
      QStringList list1 = line.split(":");
      if (list1.count() != 2) {
         trace(QString("cCourseGenerator::generate Input error: ")+line, traceWarning);
         continue;
      }
      questions.push_back(list1[0].trimmed());
      answers.push_back(list1[1].trimmed());
   }

   int voiceIndexA = getVoiceEngineIndex(m_courseTemplate.options.voiceNameA)+1;
   int voiceIndexQ = getVoiceEngineIndex(m_courseTemplate.options.voiceNameQ)+1;

   buildTopic(m_courseTemplate.options.courseName, m_courseTemplate.options.subname, questions, answers, voiceIndexA, voiceIndexQ);

   if (m_courseTemplate.options.bothDirections) {
      buildTopic(m_courseTemplate.options.courseName, m_courseTemplate.options.subname+"*", answers, questions, voiceIndexQ, voiceIndexA);
   }

   m_isFailed = false;
}

/////////////////////////////////////////////////////////////////////////////
QDomNode CourseGenerator::getNode
(
      QDomDocument &doc,
      QDomNode &rootElement,
      const QString &nodeName,
      const QString &type,
      int nodeID
      )
{
   for (QDomNode node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling()) {
      QDomElement e = node.toElement(); // try to convert the node to an element.
      if(e.isNull())
         continue;

      if (e.attribute("id", "0").toInt() == nodeID)
         return node; // topic Found
   }

   trace(QString("getNode createNewNode Name: ")+nodeName+QString(" ID:")+QString::number(nodeID), traceLevel2);

   QDomElement newElement = doc.createElement("element");
   newElement.setAttribute("name", nodeName);
   newElement.setAttribute("type", type);
   newElement.setAttribute("id", QString::number(nodeID));

   return rootElement.appendChild(newElement);
}
/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::generateCourseElement2
(
      const QString &chapterName,
      const QString &instruction,
      const QString &question,
      const QString &answer,
      const QString &courseFileDirectory,
      int id,
      int voiceIndexA,
      int voiceIndexQ,
      bool withImages
      )
{
   QDomDocument docItem = createCourseItemDoc (chapterName, instruction
                                           , getTextToPrint(question), getTextToPrint(answer), id);
   DomDoucumentToFile(docItem, courseFileDirectory+"\\"+getFileName(id));
   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::generateCourseElement(int courseIDSQL, const QString &question, const QString &answer
                                            , const QString &topicName, QDomNode &topicNode, int topicID
                                            , QDomDocument &doc, const QString &courseFileDirectory, bool bMode
                                            , int voiceIndexA, int voiceIndexQ, bool foreceRebuild)
{
   int ID = 0;
   bool forceMedia = foreceRebuild;
   const int timeOut = -1; // no timeout
   QProcess myProcess;

   if (!m_db.addItem(getTextToPrint(question), courseIDSQL, topicID, &ID))
      return false;

   // create xml course file
   if (foreceRebuild ||
       checkIfNewAnswers(courseFileDirectory+"\\"+getFileName(ID), answer)) {
      forceMedia = true;
      //QDomDocument docItem = createCourseItemDoc(1, topicName, m_courseTemplate.options.instruction, getTextToPrint(question), getTextToPrint(answer), ID, bMode);
      //DomDoucumentToFile(docItem, courseFileDirectory+"\\"+getFileName(ID));
   }

   // create mp3
   QString mp3Q = bMode ? "a" : "q";
   if (m_courseTemplate.options.voiceQ &&
       (forceMedia ||
        (!checkIsFileOk(courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+mp3Q+".mp3")))) {
      QStringList arguments; // filename, text, trim, gain
      arguments.append(courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+mp3Q);
      arguments.append(bMode ? getTranscript(QString(answer).replace("|", " ")):getTranscript(QString(question).replace("|", " ")));
      arguments.append(QString::number(voiceIndexQ));
      arguments.append(QString::number(m_courseTemplate.options.voiceTrimQ));
      arguments.append(QString::number(m_courseTemplate.options.voiceGainQ));

      trace(QString("createMp3.bat ")+arguments.join(" "), traceLevel1);

      myProcess.start("createMp3.bat", arguments);
      if (!myProcess.waitForStarted())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
      myProcess.waitForFinished(timeOut);
      if (myProcess.exitCode())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
   }

   QString mp3A = bMode ? "q" : "a";
   if (m_courseTemplate.options.voiceA &&
       ((forceMedia) ||
        (!checkIsFileOk(courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+mp3A+".mp3")))) {
      QStringList arguments; // filename, text, trim, gain
      arguments.append(courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+mp3A);
      arguments.append(!bMode ? getTranscript(QString(answer).replace("|", " ")):getTranscript(QString(question).replace("|", " ")));
      arguments.append(QString::number(voiceIndexA));
      arguments.append(QString::number(m_courseTemplate.options.voiceTrimA));
      arguments.append(QString::number(m_courseTemplate.options.voiceGainA));

      trace(QString("createMp3.bat ")+arguments.join(" "), traceLevel1);

      myProcess.start("createMp3.bat", arguments);
      if (!myProcess.waitForStarted())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
      myProcess.waitForFinished(timeOut);
      if (myProcess.exitCode())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
   }

   // create jpg
   if (m_courseTemplate.options.graphics &&
       ((forceMedia) ||
        (!checkIsFileOk(courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+"m.jpg")) ||
        (!checkIsFileOk(courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+"n.jpg")))) {
      deleteFile(TMPDIR+"HTML");
      QStringList arguments;

      arguments.append(getKeyWord(question));
      arguments.append(TMPDIR+"HTML");

      myProcess.start("getGoogleHtml.bat", arguments);
      if (!myProcess.waitForStarted())
         trace(QString("Error:getGoogleHtml.bat ")+arguments.join(" "), traceError);
      myProcess.waitForFinished(timeOut);
      if (myProcess.exitCode())
         trace(QString("Error:getGoogleHtml.bat ")+arguments.join(" "), traceError);

      QStringList fileUrls = parseGoogleHtml(TMPDIR+"HTML");

      int i = 0; // download 2 images
      while ((fileUrls.count())>0 && (i<2)) {
         QString fileName = courseFileDirectory+"\\media"+QDir::separator()+getMediaFileName(ID)+('m'+i)+".jpg";
         //QString filrExt = ;
         deleteFile(fileName);
         arguments.clear();
         arguments.append(fileUrls.first()+QString(" "));
         arguments.append(fileName+" ");

         trace(QString("getImage.bat ")+arguments.join(" "), traceLevel1);

         myProcess.start("getImage.bat", arguments);
         if (!myProcess.waitForStarted())
            trace(QString("Error:getImage.bat ")+arguments.join(" "), traceError);
         myProcess.waitForFinished(timeOut);
         if (myProcess.exitCode())
            trace(QString("Error:getImage.bat ")+arguments.join(" "), traceError);

         if (!scalePicture(fileName, IMG_WIDTH, IMG_HEIGHT)) {
            trace(QString("Error:scalePicture ")+fileName, traceError);
            deleteFile(fileName);
         }

         if (checkIsFileOk(fileName))
            i++;
         fileUrls.removeFirst();
      }
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::doDelete (int courseIDSQL, int paretntIDSQL, QDomNode &docElement, QString courseFileDirectory)
{
   QDomNode n = docElement.firstChild();
   std::set<int> validIds;

   trace (QString("doDelete: id:")
          + QString(" CourseId:")+QString::number(courseIDSQL)
          + QString(" ParetntId:")+QString::number(paretntIDSQL)
          , traceLevel2);

   while(!n.isNull()) {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull()) {
         if (e.attribute("delete", "none") == "true") {
            trace(QString("doDelete: id:")+e.attribute("id", "0")+QString(" name:")+e.attribute("name", ""), traceLevel2);
            int ID = e.attribute("id", "0").toInt();
            deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"a.mp3");
            deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"q.mp3");
            deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"m.jpg");
            deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"n.jpg");

            QDomNode tmp = n;
            n = n.nextSibling();
            docElement.removeChild(tmp);
            continue;
         }
         validIds.insert(e.attribute("id", "0").toInt());
         trace(QString("validID:")+e.attribute("id", "0"), traceLevel2);
      }
      n = n.nextSibling();
   }

   if (!m_db.deleteNotValidItems(courseIDSQL, paretntIDSQL, validIds))
      return false;

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::DomDoucumentToFile (const QDomDocument &document, const QString &path)
{
   QFile file(path);
   if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
      return false;

   QTextStream ts(&file);
   ts.setCodec("UTF-8");
   ts << document.toString();

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::DomDoucumentFromFile(const QString &path, QDomDocument *document)
{
   QFile docFile(path);
   if (!document->setContent(&docFile)) {
      trace(QString("Cannot open file: ") + path, traceError);
      return false;
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::checkIfNewAnswers(const QString &filePath, const QString &answer)
{
   QDomDocument doc("");
   QFile docFile(filePath);
   if (!doc.setContent(&docFile)) {
      trace(QString("cCourseGenerator::checkIfNewAnswers file not exist:")+filePath, traceLevel1);
      return true;
   }

   QDomElement rootElement = doc.documentElement();
   QDomNode n = rootElement.firstChildElement("question");
   if (n.isNull())
      return true;

   QDomNode n1 = n.firstChildElement("spellpad");
   if (n1.isNull())
      return true;

   QDomElement e = n1.toElement(); // try to convert the node to an element.
   if(e.isNull())
      return true;

   QString oldAnswer = e.attribute("correct", "");
   if (oldAnswer == answer)
      return false;

   trace(QString("New answers: ")+oldAnswer+":"+answer, traceLevel1);
   return true;
}

/////////////////////////////////////////////////////////////////////////////
QDomDocument CourseGenerator::createCourseItemDoc (QString chapter, QString title, QString question, QString answer, int ID, bool bMode, bool withGraphic)
{

   QString graphics = !withGraphic ? "" :
                                     "<br/><br/><table width=_100%_ cellspacing=_0_ cellpadding=_0_ border=_0_>"
                                     "<tr><td align=_center_>"
                                     "<gfx space=_1_ width=_320_ scale-base=_900_ height=_320_ file=_m_ auto-play=_false_/>"
                                     "</td><td align=_center_>"
                                     "<gfx space=_1_ width=_320_ scale-base=_900_ height=_320_ file=_n_ auto-play=_false_/>"
                                     "</td></tr></table>";

   QString audioQuestion = "<question-audio>true</question-audio>";
   QString audioAnswer = "<answer-audio>true</answer-audio>";


   QString item =
         "<item xmlns=_http://www.supermemo.net/2006/smux_>"
         "<chapter-title>%1</chapter-title>"
         "<modified>%2</modified>"
         "<template-id>1</template-id>"
         "<lesson-title>%3</lesson-title>"
         "<question-title>%4</question-title>"
         "<question>%5<spellpad correct=%6/>"
         "%7"
         "</question>"
         "%8"
         "%9"
         "</item>";


   item = item.arg(chapter, QDate::currentDate().toString(), QString::number(ID, 10),  title
         , question, answer, graphics, audioQuestion, audioAnswer);
   item.replace('_', '\"');

   QDomDocument doc;
   if (!doc.setContent(item))
      return doc;
   return doc;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::isFailed()
{
   return m_isFailed;
}
