//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <stdexcept>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
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
   //terminate();
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
void CourseGenerator::run ()
{
   try {
      m_isFailed = true;

      if (!m_db.openUser(m_courseTemplate.options.user))
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

      buildTopic(m_courseTemplate.options.courseName, m_courseTemplate.options.subname
                 , questions, answers
                 , m_courseTemplate.options.voiceNameA, m_courseTemplate.options.voiceGainA, m_courseTemplate.options.voiceTrimA
                 , m_courseTemplate.options.voiceNameQ, m_courseTemplate.options.voiceGainQ, m_courseTemplate.options.voiceTrimQ );

      if (m_courseTemplate.options.bothDirections) {
         buildTopic(m_courseTemplate.options.courseName, m_courseTemplate.options.subname+"*"
                    , answers, questions
                    , m_courseTemplate.options.voiceNameQ, m_courseTemplate.options.voiceGainQ, m_courseTemplate.options.voiceTrimQ
                    , m_courseTemplate.options.voiceNameA, m_courseTemplate.options.voiceGainA, m_courseTemplate.options.voiceTrimA );
      }

      m_isFailed = false;
   }
   catch (const std::exception &e){
      trace(e.what(), traceError);
   }
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
void CourseGenerator::buildTopic
(
      const QString &courseName,
      const QString &topicName,
      const std::vector<QString> &questions,
      const std::vector<QString> &answers,
      const QString &voiceNameA,
      int voiceGainA,
      double voiceTrimA,
      const QString &voiceNameQ,
      int voiceGainQ,
      double voiceTrimQ
      )
{
   int courseId;
   QString courseMasterDir;
   m_db.getCourseDetails (courseName, &courseId, &courseMasterDir);

   QString courseDocumentPath = courseMasterDir+"\\override\\course.xml";
   QString courseDocumentDir = QFileInfo(courseDocumentPath).dir().path();

   // create media dir if doesn't exist
   QDir(courseDocumentDir).mkdir("media");

   // get root document
   QDomDocument courseDoc("");
   DomDoucumentFromFile (courseDocumentPath, &courseDoc);

   int topicId;
   m_db.addItem(topicName, courseId, 0, &topicId);

   QDomElement rootElement = courseDoc.documentElement();
   QDomNode topicNode = getNode (courseDoc, rootElement, topicName, "pres", topicId);
   while (topicNode.hasChildNodes()) {
      topicNode.removeChild(topicNode.lastChild());
   }

   std::set<int> topicElementsIds;
   for (size_t i = 0; i < questions.size(); ++i) {

      if (m_abortProces)
         return;

      emit progressSignal(QString::number(i+1)+"/"+QString::number(questions.size())+" "+topicName+"@"+courseName+":"
                          + questions[i]);

      int itemId;
      m_db.addItem(getTextToPrint(questions[i]), courseId, topicId, &itemId);
      topicElementsIds.insert(itemId);

      getNode (courseDoc, topicNode, getTextToPrint(questions[i]), "exercise", itemId);

      QString mediaDir = courseDocumentDir+"\\media\\";
      bool generateCourseElement = m_rebuild;

      if (checkIfNewAnswers(courseDocumentDir+"\\"+getFileName(itemId), answers[i]))
         generateCourseElement = true;

      if (m_courseTemplate.options.graphics) {
         if (!checkIsFileOk(mediaDir+getMediaFileName(itemId)+"m.jpg") ||
             !checkIsFileOk(mediaDir+getMediaFileName(itemId)+"n.jpg")) {
            generateCourseElement = true;
         }
      }

      if (voiceNameQ != 0 &&
          !checkIsFileOk(mediaDir+getMediaFileName(itemId)+"q.mp3")) {
         generateCourseElement = true;
      }

      if (voiceNameA != 0 &&
          !checkIsFileOk(mediaDir+getMediaFileName(itemId)+"a.mp3")) {
         generateCourseElement = true;
      }

      if (generateCourseElement) {
         generateCourseElement2(topicName, m_courseTemplate.options.instruction
                                , questions[i], answers[i]
                                , courseDocumentDir
                                , itemId
                                , voiceNameA, voiceGainA, voiceTrimA
                                , voiceNameQ, voiceGainQ, voiceTrimQ
                                , m_courseTemplate.options.graphics);
      }
   }

   doDelete(courseId, topicId, courseDocumentDir, topicElementsIds);
   DomDoucumentToFile(courseDoc, courseDocumentPath);
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::generateCourseElement2
(
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
      bool graphics
      )
{
   int voiceIndexA = getVoiceEngineIndex(voiceNameA);
   int voiceIndexQ = getVoiceEngineIndex(voiceNameQ);

   QDomDocument docItem =  createCourseItemDoc (chapterName, instruction, getTextToPrint(question), getTextToPrint(answer)
                                                , id, voiceIndexA != 0, voiceIndexQ != 0, graphics);
   DomDoucumentToFile(docItem, courseFileDirectory+"\\"+getFileName(id));

   QString mediaDir = courseFileDirectory + "\\media\\";

   if (voiceIndexQ != 0) {
      QString filePath = mediaDir + getMediaFileName(id)+"q.mp3";
      generateMp3(filePath, getTranscript(QString(question).replace("|", " ")), voiceIndexQ, voiceGainQ, voiceTrimQ);
   }

   if (voiceIndexA != 0) {
      QString filePath = mediaDir + getMediaFileName(id)+"a.mp3";
      generateMp3(filePath, getTranscript(QString(answer).replace("|", " ")), voiceIndexA, voiceGainA, voiceTrimA);
   }

   if (graphics) {
      QStringList filePaths;
      filePaths.append(mediaDir + getMediaFileName(id)+"m.jpg");
      filePaths.append(mediaDir + getMediaFileName(id)+"n.jpg");
      generateGraphics(filePaths, question);
   }
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::doDelete
(
      int courseId,
      int parentId,
      const QString &courseFileDirectory,
      const std::set<int> &validIds
      )
{
   std::set<int> alIds;
   m_db.getItems(courseId, parentId, &alIds);

   std::set<int> invalidIds;
   for (std::set<int>::iterator i=alIds.begin(); i!=alIds.end(); ++i) {
      if (validIds.find(*i) == validIds.end()) {
         invalidIds.insert(*i);
      }
   }

   QString mediaDir = courseFileDirectory + "\\media\\";
   for (std::set<int>::iterator i=invalidIds.begin(); i!=invalidIds.end(); ++i) {
      trace(QString("Deleteing: ") + QString::number(*i), traceLevel2);
      deleteFile(courseFileDirectory+"\\"+getFileName(*i));
      deleteFile(mediaDir+getMediaFileName(*i)+"a.mp3");
      deleteFile(mediaDir+getMediaFileName(*i)+"q.mp3");
      deleteFile(mediaDir+getMediaFileName(*i)+"m.jpg");
      deleteFile(mediaDir+getMediaFileName(*i)+"n.jpg");
      m_db.deleteItem(courseId, parentId, *i);
   }
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::DomDoucumentToFile (const QDomDocument &document, const QString &path)
{
   QFile file(path);
   if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
      throw std::runtime_error (std::string("Cannot open file: ")+path.toStdString());

   QTextStream ts(&file);
   ts.setCodec("UTF-8");
   ts << document.toString();
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::DomDoucumentFromFile(const QString &path, QDomDocument *document)
{
   QFile docFile(path);
   if (!document->setContent(&docFile))
      throw std::runtime_error (std::string("Cannot open file: ")+path.toStdString());
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
QDomDocument CourseGenerator::createCourseItemDoc
(
      QString chapterTitle,
      QString instruction,
      QString question,
      QString answer,
      int id,
      bool voiceA,
      bool voiceQ,
      bool graphics
      )
{
   QDomDocument doc;
   QDomElement rootElement = doc.createElement("item");
   rootElement.setAttribute("xmlns", "http://www.supermemo.net/2006/smux");
   doc.appendChild(rootElement);

   QDomElement tmpElement0 = doc.createElement("chapter-title");
   tmpElement0.appendChild(doc.createTextNode(chapterTitle));
   rootElement.appendChild(tmpElement0);

   QDomElement tmpElement1 = doc.createElement("modified");
   QDate x = QDate::currentDate();
   tmpElement1.appendChild(doc.createTextNode(x.toString("yyyy-MM-dd")));
   rootElement.appendChild(tmpElement1);

   QDomElement tmpElement2 = doc.createElement("template-id");
   tmpElement2.appendChild(doc.createTextNode(QString::number(1)));
   rootElement.appendChild(tmpElement2);

   QDomElement tmpElement3 = doc.createElement("lesson-title");
   tmpElement3.appendChild(doc.createTextNode(QString::number(id)));
   rootElement.appendChild(tmpElement3);

   QDomElement tmpElement4 = doc.createElement("question-title");
   tmpElement4.appendChild(doc.createTextNode(instruction));
   rootElement.appendChild(tmpElement4);

   QDomElement tmpElement5 = doc.createElement("question");
   tmpElement5.appendChild(doc.createTextNode(question+" - "));

   QDomElement tmpElement6 = doc.createElement("spellpad");
   tmpElement6.setAttribute("correct", answer);
   tmpElement5.appendChild(tmpElement6);

   if (graphics) {
      // create table with images
      tmpElement5.appendChild(doc.createElement("br"));
      tmpElement5.appendChild(doc.createElement("br"));

      QDomElement table = doc.createElement("table");
      table.setAttribute("width", "100%");
      table.setAttribute("border", "0");
      table.setAttribute("cellpadding", "0");
      table.setAttribute("cellspacing", "0");

      QDomElement tr = doc.createElement("tr");
      QDomElement td = doc.createElement("td");
      td.setAttribute("align", "center");

      QDomElement gfx = doc.createElement("gfx");
      gfx.setAttribute("file", "m");
      gfx.setAttribute("space", "1");
      gfx.setAttribute("width" , QString::number(IMG_WIDTH));
      gfx.setAttribute("height" , QString::number(IMG_HEIGHT));
      gfx.setAttribute("scale-base" , QString::number(900));

      td.appendChild(gfx);
      tr.appendChild(td);
      td = doc.createElement("td");
      td.setAttribute("align", "center");

      gfx = doc.createElement("gfx");
      gfx.setAttribute("file", "n");
      gfx.setAttribute("space", "1");
      gfx.setAttribute("width" , QString::number(IMG_WIDTH));
      gfx.setAttribute("height" , QString::number(IMG_HEIGHT));
      gfx.setAttribute("scale-base" , QString::number(900));
      td.appendChild(gfx);
      tr.appendChild(td);
      table.appendChild(tr);
      tmpElement5.appendChild(table);
   }


   rootElement.appendChild(tmpElement5);

   if (voiceQ) {
      QDomElement tmpElement8 = doc.createElement("question-audio");
      tmpElement8.appendChild(doc.createTextNode("true"));
      rootElement.appendChild(tmpElement8);
   }

   if (voiceA) {
      QDomElement tmpElement7 = doc.createElement("answer-audio");
      tmpElement7.appendChild(doc.createTextNode("true"));
      rootElement.appendChild(tmpElement7);
   }

   return doc;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::generateGraphics(const QStringList &filePaths, const QString &question)
{
   QString htmlFilePath = QDir::toNativeSeparators(QDir::tempPath()+"\\google.html");
   QStringList arguments;
   arguments.append(getKeyWord(question));
   arguments.append(htmlFilePath);

   if (!runExternalTool("getGoogleHtml.bat", arguments))
      return false;

   QStringList fileUrls = parseGoogleHtml(htmlFilePath);
   int i = 0;
   while ((fileUrls.count())>0 && (i<filePaths.count())) {
      QString imgFilePath = QDir::toNativeSeparators(filePaths[i]);

      QStringList arguments2;
      arguments2.append(fileUrls.first());
      arguments2.append(imgFilePath);

      runExternalTool("getImage.bat", arguments2);

      if (!scalePicture(imgFilePath , IMG_WIDTH, IMG_HEIGHT)) {
         trace(QString("Error:scalePicture ")+imgFilePath , traceError);
         deleteFile(imgFilePath );
      }

      if (checkIsFileOk(imgFilePath ))
         i++;

      fileUrls.removeFirst();
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::isFailed()
{
   return m_isFailed;
}
