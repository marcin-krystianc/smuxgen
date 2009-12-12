//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "cglobaltracer.h"
#include "csupermemosql.h"

/////////////////////////////////////////////////////////////////////////////
cSuperMemoSQL::cSuperMemoSQL()
{
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::open(const QString &fileName)
{
    this->database=QSqlDatabase::addDatabase("QSQLITE");
    this->database.setDatabaseName(fileName);
    this->database.open();

    if ((!this->database.isOpen())  ||
        (!this->database.isValid()) ||
        (!isValidSuperMemoDatabase()))
    {
        trace(QString("cSuperMemoSQL::open - error: ")+(this->database.lastError().text()),traceError);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
void cSuperMemoSQL::trace(const QString &text,const int & flags)
{
     globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::isValidSuperMemoDatabase ()
{
    QStringList tables=this->database.tables();
    if (!tables.contains("Courses") ||
        !tables.contains("Items")   ||
        !tables.contains("Items"))
        return false;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourses (QStringList &retList)
{
    QSqlTableModel model(0,database);
    model.setTable("Courses");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    if (!model.select())
    {
        this->trace(QString("cSuperMemoSQL::getCourses: ")+database.lastError().text(),traceError);
        return false;
    }

    retList.clear();
    for (int i=0;i<model.rowCount();i++)
    {
        QSqlRecord record   = model.record(i);
        QVariant v1         = record.value("Title");
        retList.append(v1.toString());
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourseIdPath (QString course, int &id,QString &path)
{

    QSqlTableModel model(0,database);
    model.setTable("Courses");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(QString::fromUtf8("Title=")+quotationString(course));

    if (!model.select()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath: ")+database.lastError().text(),traceError);
        return false;
    }

    if (model.rowCount()==0) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath - Cannot find course with name: ")+course,traceWarning);
        return false;
    }

    QSqlRecord record   = model.record(0);
    QVariant v1         = record.value(QString::fromUtf8("Id"));
    QVariant v2         = record.value(QString::fromUtf8("Path"));

    if (!v1.isValid()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath Cannot find Id: "),traceError);
        return false;
    }

    if (!v2.isValid()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath Cannot find path: "),traceError);
        return false;
    }

    id  = v1.toInt();
    path= v2.toString();

    QFileInfo courseFileInfo(path);

    path = courseFileInfo.path()+QDir::separator()+QString("override")+QDir::separator()+QString("course.xml");

    this->trace(QString("cSuperMemoSQL::getCourseIdPath ID: ")+QString::number(id)  ,traceLevel2);
    this->trace(QString("cSuperMemoSQL::getCourseIdPath Path:")+path                ,traceLevel2);

    return true;
}

/////////////////////////////////////////////////////////////////////////////
QString cSuperMemoSQL::quotationString (QString s)
{
    s.prepend(QString::fromUtf8("'"));
    s.append(QString::fromUtf8("'"));
    return s;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::setElementSQL (QString elementName, int courseIDSQL,int elementIDSQL,int paretntIDSQL)
{
    QSqlTableModel model(0,this->database);
    QString   filter;   // CourseID + PageNum - primary key
    filter  +=QString::fromUtf8("CourseId=")+QString::number(courseIDSQL);
    filter  +=QString::fromUtf8(" and ");
    filter  +=QString::fromUtf8("PageNum=")+QString::number(elementIDSQL);
    filter  +=QString::fromUtf8(" and ");
    filter  +=QString::fromUtf8("ParentID=")+QString::number(paretntIDSQL);

    model.setTable("items");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(filter);

    trace(QString("setElementSQL, filter:")+filter,traceLevel3);

    if (!model.select())
    {
        trace(QString("setElementSQL: select: ")+database.lastError().text(),traceError);
        return false;
    }

    QSqlRecord record=model.record(0);

    if (model.rowCount()==0)
    { // generate new record
        record.setValue("CourseId",courseIDSQL);
        record.setValue("Name",elementName);
        record.setValue("PageNum",elementIDSQL);
        record.setValue("QueueOrder",elementIDSQL);
        record.setValue("ParentId",paretntIDSQL);

        if (paretntIDSQL>0)
            record.setValue("Type",0);
        else
            record.setValue("Type",5);

       trace(QString("setElementSQL, generate new record: ")
            + QString(" CourseId:")     +QString::number(courseIDSQL)
            + QString(" Name:")         +elementName
            + QString(" PageNum:")      +QString::number(elementIDSQL)
            + QString(" QueueOrder:")   +QString::number(elementIDSQL)
            + QString(" ParentId:")     +QString::number(paretntIDSQL)
                                                         ,traceLevel3);

        if (!model.insertRecord(-1,record)){ // to the end
            trace(QString("setElementSQL insertRecord: ")+database.lastError().text(),traceError);
            return false;
        }
    }
    else
    {
        record.setValue("Name",elementName);
        record.setValue("QueueOrder",elementIDSQL);

        trace(QString("setElementSQL, modify record:")
            + QString(" Name:") + elementName
            + QString(" QueueOrder:")  + QString::number(elementIDSQL)
            ,traceLevel3);

        model.setRecord(0,record);
    }

    model.database().transaction();
    if (model.submitAll()) {
        model.database().commit();
    } else {
        model.database().rollback();
        trace(QString("setElementSQL submitAll: ")+database.lastError().text(),traceLevel3);
        return false;
    }

    return true;
}
/////////////////////////////////////////////////////////////////////////////
QSqlDatabase cSuperMemoSQL::getDatabase()
{
    return this->database;
}
