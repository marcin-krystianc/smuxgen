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
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(fileName);
    m_database.open();

    if (!m_database.isOpen() ||
            !m_database.isValid() ||
            !isValidSuperMemoDatabase(m_database))
    {
        trace(QString("cSuperMemoSQL::open:")+fileName+QString(" Error:")+(m_database.lastError().text()), traceError);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
void cSuperMemoSQL::trace(const QString &text, const int &flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::isValidSuperMemoDatabase (const QSqlDatabase &db)
{
    QStringList tables = db.tables();
    if (!tables.contains("Courses") ||
            !tables.contains("Items") ||
            !tables.contains("Items"))
        return false;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourses (QStringList *retList)
{
    QSqlQuery query (m_database);
    QString q = QString("Select %1 from %2").arg("Title", "Courses");
    if (!query.exec(q)) {
        trace(QString("cSuperMemoSQL::getCourses error query.exec(): ")+query.lastError().text(), traceError);
        return false;
    }

    retList->clear();
    for (query.first();query.isValid();query.next()) {
        retList->append(query.value(0).toString());
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::getCourseIdPath (QString course, int *id, QString *path)
{
    QSqlQuery query (m_database);
    QString q = QString("Select %1, %2 from %3 where %4 = :v1").arg("Id", "Path", "Courses", "Title");
    query.prepare(q);
    query.bindValue(":v1", QVariant(course));
    if (!query.exec()) {
        trace(QString("cSuperMemoSQL::getCourses error query.exec(): ")+query.lastError().text(), traceError);
        return false;
    }

    if (!query.first()) {
        this->trace(QString("cSuperMemoSQL::getCourseIdPath - Cannot find course with name: ")+course, traceWarning);
        return false;
    }

    QVariant v1 = query.value(0);
    *id = v1.toInt();

    QVariant v2 = query.value(1);
    *path = QFileInfo(v2.toString()).path()+QString("\\override\\course.xml");

    this->trace(QString("cSuperMemoSQL::getCourseIdPath ID: ")+QString::number(*id), traceLevel2);
    this->trace(QString("cSuperMemoSQL::getCourseIdPath Path:")+*path, traceLevel2);

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool cSuperMemoSQL::setElementSQL (QString elementName, int courseId, int parentItemId, int *itemId)
{
    QSqlTableModel model(0, m_database);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QString filter = QString("CourseId = %1 and ParentID = %2 and Name = \"%3\"")
            .arg(QString::number(courseId), QString::number(parentItemId), elementName.remove("\""));

    model.setTable("items");
    model.setFilter(filter);

    trace(QString("setElementSQL, filter:")+filter, traceLevel3);

    if (!model.select()) {
        trace(QString("setElementSQL: select: ")+m_database.lastError().text(), traceError);
        return false;
    }

    QSqlRecord record = model.record(0);
    if (model.rowCount() == 0)
    {
        // generate new record
        int maxId = 0;
        if (!getCourseMaxId(courseId, &maxId))
            return false;

        *itemId = maxId + 1;
        record.setValue("CourseId", courseId);
        record.setValue("Name", elementName);
        record.setValue("PageNum", *itemId);
        record.setValue("QueueOrder", *itemId);
        record.setValue("ParentId", parentItemId);

        if (parentItemId>0)
            record.setValue("Type", 0);
        else
            record.setValue("Type", 5);

        trace(QString("setElementSQL, generate new record: ")
              + QString(" CourseId:") +QString::number(courseId)
              + QString(" Name:") +elementName
              + QString(" PageNum:") +QString::number(*itemId)
              + QString(" QueueOrder:") +QString::number(*itemId)
              + QString(" ParentId:") +QString::number(parentItemId)
              , traceLevel3);

        if (!model.insertRecord(-1, record)){ // to the end
            trace(QString("setElementSQL insertRecord: ")+m_database.lastError().text(), traceError);
            return false;
        }
    }
    else
        *itemId = record.value("PageNum").toInt();

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
bool cSuperMemoSQL::getElementID (int courseId, int parentItemId, QString elementName, int *retID)
{
    QSqlQuery query (m_database);
    QString q = QString("select %1 from %2 where %3 = :v1 and %4 = :v2 and %4 = :v3")
            .arg("PageNum", "items", "CourseId", "ParentID", "Name");
    query.prepare(q);
    query.bindValue(":v1", QVariant(courseId));
    query.bindValue(":v2", QVariant(parentItemId));
    query.bindValue(":v3", QVariant(elementName.remove('\"')));
    if (!query.exec()) {
        trace(QString("cSuperMemoSQL::getElementID error query.exec(): ")+query.lastError().text(), traceError);
        return false;
    }

    if (!query.first())
        return false;

    *retID = query.value(0).toInt();
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
    return m_database;
}
