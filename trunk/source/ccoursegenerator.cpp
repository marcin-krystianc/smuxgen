//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================


#include "ccoursegenerator.h"

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
#include <list>

#include "globalsmuxgentools.h"



/////////////////////////////////////////////////////////////////////////////
cCourseGenerator::cCourseGenerator()
{
    this->abortProces = false;
}

/////////////////////////////////////////////////////////////////////////////
cCourseGenerator::~cCourseGenerator()
{
    mutex.lock();
    this->abortProces = true;
    mutex.unlock();

    wait();
}

/////////////////////////////////////////////////////////////////////////////
void cCourseGenerator::stop()
{
    mutex.lock();
    this->abortProces = true;
    mutex.unlock();
}

/////////////////////////////////////////////////////////////////////////////
void cCourseGenerator::trace (const QString &text,const int & flags)
{
      globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
void cCourseGenerator::generate (const cCourseTemplate &courseTemplate)
{
    this->courseTemplate    = courseTemplate;
    this->abortProces       = false;
    this->start();
}

/////////////////////////////////////////////////////////////////////////////
void  cCourseGenerator::run ()
{

    this->status  = 1;
    if (!this->database.open(this->courseTemplate.options.database))
        return;

    int courseID;
    QString courseFileName,courseFileDirectoryName;

    if (!this->database.getCourseIdPath (this->courseTemplate.options.course, courseID, courseFileName))
        return;

    QFileInfo courseFileInfo(courseFileName);
    courseFileDirectoryName = QDir::toNativeSeparators(courseFileInfo.dir().path())+QDir::separator();

    // check media directory
    QDir *pDir= new QDir(courseFileDirectoryName);
    if (!pDir->exists("media"))
       pDir->mkdir("media");
    delete(pDir);

    // get root document
    QDomDocument doc("mydocument");
    QFile docFile(courseFileName);

    if (!doc.setContent(&docFile)) {
        docFile.close();
        trace(QString("Cannot open file: ")+courseFileName,traceError);
        return;
    }
    docFile.close();


    QDomElement rootElement = doc.documentElement();
    if (!this->database.getCourseMaxId(courseID))
    {
        trace(QString("Cannot getCourseMaxId"),traceError);
        return;
    }

    QString topicNameA = this->courseTemplate.options.subname;
    QString topicNameB = this->courseTemplate.options.subname+"*";

    QDomNode topicNodeA;
    QDomNode topicNodeB;

    int topicAID,topicBID;
    // A course
    if (!database.setElementSQL(topicNameA,courseID,0,topicAID))
        goto END;
    topicNodeA = getNode (rootElement,topicNameA,doc,courseFileDirectoryName,"pres",topicAID);

    setDelete (topicNodeA);

    for (int i=0;i<this->courseTemplate.content.count();++i)
    {
        QString line=(this->courseTemplate.content.at(i)).trimmed();
        if (line.length()==0) continue;
        QStringList list1 = line.split(":");

        if (list1.count()<2)
        {
            trace(QString("cCourseGenerator::generate Input error: ")+line,traceWarning);
            continue;
        }

        emit progressSignal(topicNameA+"@"+this->courseTemplate.options.course+":"
                                  +list1.at(0)+" "+QString::number(i+1)+"/"+QString::number(this->courseTemplate.content.count()));

        if (this->abortProces)
         goto END;

        generateCourseElement(courseID,(list1.at(0)).trimmed(),(list1.at(1)).trimmed(),topicNameA,topicNodeA,topicAID,doc,courseFileDirectoryName,false);
    }


    doDelete (courseID,topicAID,topicNodeA,courseFileDirectoryName);


    // B course
    if (this->courseTemplate.options.bit.oDouble)
    {
        if (!database.setElementSQL(topicNameB,courseID,0,topicBID))
            goto END;
        topicNodeB = getNode (rootElement,topicNameB,doc,courseFileDirectoryName,"pres",topicBID);

        setDelete (topicNodeB);

        for (int i=0;i<this->courseTemplate.content.count();++i)
        {
            QString line=(this->courseTemplate.content.at(i)).trimmed();
            if (line.length()==0) continue;
            QStringList list1 = line.split(":");

            if (list1.count()<2)
            {
                trace(QString("cCourseGenerator::generate Input error: ")+line,traceWarning);
                continue;
            }

               emit progressSignal(topicNameB+"@"+this->courseTemplate.options.course+":"
                                  +list1.at(0)+" "+QString::number(i+1)+"/"+QString::number(this->courseTemplate.content.count()));

            if (this->abortProces)
                goto END;

            generateCourseElement(courseID,(list1.at(1)).trimmed(),(list1.at(0)).trimmed(),topicNameB,topicNodeB,topicBID,doc,courseFileDirectoryName,true);
         }

        doDelete (courseID,topicBID,topicNodeB,courseFileDirectoryName);
    }


END:
    writeDomDoucumentToFile(doc,courseFileName);
    this->status = 0;
}

/////////////////////////////////////////////////////////////////////////////
QDomNode cCourseGenerator::getNode (QDomNode &rootElement,QString nodeName,QDomDocument &doc,QString courseFileDirectory,QString type,int nodeID)
{
    QDomNode node = rootElement.firstChild();
    while(!node.isNull())
    {
        QDomElement e = node.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            if ((e.attribute("type","none")==type)      &&
                (e.attribute("name","none")==nodeName)  &&
                (e.attribute("id","0").toInt()==nodeID))
            {
                e.setAttribute("delete","false");
                break; // topic Found
            }
        }
        node = node.nextSibling();
    }

    if (node.isNull())
    {
        QDomElement tmpElement = doc.createElement( "element" );
        tmpElement.setAttribute("name",nodeName);
        tmpElement.setAttribute("type",type);
        tmpElement.setAttribute("id",QString::number(nodeID));
        tmpElement.setAttribute("delete","false");
        node = rootElement.appendChild(tmpElement);
        QDomDocument docItem=createCourseItem(1,nodeName);
        writeDomDoucumentToFile(docItem,courseFileDirectory+getFileName(nodeID));
        trace(QString("getNode createNewNode Name: ")+nodeName+QString(" ID:")+nodeID,traceLevel2);
    }

    return node;
}

