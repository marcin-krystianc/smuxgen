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
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QStandardItemModel>

#include "coursetemplate.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE


class QMyItemModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   QMyItemModel();
   ~QMyItemModel();
   QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   QModelIndex parent ( const QModelIndex & index ) const;
   int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
};

class ContentTable: public QWidget
{
   Q_OBJECT

public:
   ContentTable();
   bool fromCourseTemplate(const CourseTemplate &courseTemplate);
   bool toCourseTemplate (CourseTemplate *courseTemplate);

private:
   QMyItemModel m_templateModel;
   //QStandardItemModel m_itemModel;
   QTableView m_templateView;
   //QTableView m_itemView;

private slots:
   void itemChangedSlot(QStandardItem *item);
};


#endif
