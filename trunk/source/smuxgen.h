#ifndef SMUXGEN_H
#define SMUXGEN_H

#include <QString>
#include <QSqlDatabase>
#include <qdom.h>


const QString VERSION = QString::fromUtf8("2.0");


/*
/////////////////////////////////////////////////////////////////////////////
bool getDatabaseSQL(const USER_OPTIONS &options, QSqlDatabase &database);

/////////////////////////////////////////////////////////////////////////////
bool getOptions (QString optionsString,USER_OPTIONS &options);

/////////////////////////////////////////////////////////////////////////////
QString quotationString (QString s);

/////////////////////////////////////////////////////////////////////////////
bool getCourseIdPathSQL (const USER_OPTIONS &options,QSqlDatabase &database,QString course, int &id,QString &path);

/////////////////////////////////////////////////////////////////////////////
bool setElementSQL (const USER_OPTIONS &options, QSqlDatabase &database,QString topic, int courseIDSQL,int elementIDSQL,int paretntIDSQL);

/////////////////////////////////////////////////////////////////////////////
bool generateCourseElement(const USER_OPTIONS &options,QSqlDatabase &database,int courseIDSQL,QString question,QString answer,QString topicName,QDomNode &topicNode,int topicID,QDomDocument &doc,QString courseFileDirectory,bool bMode,int &GID);

/////////////////////////////////////////////////////////////////////////////
bool  doDelete (const USER_OPTIONS &options,QSqlDatabase &database,int courseIDSQL,int paretntIDSQL,QDomNode &docElement);
*/

#endif // SMUXGEN_H
