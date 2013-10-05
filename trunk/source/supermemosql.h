//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CSUPERMEMOSQL_H
#define CSUPERMEMOSQL_H

#include <set>

#include <QString>
#include <QSqlDatabase>
#include "globaltracer.h"

class SuperMemoSQL
{
public:
   static bool getAvailableDbList (QStringList *retList);

   void getCourses (QStringList *retList);
   void getCourseDetails(const QString &courseName, int *id, QString *path);
   void getItems (int courseId, int parentItemId, std::set<int> *itemsId);
   bool getItemId (const QString &itemName, int courseId, int parentItemId, int *retID);
   void addItem(const QString &itemName, int courseId, int parentItemId, int *itemId);
   void deleteItem (int courseId, int parentItemId, int itemId);
   bool openUser(const QString &userName);

private:
   void getCourseMaxId(int courseId, int *maxId);
   void trace (const QString &text, const int &flags = traceLevel1|0);
   bool openFile(const QString &fileName);

   QSqlDatabase m_database;
};

#endif // CSUPERMEMOSQL_H
