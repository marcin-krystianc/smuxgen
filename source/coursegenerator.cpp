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
void CourseGenerator::run ()
{
   m_isFailed = true;

   if (!m_db.open(m_courseTemplate.options.dbPath))
      return;

   int courseId;
   QString courseFileName;
   if (!m_db.getCourseDetails (m_courseTemplate.options.courseName, &courseId, &courseFileName))
      return;

   QString courseFileDirectoryName = QFileInfo(courseFileName).dir().path()+QDir::separator();
   QDir(courseFileDirectoryName).mkdir("media");

   // get root document
   QFile docFile(courseFileName);
   QDomDocument doc("mydocument");
   if (!doc.setContent(&docFile)) {
      trace(QString("Cannot open file: ")+courseFileName, traceError);
      return;
   }

   QDomElement rootElement = doc.documentElement();
   QString topicNameA = m_courseTemplate.options.subname;
   QString topicNameB = m_courseTemplate.options.subname+"*";
   int voiceIndexA = getVoiceEngineIndex(m_courseTemplate.options.voiceNameA)+1;
   int voiceIndexQ = getVoiceEngineIndex(m_courseTemplate.options.voiceNameQ)+1;

   int topicAID, topicBID;
   // A course
   if (!m_db.addItem(topicNameA, courseId, 0, &topicAID))
      return;

   QDomNode topicNodeA = getNode (rootElement, topicNameA, doc, courseFileDirectoryName, "pres", topicAID);
   setDelete (topicNodeA);

   for (int i = 0;i<m_courseTemplate.content.count();++i) {
      QString line = (m_courseTemplate.content.at(i)).trimmed();
      if (line.length() == 0) continue;
      QStringList list1 = line.split(":");

      if (list1.count() < 2) {
         trace(QString("cCourseGenerator::generate Input error: ")+line, traceWarning);
         continue;
      }

      emit progressSignal(QString::number(i+1)+"/"+QString::number(m_courseTemplate.content.count())+" "+topicNameA+"@"+m_courseTemplate.options.courseName+":"
                          +list1.at(0));

      if (m_abortProces)
         return;

      generateCourseElement(courseId, list1.at(0), list1.at(1), topicNameA, topicNodeA, topicAID, doc, courseFileDirectoryName, false, voiceIndexA, voiceIndexQ, m_rebuild);
   }


   doDelete (courseId, topicAID, topicNodeA, courseFileDirectoryName);


   // B course
   if (m_courseTemplate.options.bothDirections) {
      if (!m_db.addItem(topicNameB, courseId, 0, &topicBID))
         return;
      QDomNode topicNodeB = getNode (rootElement, topicNameB, doc, courseFileDirectoryName, "pres", topicBID);

      setDelete (topicNodeB);

      for (int i = 0; i<m_courseTemplate.content.count(); ++i) {
         QString line = (m_courseTemplate.content.at(i)).trimmed();
         if (line.length() == 0) continue;
         QStringList list1 = line.split(":");

         if (list1.count()<2) {
            trace(QString("cCourseGenerator::generate Input error: ")+line, traceWarning);
            continue;
         }

         emit progressSignal(QString::number(i+1)+"/"+QString::number(m_courseTemplate.content.count())+" "+topicNameB+"@"+m_courseTemplate.options.courseName+":"
                             +list1.at(0));

         if (m_abortProces)
            return;

         generateCourseElement(courseId, list1.at(1), list1.at(0), topicNameB, topicNodeB, topicBID, doc, courseFileDirectoryName, true, voiceIndexA, voiceIndexQ, m_rebuild);
      }

      doDelete (courseId, topicBID, topicNodeB, courseFileDirectoryName);
   }


   writeDomDoucumentToFile(doc, courseFileName);
   m_isFailed = false;
}

/////////////////////////////////////////////////////////////////////////////
QDomNode CourseGenerator::getNode (QDomNode &rootElement, QString nodeName, QDomDocument &doc, QString courseFileDirectory, QString type, int nodeID)
{
   QDomNode node = rootElement.firstChild();
   while(!node.isNull()) {
      QDomElement e = node.toElement(); // try to convert the node to an element.
      if(!e.isNull()) {
         if ((e.attribute("type", "none") == type) &&
             (e.attribute("name", "none") == nodeName) &&
             (e.attribute("id", "0").toInt() == nodeID)) {
            e.setAttribute("delete", "false");
            break; // topic Found
         }
      }
      node = node.nextSibling();
   }

   if (node.isNull()) {
      QDomElement tmpElement = doc.createElement( "element" );
      tmpElement.setAttribute("name", nodeName);
      tmpElement.setAttribute("type", type);
      tmpElement.setAttribute("id", QString::number(nodeID));
      tmpElement.setAttribute("delete", "false");
      node = rootElement.appendChild(tmpElement);
      QDomDocument docItem = createCourseItem(1, nodeName);
      writeDomDoucumentToFile(docItem, courseFileDirectory+getFileName(nodeID));
      trace(QString("getNode createNewNode Name: ")+nodeName+QString(" ID:")+QString::number(nodeID), traceLevel2);
   }

   return node;
}

