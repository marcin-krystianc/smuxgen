
//-----------------------------------------------------
//	Author	:	Marcin Krystianc
//	Date	:	11/11/2009
//	Contact	:	marcin.krystianc@gmail.com
//------------------------------------------------------

/*
    This file is part of smuxgen.

    Smuxgen is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Smuxgen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Anncu.  If not, see <http://www.gnu.org/licenses/>.


 */


#include <QtCore/QCoreApplication>

#include <qdom.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDate>
#include <QTextCodec>
#include <QProcess>

#include <iostream>
#include <list>
#include <algorithm>
#include <utility>

using namespace std;


typedef union
{
    unsigned int value;
    struct
    {
        unsigned int oForce  : 1; // force to create agian all course elements
        unsigned int oDouble : 1; // make also alternative course
        unsigned int oVoice  : 1; // make mp3 files
        unsigned int oImage  : 1; // get Images
        unsigned int oClean  : 1; // oClean
    } bit;
} tOPTIONS;

/////////////////////////////////////////////////////////////////////////////
int  getGID (QDomElement &docElement)
{
    int retID=0;
    QDomNode n = docElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            if (e.hasAttribute("id"))
             {
                QString id = e.attribute("id","0");
                retID = max (retID,id.toInt());
            }
            retID = max(retID,getGID (e)); // requrence
        }
        n = n.nextSibling();
    }
    return retID;
}

/////////////////////////////////////////////////////////////////////////////
int writeDomDoucumentToFile (QDomDocument &document,QString path)
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
QDomDocument createCourseItem (int templateId,QString chapter)
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
QDomDocument createCourseItem (int templateId,QString chapter,QString title,QString question,QString answers,int ID,tOPTIONS options,QString audioType)
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

    if (options.bit.oImage)
    {   // create table with images
        tmpElement2.appendChild(doc.createElement("br"));
        tmpElement2.appendChild(doc.createElement("br"));
        tmpElement2.appendChild(doc.createElement("br"));

        QDomElement table= doc.createElement( "table" );
        table.setAttribute("width","100%");

            QDomElement tbody= doc.createElement( "tbody" );
                QDomElement tr = doc.createElement("tr");

                    QDomElement td= doc.createElement("td");
                    td.setAttribute("align","center");

                        QDomElement gfx= doc.createElement("gfx");
                        gfx.setAttribute("file","m");
                        gfx.setAttribute("space","10");
                        gfx.setAttribute("width","333");
                        gfx.setAttribute("height","333");
                    td.appendChild(gfx);

                tr.appendChild(td);

                    td= doc.createElement("td");
                    td.setAttribute("align","center");

                        gfx= doc.createElement("gfx");
                        gfx.setAttribute("file","n");
                        gfx.setAttribute("space","10");
                        gfx.setAttribute("width","333");
                        gfx.setAttribute("height","333");
                    td.appendChild(gfx);

                tr.appendChild(td);
            tbody.appendChild(tr);
        table.appendChild(tbody);
    tmpElement2.appendChild(table);

    }


    rootElement.appendChild(tmpElement2);

    if (options.bit.oVoice)
    {
        QDomElement tmpElement4 = doc.createElement( audioType );
        tmpElement4.appendChild(doc.createTextNode("true"));
        rootElement.appendChild(tmpElement4);
    }

    return doc;
}

/////////////////////////////////////////////////////////////////////////////
QString getFileName (int i)
{
    QString name;
    name.fill('0',5);
    name+=QString::number(i);
    name+=".xml";
    name=name.right(9);
    return QString("item")+name;
}

/////////////////////////////////////////////////////////////////////////////
QString getMediaFileName (int i)
{
    QString name;
    name.fill('0',5);
    name+=QString::number(i);
    name=name.right(5);
    return name;
}

/////////////////////////////////////////////////////////////////////////////
void deleteFile (QString fileName)
{
    QFile fileObject (fileName);
    fileObject.remove();
}

