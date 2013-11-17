//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QObject>
#include <QTableWidget>
#include <QStandardItemModel>

#include "coursetemplate.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class ContentTable: public QWidget
{
   Q_OBJECT

public:
   ContentTable();
   bool fromCourseTemplate(const CourseTemplate &courseTemplate);
   bool toCourseTemplate (CourseTemplate *courseTemplate);

private:
   QStandardItemModel m_templateModel;
   QStandardItemModel m_itemModel;
   QTableView m_templateView;
   QTableView m_itemView;

private slots:
   void itemChangedSlot(QStandardItem *item);
};


#endif
