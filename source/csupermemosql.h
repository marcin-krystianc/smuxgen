//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CSUPERMEMOSQL_H
#define CSUPERMEMOSQL_H

#include <QString>
#include <QSqlDatabase>
#include "cglobaltracer.h"

class cSuperMemoSQL
{
public:
    cSuperMemoSQL();
    bool open(const QString &fileName);
    bool getCourses (QStringList &retList);
    bool getCourseIdPath (QString course, int *id, QString *path);
    bool setElementSQL (QString elementName, int courseIDSQL, int paretntIDSQL, int &elementIDSQL);
    bool getElementID (int courseIDSQL, int parentID, QString elementName, int &retID);

    QSqlDatabase getDatabase();

private:
    QSqlDatabase m_database;

    bool getCourseMaxId (int courseId, int *maxId);
    void trace (const QString &text, const int & flags = traceLevel1|0);
    bool isValidSuperMemoDatabase();
    QString quotationString (QString s);
};

#endif // CSUPERMEMOSQL_H