/////////////////////////////////////////////////////////////////////////////
bool checkIsFileOk(QString fileName)
{
    QFile fileObject (fileName);
    //fileObject.open(QIODevice::ReadOnly);
    if (fileObject.size()>0)
        return true;
    else
    {
        cout<< "NOT OK: "<<qPrintable(fileName)<<endl;
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////
bool checkIfNewAnswers(QString fileName,QString answers)
{
    QDomDocument doc("mydocument");
    QFile docFile(fileName);

    if (!doc.setContent(&docFile)) {
        docFile.close();
        cout<<"New answers: "<<"fileName:"<<qPrintable(fileName)<<" answers:"<<qPrintable(answers)<<endl;
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

         cout<<"New answers: "<<qPrintable(oAnswer)<<":"<<qPrintable(answers)<<endl;

    }

    return 1;
}

/////////////////////////////////////////////////////////////////////////////
QStringList parseGoogleHtml (QString fileName)
{
    QFile inputFile;
    QStringList retList;
    QTextStream inputFileStream;
    QString html;
    inputFile.setFileName(fileName);
    if (!inputFile.open(QIODevice::ReadOnly))
     {
         cout << " Nie mo¿na otworzyæ pliku: "<<qPrintable(fileName)<<endl;
         return retList;
     }

    inputFileStream.setDevice( &inputFile );
    inputFileStream.setCodec("UTF-8");
    html=inputFileStream.readAll();

    int pos=0,leftPos,rightPos;

    QString  leftBound  ("/imgres?imgurl\\x3d");
    QString  rightBound ("\\x26");
    QString  tmp;

    while ((leftPos=html.indexOf(leftBound,pos)) != -1)
    {
     rightPos=html.indexOf(rightBound,leftPos);
     if (rightPos==-1)
         break;

     pos=rightPos;
     tmp = html.mid(leftPos+leftBound.length(),rightPos-leftPos-leftBound.length());
     if (tmp.indexOf("%")!=-1)
         continue;
     retList.append(tmp);
    }
    return retList;
}

/////////////////////////////////////////////////////////////////////////////
QDomNode getNode (QDomNode &rootElement,QString nodeName,QDomDocument &doc,QString courseFileDirectory,QString type,int &retID,int &GID)
{
    QDomNode node = rootElement.firstChild();
    while(!node.isNull())
    {
        QDomElement e = node.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            if ((e.attribute("type","none")==type)&&(e.attribute("name","none")==nodeName))
            {
                retID=(e.attribute("id","0")).toInt();
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
        tmpElement.setAttribute("id",QString::number(retID=++GID));
        tmpElement.setAttribute("delete","false");
        node = rootElement.appendChild(tmpElement);
        QDomDocument docItem=createCourseItem(1,nodeName);
        writeDomDoucumentToFile(docItem,courseFileDirectory+getFileName(retID));
        cout<<"getNode createNewNode Name:"<<qPrintable(nodeName)<<" ID:"<<retID<<endl;
    }

    return node;
}

/////////////////////////////////////////////////////////////////////////////
QDomNode getNode (QDomNode &rootElement,QString nodeName,QDomDocument &doc,QString courseFileDirectory,QString type,int &GID)
{
    int tmp;
    return getNode (rootElement,nodeName,doc,courseFileDirectory,type,tmp,GID);
}

/////////////////////////////////////////////////////////////////////////////
QString getKeyWord (QString iString)
{
    static QString A=QString::fromUtf8("¥Æ¯ŒÑÓ£Ê¹æ¿ŸœBó³ê!@#$%^&*()_-=+,./<>?;':\"[]\{}|");
    static QString B=QString::fromUtf8("ACZXSNOLEaczzsnole                               ");

    for (int i=A.length()-1;i>=0;--i)
        iString.replace(A.at(i),B.at(i));

    QStringList tmp=iString.split(" ",QString::SkipEmptyParts);

    if (tmp.count() > 1)
        return (tmp.at(0)+QString(" ")+tmp.at(1));
    else
        return tmp.at(0);
}

/////////////////////////////////////////////////////////////////////////////
void generateCourseElement(QString question,QString answer,QString topicName,QDomNode &topicNode,QDomDocument &doc,QString courseFileDirectory,tOPTIONS options,bool bMode,int &GID)
{
    int     ID          = 0;
    bool    forceMedia  = options.bit.oForce;

    QDomNode questionNode=getNode (topicNode,question,doc,courseFileDirectory,"exercise",ID,GID);

    // create xml course file
    if ((options.bit.oForce)||
    (checkIfNewAnswers(courseFileDirectory+getFileName(ID),answer)))
    {
        forceMedia=true;

        QString audio = bMode ? "question-audio" : "answer-audio";
        QDomDocument docItem=createCourseItem(1,topicName,QString::fromUtf8("Przet³umacz"),question,answer,ID,options,audio);

        writeDomDoucumentToFile(docItem,courseFileDirectory+getFileName(ID));

        { // check media directory
           QDir *pDir= new QDir(courseFileDirectory);
           if (!pDir->exists("media"))
               pDir->mkdir("media");
        }
    }

    // create mp3
    QString mp3 = bMode ? "q" : "a";
    if (options.bit.oVoice&&
     ((forceMedia)||
    (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3+".mp3"))))
    {
     QStringList  arguments;
     arguments.append(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+mp3);
     answer.replace("|"," ");
     arguments.append(bMode ? question:answer);
     if (QProcess::execute ( "createMp3.bat", arguments  ))
     {
        cout<<"Error:createMp3.bat "<<qPrintable(arguments.at(0))<<" "<<qPrintable(arguments.at(1))<<endl;
        if (bMode)
        deleteFile(courseFileDirectory+QDir::separator()+getMediaFileName(ID)+mp3+".mp3");
     }
    }

    // create jpg
    if (options.bit.oImage&&
        ((forceMedia)||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"m.jpg"))||
        (!checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+"n.jpg"))))
    {
        deleteFile("HTML");
        QStringList  arguments;
        arguments.clear();

        arguments.append(getKeyWord(question));
        arguments.append("HTML");

        if (QProcess::execute ( "getGoogleHtml.bat", arguments  ))
        {
            cout<<"Error:getGoogleHtml.bat "<<qPrintable(arguments.at(0))<<" "<<qPrintable(arguments.at(1))<<endl;
            deleteFile("HTML");
        }

        QStringList  fileUrls= parseGoogleHtml("HTML");

        int i=0; // download 2 images
        while ((fileUrls.count())>0&&(i<2))
        {
            deleteFile(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+('m'+i)+".jpg");
            arguments.clear();
            arguments.append(fileUrls.first()+QString(" "));
            arguments.append(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+('m'+i)+QString(" "));

            cout<<"getImage.bat "<<qPrintable(arguments.at(0))<<" "<<qPrintable(arguments.at(1))<<endl;

            if (QProcess::execute ("getImage.bat", arguments  ))
            {
                cout<<"Error:getImage.bat "<<qPrintable(arguments.at(0))<<" "<<qPrintable(arguments.at(1))<<endl;
            }
            else
            {
                if (checkIsFileOk(courseFileDirectory+"media"+QDir::separator()+getMediaFileName(ID)+('m'+i)+".jpg"))
                    i++;
            }
            fileUrls.removeFirst();
         }
    }
}

