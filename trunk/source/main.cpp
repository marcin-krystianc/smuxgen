//============================================================================
// Name         : main.cpp
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================



#include "smuxgen.h"

#include <QtCore/QCoreApplication>
#include <QApplication>

#include <qdom.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDate>
#include <QTextCodec>
#include <QProcess>
#include <QSettings>
#include <iostream>
#include <list>
#include <algorithm>
#include <utility>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>

using namespace std;





/////////////////////////////////////////////////////////////////////////////
bool getOptions (QString optionsString,USER_OPTIONS &options)
{
    options.bit.oForce  = false;
    options.bit.oVoice  = false;
    options.bit.oImage  = false;
    options.bit.oDouble = false;
    options.bit.oTSQL   = false;
    options.bit.oTXML   = false;
    options.user.clear();
    options.course.clear();
    options.subname.clear();

    QStringList chunks = optionsString.split(" ",QString::SkipEmptyParts);

    int i=0;
    while (i<chunks.count())
    {
        QString first = chunks.at(i++);

        if (first==QString::fromUtf8("-Force")) {
            options.bit.oForce = true;
            continue;
        }

        if (first==QString::fromUtf8("-Double")) {
            options.bit.oDouble = true;
            continue;
        }

        if (first==QString::fromUtf8("-Voice")){
            options.bit.oVoice = true;
            continue;
        }

        if (first==QString::fromUtf8("-Image")){
            options.bit.oImage = true;
            continue;
        }
        if (first==QString::fromUtf8("-Tsql")){
            options.bit.oTSQL = true;
            continue;
        }

        if (first==QString::fromUtf8("-Txml")){
            options.bit.oTXML = true;
            continue;
        }


        if (i==chunks.count())
        {
            cout <<"No value after: "<<qPrintable(first)<<endl;
            return false;
        }

        QString second = chunks.at(i++);

        if (first==QString::fromUtf8("-course")) {
            options.course = second;
            continue;
        }

        if (first==QString::fromUtf8("-user")) {
            options.user = second;
            continue;
        }

        if (first==QString::fromUtf8("-subname")) {
            options.subname = second;
            continue;
        }

        if (first==QString::fromUtf8("-version")) {
            if (second==VERSION)
                continue;
            cout<<qPrintable(QString::fromUtf8("Unsupportet version:"))<<qPrintable(second)<<endl;
        }

        cout <<qPrintable(QString::fromUtf8("Unknown parameter:"))<<qPrintable(first)<<endl;
        return false;   // wrong parameters
    }

    cout<<endl;
    cout<<qPrintable(QString::fromUtf8("User defined parameters:"))<<endl;
    cout<<qPrintable(QString::fromUtf8("oForce:"))<<options.bit.oForce<<endl;
    cout<<qPrintable(QString::fromUtf8("oDouble:"))<<options.bit.oDouble<<endl;
    cout<<qPrintable(QString::fromUtf8("oImage:"))<<options.bit.oImage<<endl;
    cout<<qPrintable(QString::fromUtf8("oVoice:"))<<options.bit.oVoice<<endl;
    cout<<qPrintable(QString::fromUtf8("user:"))<<qPrintable(options.user)<<endl;
    cout<<qPrintable(QString::fromUtf8("course:"))<<qPrintable(options.course)<<endl;
    cout<<qPrintable(QString::fromUtf8("subname:"))<<qPrintable(options.subname)<<endl;
    cout<<endl;

    return true;
}


