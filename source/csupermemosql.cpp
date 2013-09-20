//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSqlRecord>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlTableModel>

#include "cglobaltracer.h"
#include "csupermemosql.h"

/////////////////////////////////////////////////////////////////////////////
cSuperMemoSQL::cSuperMemoSQL()
{
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::open(const QString &fileName)
{
    this->m_database = QSqlDatabase::addDatabase("QSQLITE");
    this->m_database.setDatabaseName(fileName);
    this->m_database.open();

    if ((!this->m_database.isOpen()) ||
            (!this->m_database.isValid()) ||
            (!isValidSuperMemoDatabase()))
    {
        trace(QString("cSuperMemoSQL::open:")+fileName+QString(" Error:")+(this->m_database.lastError().text()), traceError);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
void cSuperMemoSQL::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::isValidSuperMemoDatabase ()
{
    QStringList tables = this->m_database.tables();
    if (!tables.contains("Courses") ||
            !tables.contains("Items") ||
            !tables.contains("Items"))
        return false;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourses (QStringList &retList)
{
    QSqlQuery query (this->m_database);
    QString filter = "Select Title from Courses";
    if (!query.exec(filter))
    {
        trace(QString("cSuperMemoSQL::getCourses error query.exec(): ")+query.lastError().text(), traceError);
        return false;
    }


    retList.clear();
    for (query.first();query.isValid();query.next())
    {
        retList.append(query.value(0).toString());
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourseIdPath (QString course, int *id, QString *path)
{
    QSqlQuery query (this->m_database);
    QString filter = "Select Id, Path from Courses where Title = "+quotationString(course);
    if (!query.exec(filter)) {
        trace(QString("cSuperMemoSQL::getCourses error query.exec(): ")+query.lastError().text(), traceError);
        return false;
    }

    if (!query.first()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath - Cannot find course with name: ")+course, traceWarning);
        return false;
    }

    QVariant v1 = query.value(0);
    QVariant v2 = query.value(1);

    if (!v1.isValid()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath Cannot find Id: "), traceError);
        return false;
    }

    if (!v2.isValid()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath Cannot find path: "), traceError);
        return false;
    }

    *id = v1.toInt();
    QFileInfo courseFileInfo(v2.toString());
    *path = courseFileInfo.path()+QDir::separator()+QString("override")+QDir::separator()+QString("course.xml");

    this->trace(QString("cSuperMemoSQL::getCourseIdPath ID: ")+QString::number(*id), traceLevel2);
    this->trace(QString("cSuperMemoSQL::getCourseIdPath Path:")+*path, traceLevel2);

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
bool cSuperMemoSQL::setElementSQL (QString elementName, int courseIDSQL, int paretntIDSQL, int &elementIDSQL)
{
    QSqlTableModel model(0, this->m_database);

    QString filter; // CourseID + PageNum - primary key
    filter += QString::fromUtf8("CourseId = ")+QString::number(courseIDSQL);
    filter += QString::fromUtf8(" and ");
    filter += QString::fromUtf8("ParentID = ")+QString::number(paretntIDSQL);
    filter += QString::fromUtf8(" and ");
    filter += QString::fromUtf8(" Name = \"")+elementName.remove("\"")+QString::fromUtf8("\"");

    //trace (QString("setElementSQL: lastQuery = ")+query.lastQuery(), traceLevel3);
    model.setTable("items");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setFilter(filter);

    trace(QString("setElementSQL, filter:")+filter, traceLevel3);

    if (!model.select())
    {
        trace(QString("setElementSQL: select: ")+m_database.lastError().text(), traceError);
        return false;
    }

    QSqlRecord record = model.record(0);

    if (model.rowCount() == 0)
    {
        // generate new record
        int maxId = 0;
        if (!getCourseMaxId(courseIDSQL, &maxId))
            return false;

        elementIDSQL = maxId + 1;
        record.setValue("CourseId", courseIDSQL);
        record.setValue("Name", elementName);
        record.setValue("PageNum", elementIDSQL);
        record.setValue("QueueOrder", elementIDSQL);
        record.setValue("ParentId", paretntIDSQL);

        if (paretntIDSQL>0)
            record.setValue("Type", 0);
        else
            record.setValue("Type", 5);

        trace(QString("setElementSQL, generate new record: ")
              + QString(" CourseId:") +QString::number(courseIDSQL)
              + QString(" Name:") +elementName
              + QString(" PageNum:") +QString::number(elementIDSQL)
              + QString(" QueueOrder:") +QString::number(elementIDSQL)
              + QString(" ParentId:") +QString::number(paretntIDSQL)
              , traceLevel3);

        if (!model.insertRecord(-1, record)){ // to the end
            trace(QString("setElementSQL insertRecord: ")+m_database.lastError().text(), traceError);
            return false;
        }
    }
    else
        elementIDSQL = record.value("PageNum").toInt();

    model.database().transaction();
    if (model.submitAll()) {
        model.database().commit();
    } else {
        model.database().rollback();
        trace(QString("setElementSQL submitAll: ")+m_database.lastError().text(), traceLevel3);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getElementID (int courseIDSQL, int parentID, QString elementName, int &retID)
{

    QString filter; // CourseID + PageNum - primary key
    filter += QString::fromUtf8("select PageNum from items where ");
    filter += QString::fromUtf8("CourseId = ")+QString::number(courseIDSQL);
    filter += QString::fromUtf8(" and ");
    filter += QString::fromUtf8("ParentID = ")+QString::number(parentID);
    filter += QString::fromUtf8(" and ");
    filter += QString::fromUtf8(" Name = \"")+elementName.remove('\"')+QString::fromUtf8("\"");

    QSqlQuery query (this->m_database);
    if (!query.exec(filter)) // delete all unknown course items
    {
        trace(QString("cSuperMemoSQL::getElementID error query.exec(): ")+query.lastError().text(), traceError);
        return false;
    }
    query.first();
    QVariant tmp = query.value(0);

    if (!tmp.isValid())
        return false;

    retID = tmp.toInt();
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourseMaxId (int courseID, int *maxId)
{
    // TODO: remove this function

    QSqlQuery query (m_database);
    QString q1 = QString("select max(%1) from items where CourseId = :v1").arg("PageNum");
    query.prepare(q1);
    query.bindValue(":v1", QVariant(courseID));
    if (!query.exec()) {
       trace(QString("cSuperMemoSQL::getCourseMaxId error query.exec(): ") + query.lastError().text(), traceError);
       return false;
    }

    *maxId = 1;
    if (!query.first())
        return true;

    *maxId = query.value(0).toInt();
    return true;
}

/////////////////////////////////////////////////////////////////////////////
QSqlDatabase cSuperMemoSQL::getDatabase()
{
    return this->m_database;
}