/////////////////////////////////////////////////////////////////////////////
void  setDelete (QDomNode &topicNode)
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
void  doDelete (QDomNode &docElement)
{
    QDomNode n = docElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            if (e.attribute("delete","none")=="true")
            {
                cout <<"doDelete: "<<" id:"<<qPrintable(e.attribute("id","0"))<<" name:"<<qPrintable(e.attribute("name",""))<<endl;
                QDomNode tmp = n;
                n = n.nextSibling();
                docElement.removeChild(tmp);
                continue;
            }
        }
        n = n.nextSibling();
    }
}

/////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
  QDomDocument doc("mydocument");
  QTextStream inputFileStream;
  QString courseFile;
  QString courseFileDirectory;
  QFile inputFile;
  tOPTIONS options ={0};
  int GID = 0;

  {
      if (argc<2)
      {
          cout<<"Za ma³o argumentów"<<endl;
          return 1;
      }

      inputFile.setFileName(argv[1]);

     if (!inputFile.open(QIODevice::ReadOnly))
     {
         cout << " Nie mo¿na otworzyæ pliku: "<<argv[1]<<endl;
         return 1;
     }

     inputFileStream.setDevice( &inputFile );
     inputFileStream.setCodec("UTF-8");
     courseFile=inputFileStream.readLine();

    QFileInfo courseFileInfo(courseFile);
    courseFileDirectory = QDir::toNativeSeparators(courseFileInfo.dir().path())+QDir::separator();

     QFile docFile(courseFile);

     if (!doc.setContent(&docFile)) {
         docFile.close();
         cout << " Nie mo¿na odczytaæ pliku: "<<qPrintable(courseFile)<<endl;
         return 1;
     }
     docFile.close();

 } // reading course.xml

 QDomElement rootElement = doc.documentElement();
 GID = getGID(rootElement);


 QString  topicName;

// parse options
 {
    QString line=inputFileStream.readLine();
    if (line.isNull()) return 0;  //end of file
    QStringList list = line.split(" ");
    if (list.count()<1)
    {
         cout<<"B³¹d opcji:"<<qPrintable(line)<<endl;
         return 1;
    }
    topicName = list.at(0);
    for (int i=1;i<list.count();++i)
    {
        if (list.at(i)=="Force")    options.bit.oForce = true;
        if (list.at(i)=="Double")   options.bit.oDouble= true;
        if (list.at(i)=="Voice")    options.bit.oVoice = true;
        if (list.at(i)=="Image")    options.bit.oImage = true;
        if (list.at(i)=="Clean")    options.bit.oClean = true;
    }
 }

 QString topicNameA = topicName;
 QString topicNameB = topicName+"*";


 QDomNode topicNodeA = getNode (rootElement,topicNameA,doc,courseFileDirectory,"pres",GID);
 QDomNode topicNodeB = getNode (rootElement,topicNameB,doc,courseFileDirectory,"pres",GID);

 if (options.bit.oClean)
 {
     setDelete (topicNodeA);
     setDelete (topicNodeB);
}

 while (1)
 {
    QString line=inputFileStream.readLine();
    if (line.isNull()) break;  //end of file
    QStringList list1 = line.split(":");
    if (list1.count()<2)
    {
        cout<<"B³¹d danych:"<<qPrintable(line)<<endl;
        return 1;
    }

    generateCourseElement((list1.at(0)).trimmed(),(list1.at(1)).trimmed(),topicNameA,topicNodeA,doc,courseFileDirectory,options,false,GID);

    if (options.bit.oDouble)
    {
        generateCourseElement((list1.at(1)).trimmed(),(list1.at(0)).trimmed(),topicNameB,topicNodeB,doc,courseFileDirectory,options,true,GID);
    }

 }// main loop

 if (options.bit.oClean)
 {
     doDelete (topicNodeA);
     doDelete (topicNodeB);
 }

 writeDomDoucumentToFile(doc,courseFile);
 return 0;
}