/////////////////////////////////////////////////////////////////////////////
bool getDatabaseSQL(const USER_OPTIONS &options, QSqlDatabase &database)
{
    QString smux =QString::fromUtf8("SuperMemo UX");
    QString bf  = QString::fromUtf8("Repetitions.dat");

    QSettings cfg(QSettings::IniFormat, QSettings::UserScope,"SuperMemo World", smux);
    QString config_dir = QDir::toNativeSeparators(QFileInfo(cfg.fileName()).absolutePath()) ;
    QString databaseFile =config_dir+QDir::separator()+"SuperMemo UX"+  QDir::separator()+options.user+ QDir::separator()+bf;
    cout<<qPrintable(databaseFile)<<endl;

    database=QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(databaseFile);
    if (!database.open())
    {
        cout<<qPrintable(QString::fromUtf8("getDatabase: "))<<qPrintable(database.lastError().text())<<endl;
        return false;
    }

    if (options.bit.oTSQL)
        cout<<qPrintable(QString::fromUtf8("getDatabase, OK:"))<<qPrintable(databaseFile)<<endl;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
QString quotationString (QString s)
{
    s.prepend(QString::fromUtf8("'"));
    s.append(QString::fromUtf8("'"));
    return s;
}

/////////////////////////////////////////////////////////////////////////////
bool getCourseIdPathSQL (const USER_OPTIONS &options, QSqlDatabase &database,QString course, int &id,QString &path)
{
    QSqlTableModel model(0,database);
    model.setTable("courses");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(QString::fromUtf8("Title=")+quotationString(course));

    if (!model.select())
    {
        cout<<qPrintable(QString::fromUtf8("getCourseIdPath: "))<<qPrintable(database.lastError().text())<<endl;
        return false;
    }

    if (model.rowCount()==0)
    {
        cout<<qPrintable(QString::fromUtf8("getCourseIdPath: Cannot find course with name:"))<<qPrintable(course)<<endl;
        return false;    
    }

    QSqlRecord record   = model.record(0);
    QVariant v1         = record.value(QString::fromUtf8("Id"));
    QVariant v2         = record.value(QString::fromUtf8("Path"));

    if (!v1.isValid())
    {
        cout<<qPrintable(QString::fromUtf8("getCourseIdPath: Cannot find Id:"))<<endl;
        return false;
    }

    if (!v2.isValid())
    {
        cout<<qPrintable(QString::fromUtf8("getCourseIdPath: Cannot find Id:"))<<endl;
        return false;
    }

    id  = v1.toInt();
    path= v2.toString();

    QFileInfo courseFileInfo(path);
    path = QDir::toNativeSeparators(courseFileInfo.dir().path())+QDir::separator()
           +QString::fromUtf8("override")+QDir::separator()+QString::fromUtf8("course.xml");

    if (options.bit.oTSQL)
    {
        cout<<qPrintable(QString::fromUtf8("getCourseIdPath ID:"))<<id<<endl;
        cout<<qPrintable(QString::fromUtf8("getCourseIdPath Path:"))<<qPrintable(path)<<endl;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool setElementSQL (const USER_OPTIONS &options, QSqlDatabase &database,QString elementName, int courseIDSQL,int elementIDSQL,int paretntIDSQL)
{
    QSqlTableModel model(0,database);
    QString   filter;   // CourseID + PageNum - primary key
    filter  +=QString::fromUtf8("CourseId=")+QString::number(courseIDSQL);
    filter  +=QString::fromUtf8(" and ");
    filter  +=QString::fromUtf8("PageNum=")+QString::number(elementIDSQL);
    filter  +=QString::fromUtf8(" and ");
    filter  +=QString::fromUtf8("ParentID=")+QString::number(paretntIDSQL);

    model.setTable("items");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(filter);

    if (options.bit.oTSQL)
           cout<<qPrintable(QString::fromUtf8("setElementSQL, filter:"))<<qPrintable(filter)<<endl;

    if (!model.select())
    {
        cout<<qPrintable(QString::fromUtf8("setElementSQL: select:"))<<qPrintable(database.lastError().text())<<endl;
        return false;
    }

    QSqlRecord record=model.record(0);

    if (model.rowCount()==0)
    { // generate new record
        record.setValue("CourseId",courseIDSQL);
        record.setValue("elementName",elementName);
        record.setValue("PageNum",elementIDSQL);
        record.setValue("QueueOrder",elementIDSQL);
        record.setValue("ParentId",paretntIDSQL);

        if (paretntIDSQL>0)
            record.setValue("Type",0);
        else
            record.setValue("Type",5);

        if (options.bit.oTSQL)
           cout<<qPrintable(QString::fromUtf8("setElementSQL, generate new record:"))
            <<qPrintable(QString::fromUtf8(" CourseId:"))<<courseIDSQL
            <<qPrintable(QString::fromUtf8(" elementName:"))<<qPrintable(elementName)
            <<qPrintable(QString::fromUtf8(" PageNum:"))<<elementIDSQL
            <<qPrintable(QString::fromUtf8(" QueueOrder:"))<<elementIDSQL
            <<qPrintable(QString::fromUtf8(" ParentId:"))<<paretntIDSQL<<endl;

        if (!model.insertRecord(-1,record)){ // to the end
            cout<<qPrintable(QString::fromUtf8("setElementSQL insertRecord: "))<<qPrintable(database.lastError().text())<<endl;
            return false;
        }
    }
    else
    {
        record.setValue("elementName",elementName);
        record.setValue("QueueOrder",elementIDSQL);

        if (options.bit.oTSQL)
            cout<<qPrintable(QString::fromUtf8("setElementSQL, modify record:"))
            <<qPrintable(QString::fromUtf8(" elementName:"))<<qPrintable(elementName)
            <<qPrintable(QString::fromUtf8(" QueueOrder:"))<<elementIDSQL<<endl;

        model.setRecord(0,record);
    }

    model.database().transaction();
    if (model.submitAll()) {
        model.database().commit();
    } else {
        model.database().rollback();
        cout<<qPrintable(QString::fromUtf8("setElementSQL submitAll: "))<<qPrintable(database.lastError().text())<<endl;
        return false;
    }

    return true;
}

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
QDomDocument createCourseItem (int templateId,QString chapter,QString title,QString question,QString answers,int ID,USER_OPTIONS options,QString audioType)
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

            QDomElement tr = doc.createElement("tr");

                QDomElement td= doc.createElement("td");
                td.setAttribute("align","center");

                    QDomElement gfx= doc.createElement("gfx");
                    gfx.setAttribute("file","m");
                    gfx.setAttribute("space","5");
                    gfx.setAttribute("width","320");
                    gfx.setAttribute("height","320");
                td.appendChild(gfx);

            tr.appendChild(td);

                td= doc.createElement("td");
                td.setAttribute("align","center");

                    gfx= doc.createElement("gfx");
                    gfx.setAttribute("file","n");
                    gfx.setAttribute("space","5");
                    gfx.setAttribute("width","320");
                    gfx.setAttribute("height","320");
                td.appendChild(gfx);

            tr.appendChild(td);
        table.appendChild(tr);
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
bool generateCourseElement(const USER_OPTIONS &options,QSqlDatabase &database,int courseIDSQL,QString question,QString answer,QString topicName,QDomNode &topicNode,int topicID,QDomDocument &doc,QString courseFileDirectory,bool bMode,int &GID)
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

        if (!setElementSQL(options, database,question,courseIDSQL,ID,topicID))
            return false;

        writeDomDoucumentToFile(docItem,courseFileDirectory+getFileName(ID));

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
    return true;
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
bool  doDelete (const USER_OPTIONS &options,QSqlDatabase &database,int courseIDSQL,int paretntIDSQL,QDomNode &docElement)
{
    QDomNode n = docElement.firstChild();
    list <int> listValidID;
    list <int>::iterator it;

    if (options.bit.oTXML || options.bit.oTSQL)
        cout <<qPrintable(QString::fromUtf8("doDelete: id:"))
            <<qPrintable(QString::fromUtf8(" CourseId:"))<<courseIDSQL
            <<qPrintable(QString::fromUtf8(" ParetntId:"))<<paretntIDSQL<<endl;


    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull())
        {
            if (e.attribute("delete","none")=="true")
            {
                if (options.bit.oTXML)
                    cout <<qPrintable(QString::fromUtf8("doDelete: id:"))<<qPrintable(e.attribute("id","0"))<<" name:"<<qPrintable(e.attribute("name",""))<<endl;
                QDomNode tmp = n;
                n = n.nextSibling();
                docElement.removeChild(tmp);
                continue;
            }
            listValidID.push_back(e.attribute("id","0").toInt());
            if (options.bit.oTSQL)
                cout<<qPrintable(QString::fromUtf8("validID:"))<<(e.attribute("id","0").toInt())<<endl;
        }
        n = n.nextSibling();
    }

    // get all ID (PageNum) from database
    QSqlTableModel model(0,database);
    QString   filter;   // CourseID + PageNum - primary key
    filter  +=QString::fromUtf8("CourseId=")+QString::number(courseIDSQL);
    filter  +=QString::fromUtf8(" and ");
    filter  +=QString::fromUtf8("ParentID=")+QString::number(paretntIDSQL);

    model.setTable("items");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(filter);

    if (options.bit.oTSQL)
        cout<<qPrintable(QString::fromUtf8("doDelete, filter"))<<qPrintable(filter)<<endl;

    if (!model.select())
    {
        cout<<qPrintable(QString::fromUtf8("doDelete: select:"))<<qPrintable(database.lastError().text())<<endl;
        return false;
    }


    for (int i=0;i<model.rowCount();i++)
    {
       QSqlRecord record=model.record(i);
       QVariant v1  = record.value(QString::fromUtf8("PageNum"));
       if (!v1.isValid())
       {
            cout<<qPrintable(QString::fromUtf8("doDelete: Cannot get IDs from database:"))<<endl;
            return false;
       }

       it = find(listValidID.begin(),listValidID.end(),v1.toInt());
       if (it==listValidID.end())
       {   // all elements that are not in XML will be removed from SQL
           model.removeRows(i,1);
           if (options.bit.oTSQL)
             cout<<qPrintable(QString::fromUtf8("doDelete, remove:"))
                <<qPrintable(QString::fromUtf8(" i:"))<<i
                <<qPrintable(QString::fromUtf8(" PageNum:"))<<v1.toInt()<<endl;
       }
       else
            if (options.bit.oTSQL)
            cout<<qPrintable(QString::fromUtf8("doDelete, leave:"))
                <<qPrintable(QString::fromUtf8(" i:"))<<i
                <<qPrintable(QString::fromUtf8(" PageNum:"))<<v1.toInt()<<endl;


    }

    model.database().transaction();
    if (model.submitAll()) {
        model.database().commit();
    } else {
        model.database().rollback();
        cout<<qPrintable(QString::fromUtf8("doDelete submitAll: "))<<qPrintable(database.lastError().text())<<endl;
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
    QApplication app(argc, argv);   // sql will not work without that

    QDomDocument doc("mydocument");
    QTextStream inputFileStream;
    QString courseFileDirectory;
    QString courseFileName;
    int courseIDSQL;
    QFile inputFile;
    USER_OPTIONS options;
    QSqlDatabase database;
    int GID = 0, topicAID =0, topicBID =0;


    cout<<qPrintable(QString::fromUtf8("SMUXGEN - SuperMemo UX generator v"))<<qPrintable(VERSION)<<endl;

    // check SMUXGEN.exe arguments
    if (argc<2)
    {
        cout<<qPrintable(QString::fromUtf8("Too few arguments"))<<endl;
        return 1;
    }
    inputFile.setFileName(argv[1]);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        cout << qPrintable(QString::fromUtf8("Cannot open file: "))<<argv[1]<<endl;
        return 1;
    }

    // check media directory
    QDir *pDir= new QDir(courseFileDirectory);
    if (!pDir->exists("media"))
       pDir->mkdir("media");

    cout<<qPrintable(QString::fromUtf8("Processing input file:"))<<argv[1]<<endl;

    // get user file options   
    inputFileStream.setDevice( &inputFile );
    inputFileStream.setCodec("UTF-8");
    if (!getOptions(inputFileStream.readLine(),options))
        return 0;

    if (!getDatabaseSQL(options,database))
        return 0;

    if (!getCourseIdPathSQL(options,database,options.course,courseIDSQL,courseFileName))
        return 0;

    QFileInfo courseFileInfo(courseFileName);
    courseFileDirectory = QDir::toNativeSeparators(courseFileInfo.dir().path())+QDir::separator();

    QFile docFile(courseFileName);

    if (!doc.setContent(&docFile)) {
     docFile.close();
     cout << qPrintable(QString::fromUtf8("Cannot open file: "))<<qPrintable(courseFileName)<<endl;
     return 0;
    }
    docFile.close();

    QDomElement rootElement = doc.documentElement();
    GID = getGID(rootElement);

    QString topicNameA = options.subname;
    QString topicNameB = options.subname+"*";

    QDomNode topicNodeA;
    QDomNode topicNodeB;

    // A course
    topicNodeA = getNode (rootElement,topicNameA,doc,courseFileDirectory,"pres",topicAID,GID);
    if (!setElementSQL(options, database,topicNameA,courseIDSQL,topicAID,0))
        return 0;

    setDelete (topicNodeA);

    qint64 inputFilePos = inputFileStream.pos(); //save for B course

    while (1)
    {
        QString line=(inputFileStream.readLine()).trimmed();
        if (line.isNull()) break;  //end of file
        if (line.length()==0) continue;
        QStringList list1 = line.split(":");

        if (list1.count()<2)
        {
            cout<<qPrintable(QString::fromUtf8("Input error:"))<<qPrintable(line)<<endl;
            continue;
        }
        generateCourseElement(options,database,courseIDSQL,(list1.at(0)).trimmed(),(list1.at(1)).trimmed(),topicNameA,topicNodeA,topicAID,doc,courseFileDirectory,false,GID);
    }


    doDelete (options,database,courseIDSQL,topicAID,topicNodeA);


    // B course
    if (options.bit.oDouble)
    {
        topicNodeB = getNode (rootElement,topicNameB,doc,courseFileDirectory,"pres",topicBID,GID);
        if (!setElementSQL (options, database,topicNameB,courseIDSQL,topicBID,0))
            return 0;

        setDelete (topicNodeB);

        inputFileStream.seek(inputFilePos);

        while (1)
        {
            QString line=(inputFileStream.readLine()).trimmed();
            if (line.isNull()) break;  //end of file
            if (line.length()==0) continue;
            QStringList list1 = line.split(":");

            if (list1.count()<2)
            {
                cout<<qPrintable(QString::fromUtf8("Input error: "))<<qPrintable(line)<<endl;
                continue;
            }
             generateCourseElement(options,database,courseIDSQL,(list1.at(1)).trimmed(),(list1.at(0)).trimmed(),topicNameB,topicNodeB,topicBID,doc,courseFileDirectory,true,GID);
         }

        doDelete (options,database,courseIDSQL,topicBID,topicNodeB);
    }

    writeDomDoucumentToFile(doc,courseFileName);
    return 0;
}