/////////////////////////////////////////////////////////////////////////////
void CourseGenerator::setDelete (QDomNode &topicNode)
{
   QDomNode n = topicNode.firstChild();
   while(!n.isNull()) {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull()) {
         e.setAttribute("delete", "true");
      }
      n = n.nextSibling();
   }
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

   QDomNode questionNode = getNode (topicNode, getTextToPrint(question), doc, courseFileDirectory, "exercise", ID);

   // create xml course file
   if (foreceRebuild ||
       checkIfNewAnswers(courseFileDirectory+getFileName(ID), answer)) {
      forceMedia = true;
      QDomDocument docItem = createCourseItem(1, topicName, m_courseTemplate.options.instruction, getTextToPrint(question), getTextToPrint(answer), ID, bMode);
      writeDomDoucumentToFile(docItem, courseFileDirectory+getFileName(ID));
   }

   // create mp3
   QString mp3Q = bMode ? "a" : "q";
   if (m_courseTemplate.options.voiceQ&&
       ((forceMedia)||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3Q+".mp3")))) {
      QStringList arguments; // filename, text, trim, gain
      arguments.append(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3Q);
      arguments.append(bMode ? getTranscript(QString(answer).replace("|", " ")):getTranscript(QString(question).replace("|", " ")));
      arguments.append(QString::number(voiceIndexQ));
      arguments.append(QString::number(m_courseTemplate.options.voiceTrimQ));
      arguments.append(QString::number(m_courseTemplate.options.voiceGainQ));

      trace(QString("createMp3.bat ")+arguments.join(" "), traceLevel1);

      myProcess.start("createMp3.bat", arguments );
      if (!myProcess.waitForStarted())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
      myProcess.waitForFinished(timeOut);
      if (myProcess.exitCode())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
   }

   QString mp3A = bMode ? "q" : "a";
   if (m_courseTemplate.options.voiceA&&
       ((forceMedia)||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3A+".mp3")))) {
      QStringList arguments; // filename, text, trim, gain
      arguments.append(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3A);
      arguments.append(!bMode ? getTranscript(QString(answer).replace("|", " ")):getTranscript(QString(question).replace("|", " ")));
      arguments.append(QString::number(voiceIndexA));
      arguments.append(QString::number(m_courseTemplate.options.voiceTrimA));
      arguments.append(QString::number(m_courseTemplate.options.voiceGainA));

      trace(QString("createMp3.bat ")+arguments.join(" "), traceLevel1);

      myProcess.start("createMp3.bat", arguments );
      if (!myProcess.waitForStarted())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
      myProcess.waitForFinished(timeOut);
      if (myProcess.exitCode())
         trace(QString("Error:createMp3.bat ")+arguments.join(" "), traceError);
   }

   // create jpg
   if (m_courseTemplate.options.graphics&&
       ((forceMedia)||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"m.jpg"))||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"n.jpg"))))
   {
      deleteFile(TMPDIR+"HTML");
      QStringList arguments;

      arguments.append(getKeyWord(question));
      arguments.append(TMPDIR+"HTML");

      myProcess.start("getGoogleHtml.bat", arguments );
      if (!myProcess.waitForStarted())
         trace(QString("Error:getGoogleHtml.bat ")+arguments.join(" "), traceError);
      myProcess.waitForFinished(timeOut);
      if (myProcess.exitCode())
         trace(QString("Error:getGoogleHtml.bat ")+arguments.join(" "), traceError);

      QStringList fileUrls = parseGoogleHtml(TMPDIR+"HTML");

      int i = 0; // download 2 images
      while ((fileUrls.count())>0&&(i<2))
      {
         QString fileName = courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+('m'+i)+".jpg";
         //QString filrExt = ;
         deleteFile(fileName);
         arguments.clear();
         arguments.append(fileUrls.first()+QString(" "));
         arguments.append(fileName+" ");

         trace(QString("getImage.bat ")+arguments.join(" "), traceLevel1);

         myProcess.start("getImage.bat", arguments );
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

   while(!n.isNull())
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull())
      {
         if (e.attribute("delete", "none") == "true")
         {
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
int CourseGenerator::writeDomDoucumentToFile (QDomDocument &document, QString path)
{
   QFile file( path );
   if( !file.open( QIODevice::WriteOnly|QIODevice::Text ) )
      return -1;
   QTextStream ts( &file );
   ts.setCodec("UTF-8");
   ts <<document.toString();
   file.close();
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
QDomDocument CourseGenerator::createCourseItem (int templateId, QString chapter)
{
   QDomDocument doc;
   QDomElement rootElement = doc.createElement( "item" );
   rootElement.setAttribute("xmlns", "http://www.supermemo.net/2006/smux");
   doc.appendChild( rootElement );

   QDomElement tmpElement0 = (doc.createElement( "chapter-title" ));
   tmpElement0.appendChild(doc.createTextNode(chapter));
   rootElement.appendChild(tmpElement0);

   QDomElement tmpElement1 = (doc.createElement( "modified" ));
   QDate x = QDate::currentDate();
   tmpElement1.appendChild(doc.createTextNode(x.toString("yyyy-MM-dd")));
   rootElement.appendChild(tmpElement1);

   QDomElement tmpElement2 = doc.createElement( "template-id" );
   tmpElement2.appendChild(doc.createTextNode(QString::number(templateId)));
   rootElement.appendChild(tmpElement2);

   return doc;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::checkIfNewAnswers(const QString &fileName, const QString &answers)
{
   QDomDocument doc("");
   QFile docFile(fileName);
   if (!doc.setContent(&docFile)) {
      docFile.close();
      trace(QString("cCourseGenerator::checkIfNewAnswers error:")+fileName, traceError);
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
   if(!e.isNull()) {
      QString oAnswer = e.attribute("correct", "");
      if (oAnswer == answers)
         return false;

      trace(QString("New answers: ")+oAnswer+":"+answers, traceLevel1);
   }
   return true;
}

/////////////////////////////////////////////////////////////////////////////
QDomDocument CourseGenerator::createCourseItem (int templateId, QString chapter, QString title, QString question, QString answers, int ID, bool bMode)
{

   QDomDocument doc = createCourseItem(templateId, chapter);
   QDomElement rootElement = doc.documentElement();

   QDomElement tmpElement0 = doc.createElement( "lesson-title" );
   tmpElement0.appendChild(doc.createTextNode(QString::number(ID, 10)));
   rootElement.appendChild(tmpElement0);

   QDomElement tmpElement1 = doc.createElement( "question-title" );
   tmpElement1.appendChild(doc.createTextNode(title));
   rootElement.appendChild(tmpElement1);

   QDomElement tmpElement2 = doc.createElement( "question" );
   tmpElement2.appendChild(doc.createTextNode(question+" - "));
   QDomElement tmpElement3 = doc.createElement( "spellpad" );
   tmpElement3.setAttribute("correct", answers);
   tmpElement2.appendChild(tmpElement3);

   if (m_courseTemplate.options.graphics)
   { // create table with images
      tmpElement2.appendChild(doc.createElement("br"));
      tmpElement2.appendChild(doc.createElement("br"));

      QDomElement table = doc.createElement( "table" );
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
      gfx.setAttribute("auto-play" , "false");

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
      gfx.setAttribute("auto-play" , "false");
      td.appendChild(gfx);

      tr.appendChild(td);
      table.appendChild(tr);
      tmpElement2.appendChild(table);

   }


   rootElement.appendChild(tmpElement2);

   if (m_courseTemplate.options.voiceA)
   {
      QDomElement tmpElement4 = doc.createElement( bMode ? "question-audio" : "answer-audio" );
      tmpElement4.appendChild(doc.createTextNode("true"));
      rootElement.appendChild(tmpElement4);
   }

   if (m_courseTemplate.options.voiceQ)
   {
      QDomElement tmpElement4 = doc.createElement( bMode ? "answer-audio" : "question-audio");
      tmpElement4.appendChild(doc.createTextNode("true"));
      rootElement.appendChild(tmpElement4);
   }
   return doc;
}

/////////////////////////////////////////////////////////////////////////////
bool CourseGenerator::isFailed()
{
   return m_isFailed;
}
