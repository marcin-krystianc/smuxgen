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

struct MyItem {
   QString text[2];
};

class QTemplateDetailedModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   QTemplateDetailedModel();
   ~QTemplateDetailedModel();
   QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   QModelIndex parent ( const QModelIndex & index ) const;
   int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   QVariant setData ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
};

class QTemplateModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   QTemplateModel();
   ~QTemplateModel();
   QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   QModelIndex parent ( const QModelIndex & index ) const;
   int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   QVariant setData ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
   Qt::ItemFlags flags(const QModelIndex &index) const;

   bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
   bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

   void fromCourseTemplate (const std::vector<ContentItem> &content);
   void toCourseTemplate (std::vector<ContentItem> *content);

private:

   std::vector<MyItem> m_items;
};


class ContentTable: public QWidget
{
   Q_OBJECT

public:
   ContentTable();
   void fromCourseTemplate (const std::vector<ContentItem> &content);
   void toCourseTemplate (std::vector<ContentItem> *content);

private:
   QTemplateModel m_templateModel;
   QTableView m_templateView;
   QTableView m_detailedView;

private slots:
   void dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};


#endif
