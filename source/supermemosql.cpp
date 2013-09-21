//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <set>

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

#include "globaltracer.h"
#include "supermemosql.h"

/////////////////////////////////////////////////////////////////////////////
bool isValidSuperMemoDatabase (const QSqlDatabase &db)
{
   QStringList tables = db.tables();
   if (!tables.contains("Courses") ||
       !tables.contains("Items") ||
       !tables.contains("Items"))
      return false;

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::open(const QString &fileName)
{
   m_database = QSqlDatabase::addDatabase("QSQLITE");
   m_database.setDatabaseName(fileName);
   m_database.open();

   if (!m_database.isOpen() ||
       !m_database.isValid() ||
       !isValidSuperMemoDatabase(m_database)) {
      trace(QString("cSuperMemoSQL::open:")+fileName+QString(" Error:")+(m_database.lastError().text()), traceError);
      return false;
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
void SuperMemoSQL::trace(const QString &text, const int &flags)
{
   globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::getCourses (QStringList *retList)
{
   QSqlQuery query (m_database);
   QString q = QString("Select %1 from %2").arg("Title", "Courses");
   if (!query.exec(q)) {
      trace(QString("cSuperMemoSQL::getCourses error query.exec(): ")+query.lastError().text(), traceError);
      return false;
   }

   retList->clear();
   for (query.first();query.isValid();query.next())
      retList->append(query.value(0).toString());

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::getCourseDetails (const QString &courseName, int *id, QString *path)
{
   QSqlQuery query (m_database);
   QString q = QString("Select %1, %2 from %3 where %4 = :v1").arg("Id", "Path", "Courses", "Title");
   query.prepare(q);
   query.bindValue(":v1", QVariant(courseName));
   if (!query.exec()) {
      trace(QString("cSuperMemoSQL::getCourses error query.exec(): ")+query.lastError().text(), traceError);
      return false;
   }

   if (!query.first()) {
      trace(QString("cSuperMemoSQL::getCourseIdPath - Cannot find course with name: ")+courseName, traceWarning);
      return false;
   }

   QVariant v1 = query.value(0);
   *id = v1.toInt();

   QVariant v2 = query.value(1);
   *path = QFileInfo(v2.toString()).path()+QString("\\override\\course.xml");

   trace(QString("cSuperMemoSQL::getCourseIdPath ID: ")+QString::number(*id), traceLevel2);
   trace(QString("cSuperMemoSQL::getCourseIdPath Path:")+*path, traceLevel2);

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::addItem (const QString &elementName, int courseId, int parentItemId, int *itemId)
{
   QString filter = QString("CourseId = %1 and ParentID = %2 and Name = \"%3\"")
         .arg(QString::number(courseId), QString::number(parentItemId), QString(elementName).remove("\""));
   trace(QString("setElementSQL, filter:")+filter, traceLevel3);

   QSqlTableModel model(0, m_database);
   model.setEditStrategy(QSqlTableModel::OnManualSubmit);
   model.setTable("items");
   model.setFilter(filter);
   if (!model.select()) {
      trace(QString("setElementSQL: select: ")+m_database.lastError().text(), traceError);
      return false;
   }

   QSqlRecord record = model.record(0);
   if (model.rowCount() == 1) {
      *itemId = record.value("PageNum").toInt();
      return true;
   }

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
   record.setValue("Type", (parentItemId > 0) ? 0 : 5);

   trace(QString("setElementSQL, generate new record: ")
         + QString(" CourseId:") + QString::number(courseId)
         + QString(" ParentId:") + QString::number(parentItemId)
         + QString(" Name:") + elementName
         + QString(" PageNum:") + QString::number(*itemId)
         + QString(" QueueOrder:") + QString::number(*itemId)

         , traceLevel3);

   if (!model.insertRecord(-1, record)){ // to the end
      trace(QString("setElementSQL insertRecord: ")+m_database.lastError().text(), traceError);
      return false;
   }

   if (!model.database().transaction() ||
       !model.submitAll() ||
       !model.database().commit()) {
      model.database().rollback();
      trace(QString("setElementSQL submitAll: ")+m_database.lastError().text(), traceLevel3);
      return false;
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::deleteNotValidItems (int courseId, int parentItemId, const std::set<int> &validItemsId)
{
   std::set<int> itemsId;
   if (!getItems(courseId, parentItemId, &itemsId))
      return false;

   for (std::set<int>::iterator i=itemsId.begin(); i!=itemsId.end(); ++i) {
      if (validItemsId.find(*i) == validItemsId.end()) {
         if (!deleteItem (courseId, parentItemId, *i))
            return false;
      }
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::getItems (int courseId, int parentItemId, std::set<int> *itemsId)
{
   QSqlQuery query (m_database);
   QString q = QString("select %1 from %2 where %3 = :v3 and %4 = :v4")
         .arg("PageNum", "items", "CourseId", "ParentID");
   query.prepare(q);

   query.bindValue(":v3", QVariant(courseId));
   query.bindValue(":v4", QVariant(parentItemId));
   if (!query.exec()) {
      trace(QString("doDelete error query.exec(): ")+query.lastError().text(), traceError);
      return false;
   }

   for (query.first(), itemsId->clear(); query.isValid(); query.next())
      itemsId->insert(query.value(0).toInt());

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::deleteItem (int courseId, int parentItemId, int itemId)
{
   QSqlQuery query (m_database);
   QString q = QString("delete from %1 where %2 = :v2 and %3 = :v3 and %4 = :v4")
         .arg("items", "CourseId", "ParentID", "PageNum");

   query.prepare(q);
   query.bindValue(":v2", QVariant(courseId));
   query.bindValue(":v3", QVariant(parentItemId));
   query.bindValue(":v4", QVariant(itemId));

   if (!query.exec()) {
      trace(QString("deleteItem error query.exec(): ")+query.lastError().text(), traceError);
      return false;
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool SuperMemoSQL::getItemId (const QString &itemName, int courseId, int parentItemId, int *retID)
{
   QSqlQuery query (m_database);
   QString q = QString("select %1 from %2 where %3 = :v1 and %4 = :v2 and %4 = :v3")
         .arg("PageNum", "items", "CourseId", "ParentID", "Name");
   query.prepare(q);
   query.bindValue(":v1", QVariant(courseId));
   query.bindValue(":v2", QVariant(parentItemId));
   query.bindValue(":v3", QVariant(itemName));
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
bool SuperMemoSQL::getCourseMaxId (int courseID, int *maxId)
{
   QSqlQuery query (m_database);
   QString q1 = QString("select max(%1) from items where CourseId = :v1").arg("PageNum");
   query.prepare(q1);
   query.bindValue(":v1", QVariant(courseID));
   if (!query.exec()) {
      trace(QString("cSuperMemoSQL::getCourseMaxId error query.exec(): ") + query.lastError().text(), traceError);
      return false;
   }

   *maxId = 1;
   if (query.first())
      *maxId = query.value(0).toInt();

   return true;
}