/////////////////////////////////////////////////////////////////////////////
void  cCourseGenerator::setDelete (QDomNode &topicNode)
{
    QDomNode n = topicNode.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            e.setAttribute("delete","true");
        }
        n = n.nextSibling();
    }
}

/////////////////////////////////////////////////////////////////////////////
bool cCourseGenerator::generateCourseElement(int courseIDSQL,QString question,QString answer,QString topicName,QDomNode &topicNode,int topicID,QDomDocument &doc,QString courseFileDirectory,bool bMode)
{
    int     ID          = 0;
    bool    forceMedia  = this->courseTemplate.options.bit.oForce;
    const int timeOut   = -1; // no timeout
    QProcess myProcess;

    if (!this->database.setElementSQL(question,courseIDSQL,topicID,ID))
        return false;

    QDomNode questionNode=getNode (topicNode,question,doc,courseFileDirectory,"exercise",ID);

    // create xml course file
    if ((this->courseTemplate.options.bit.oForce)||
    (checkIfNewAnswers(courseFileDirectory+getFileName(ID),answer)))
    {
        forceMedia=true;
        QDomDocument docItem=createCourseItem(1,topicName,this->courseTemplate.options.instruction,question,answer,ID,bMode);
        writeDomDoucumentToFile(docItem,courseFileDirectory+getFileName(ID));
    }

    // create mp3
    QString mp3Q = bMode ? "a" : "q";
    if (this->courseTemplate.options.bit.oVoiceQ&&
     ((forceMedia)||
    (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3Q+".mp3"))))
    {
        QStringList  arguments; // filename, text,trim,gain
        arguments.append(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3Q);
        answer.replace("|"," ");
        question.replace("|"," ");
        arguments.append(bMode ? answer:question);
        arguments.append(QString::number(this->courseTemplate.options.voiceIndexQ));
        arguments.append(QString::number(this->courseTemplate.options.voiceTrimQ));
        arguments.append(QString::number(this->courseTemplate.options.voiceGainQ));

        trace(QString("createMp3.bat ")+arguments.join(" "),traceLevel1);

        myProcess.start("createMp3.bat", arguments );
        if (!myProcess.waitForStarted())
             trace(QString("Error:createMp3.bat ")+arguments.join(" "),traceError);
        myProcess.waitForFinished(timeOut);
        if (myProcess.exitCode())
            trace(QString("Error:createMp3.bat ")+arguments.join(" "),traceError);
    }

    QString mp3A = bMode ? "q" : "a";
    if (this->courseTemplate.options.bit.oVoiceA&&
     ((forceMedia)||
    (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3A+".mp3"))))
    {
        QStringList  arguments; // filename, text,trim,gain
        arguments.append(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3A);
        answer.replace("|"," ");
        question.replace("|"," ");
        arguments.append(bMode ? question:answer);
        arguments.append(QString::number(this->courseTemplate.options.voiceIndexA));
        arguments.append(QString::number(this->courseTemplate.options.voiceTrimA));
        arguments.append(QString::number(this->courseTemplate.options.voiceGainA));

        trace(QString("createMp3.bat ")+arguments.join(" "),traceLevel1);

        myProcess.start("createMp3.bat", arguments );
        if (!myProcess.waitForStarted())
             trace(QString("Error:createMp3.bat ")+arguments.join(" "),traceError);
        myProcess.waitForFinished(timeOut);
        if (myProcess.exitCode())
            trace(QString("Error:createMp3.bat ")+arguments.join(" "),traceError);
    }

    // create jpg
    if (this->courseTemplate.options.bit.oImage&&
        ((forceMedia)||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"m.jpg"))||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"n.jpg"))))
    {
        deleteFile(tmpDir+"HTML");
        QStringList  arguments;

        arguments.append(getKeyWord(question));
        arguments.append(tmpDir+"HTML");

        myProcess.start("getGoogleHtml.bat", arguments  );
        if (!myProcess.waitForStarted())
            trace(QString("Error:getGoogleHtml.bat ")+arguments.join(" "),traceError);
        myProcess.waitForFinished(timeOut);
        if (myProcess.exitCode())
            trace(QString("Error:getGoogleHtml.bat ")+arguments.join(" "),traceError);

        QStringList  fileUrls= parseGoogleHtml(tmpDir+"HTML");

        int i=0; // download 2 images
        while ((fileUrls.count())>0&&(i<2))
        {
            QString fileName = courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+('m'+i)+".jpg";
            //QString filrExt = ;
            deleteFile(fileName);
            arguments.clear();
            arguments.append(fileUrls.first()+QString(" "));
            arguments.append(fileName+" ");

            trace(QString("getImage.bat ")+arguments.join(" "),traceLevel1);

            myProcess.start("getImage.bat", arguments  );
            if (!myProcess.waitForStarted())
                trace(QString("Error:getImage.bat ")+arguments.join(" "),traceError);
            myProcess.waitForFinished(timeOut);
            if (myProcess.exitCode())
                  trace(QString("Error:getImage.bat ")+arguments.join(" "),traceError);

            if (!scalePicture(fileName,IMG_WIDTH,IMG_HEIGHT))
            {
                trace(QString("Error:scalePicture ")+fileName,traceError);
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
bool  cCourseGenerator::doDelete (int courseIDSQL,int paretntIDSQL,QDomNode &docElement,QString courseFileDirectory)
{
    QDomNode n = docElement.firstChild();
    std::list <int> listValidID;
    std::list <int>::iterator it;

    trace (QString("doDelete: id:")
           + QString(" CourseId:")+QString::number(courseIDSQL)
            + QString(" ParetntId:")+QString::number(paretntIDSQL)
            ,traceLevel2);

    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            if (e.attribute("delete","none")=="true")
            {
                trace(QString("doDelete: id:")+e.attribute("id","0")+QString(" name:")+e.attribute("name",""),traceLevel2);
                int ID = e.attribute("id","0").toInt();
                deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"a.mp3");
                deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"q.mp3");
                deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"m.jpg");
                deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"n.jpg");

                QDomNode tmp = n;
                n = n.nextSibling();
                docElement.removeChild(tmp);
                continue;
            }
            listValidID.push_back(e.attribute("id","0").toInt());
            trace(QString("validID:")+e.attribute("id","0"),traceLevel2);
        }
        n = n.nextSibling();
    }

    QString   filter;   // CourseID + PageNum - primary key
    filter  +=QString::fromUtf8("delete from items where ");
    filter  +=QString::fromUtf8("CourseId=")+QString::number(courseIDSQL);
    filter  +=QString::fromUtf8(" and ");
    filter  +=QString::fromUtf8("ParentID=")+QString::number(paretntIDSQL);
    filter  +=QString::fromUtf8(" and ");
    for (it=listValidID.begin();it!=listValidID.end();it++)
        filter +=QString::fromUtf8 ("PageNum!=")+QString::number(*it)+QString(" and ");

    filter +="1=1";    // to finish qery

    QSqlQuery query (this->database.getDatabase());
    if (!query.exec(filter))    // delete all unknown course items
    {
        trace(QString("doDelete error query.exec(): ")+query.lastError().text(),traceError);
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
int cCourseGenerator::writeDomDoucumentToFile (QDomDocument &document,QString path)
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
QDomDocument cCourseGenerator::createCourseItem (int templateId,QString chapter)
{
    QDomDocument doc;
    QDomElement rootElement = doc.createElement( "item" );
    rootElement.setAttribute("xmlns","http://www.supermemo.net/2006/smux");
    doc.appendChild( rootElement );

    QDomElement tmpElement0 = (doc.createElement( "chapter-title" ));
    tmpElement0.appendChild(doc.createTextNode(chapter));
    rootElement.appendChild(tmpElement0);

    QDomElement tmpElement1 = (doc.createElement( "modified" ));
    QDate x=QDate::currentDate();
    tmpElement1.appendChild(doc.createTextNode(x.toString("yyyy-MM-dd")));
    rootElement.appendChild(tmpElement1);

    QDomElement tmpElement2 = doc.createElement( "template-id" );
    tmpElement2.appendChild(doc.createTextNode(QString::number(templateId)));
    rootElement.appendChild(tmpElement2);

    return doc;
}

/////////////////////////////////////////////////////////////////////////////
bool cCourseGenerator::checkIfNewAnswers(QString fileName,QString answers)
{
    QDomDocument doc("mydocument");
    QFile docFile(fileName);

    if (!doc.setContent(&docFile)) {
        docFile.close();
        trace(QString("cCourseGenerator::checkIfNewAnswers error:")+fileName,traceError);
        return 1;
    }
    docFile.close();

    QDomElement rootElement = doc.documentElement();
    QDomNode n = rootElement.firstChildElement("question");
    if (n.isNull()) return 1;
    QDomNode n1 = n.firstChildElement("spellpad");
    if (n1.isNull()) return 1;

    QDomElement e = n1.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
        QString oAnswer = e.attribute("correct","");
        if (oAnswer==answers)
            return 0;

         trace(QString("New answers: ")+oAnswer+":"+answers,traceLevel1);
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////////////
QDomDocument cCourseGenerator::createCourseItem (int templateId,QString chapter,QString title,QString question,QString answers,int ID,bool bMode)
{

    QDomDocument doc = createCourseItem(templateId,chapter);
    QDomElement rootElement = doc.documentElement();

    QDomElement tmpElement0 = doc.createElement( "lesson-title" );
    tmpElement0.appendChild(doc.createTextNode(QString::number(ID,10)));
    rootElement.appendChild(tmpElement0);

    QDomElement tmpElement1 = doc.createElement( "question-title" );
    tmpElement1.appendChild(doc.createTextNode(title));
    rootElement.appendChild(tmpElement1);

    QDomElement tmpElement2 = doc.createElement( "question" );
    tmpElement2.appendChild(doc.createTextNode(question+" - "));
    QDomElement tmpElement3 = doc.createElement( "spellpad" );
    tmpElement3.setAttribute("correct",answers);
    tmpElement2.appendChild(tmpElement3);

    if (this->courseTemplate.options.bit.oImage)
    {   // create table with images
        tmpElement2.appendChild(doc.createElement("br"));
        tmpElement2.appendChild(doc.createElement("br"));
        tmpElement2.appendChild(doc.createElement("br"));

        QDomElement table= doc.createElement( "table" );
        table.setAttribute("width","100%");

            QDomElement tr = doc.createElement("tr");

                QDomElement td= doc.createElement("td");
                td.setAttribute("align","center");

                    QDomElement gfx= doc.createElement("gfx");
                    gfx.setAttribute("file","m");
                    gfx.setAttribute("space","0");
                    gfx.setAttribute("width"    ,QString::number(IMG_WIDTH));
                    gfx.setAttribute("height"   ,QString::number(IMG_HEIGHT));
                td.appendChild(gfx);

            tr.appendChild(td);

                td= doc.createElement("td");
                td.setAttribute("align","center");

                    gfx= doc.createElement("gfx");
                    gfx.setAttribute("file","n");
                    gfx.setAttribute("space","0");
                    gfx.setAttribute("width"    ,QString::number(IMG_WIDTH));
                    gfx.setAttribute("height"   ,QString::number(IMG_HEIGHT));
                td.appendChild(gfx);

            tr.appendChild(td);
        table.appendChild(tr);
    tmpElement2.appendChild(table);

    }


    rootElement.appendChild(tmpElement2);

    if (this->courseTemplate.options.bit.oVoiceA)
    {
        QDomElement tmpElement4 = doc.createElement( bMode ? "question-audio" : "answer-audio" );
        tmpElement4.appendChild(doc.createTextNode("true"));
        rootElement.appendChild(tmpElement4);
    }

    if (this->courseTemplate.options.bit.oVoiceQ)
    {
        QDomElement tmpElement4 = doc.createElement( bMode ?  "answer-audio" : "question-audio");
        tmpElement4.appendChild(doc.createTextNode("true"));
        rootElement.appendChild(tmpElement4);
    }
    return doc;
}

/////////////////////////////////////////////////////////////////////////////
int cCourseGenerator::getStatus()
{
    return this->status;
}
