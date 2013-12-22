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
#include <QLineEdit>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "coursetemplate.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

struct MyItem {
   QString text;
   QImage pic1;
   QImage pic2;
};


class FindToolbar : public QWidget
{
   Q_OBJECT
public:
   FindToolbar(QWidget *parent = 0);
   void setFindFocus();

private:
   QLineEdit m_lineEdit;

private slots:
   void textChangedSlot (const QString&);

signals:
   void textChanged (const QString&);
};

class QTemplateDetailedModel: public QAbstractItemModel
{
   Q_OBJECT

public:
   QTemplateDetailedModel(MyItem *item);
   ~QTemplateDetailedModel();
   QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   QModelIndex parent ( const QModelIndex & index ) const;
   int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   QVariant setData ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
private:
   enum ROWS {
      e_text = 0,
      e_pic1 = 1,
      e_pic2 = 2
   };
   MyItem *m_item;
};

class QTemplateModel: public QAbstractItemModel
{
   Q_OBJECT

public:
   QTemplateModel();
   ~QTemplateModel();
   QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   QModelIndex parent ( const QModelIndex & index ) const;
   int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
   int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
   QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
   QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   QVariant setData ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
   Qt::ItemFlags flags(const QModelIndex &index) const;

   bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
   bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

   void fromCourseTemplate (const std::vector<ContentItem> &content);
   void toCourseTemplate (std::vector<ContentItem> *content);

   QTemplateDetailedModel* getDetailedModel (const QModelIndex &index);
private:
   std::vector<std::vector<MyItem> > m_items;
};

class ContentTable: public QWidget
{
   Q_OBJECT

public:
   ContentTable();
   ~ContentTable();
   void fromCourseTemplate (const std::vector<ContentItem> &content);
   void toCourseTemplate (std::vector<ContentItem> *content);
   QTemplateDetailedModel *m_detailedModel;

private:
   QTemplateModel m_templateModel;
   QTableView m_templateView;
   QTableView m_detailedView;
   QSortFilterProxyModel m_templateProxyModel;
   FindToolbar m_findToolbar;

private slots:
   void selectionChanged(const QModelIndex &index);
   void keyPressEvent (QKeyEvent * event);
   void filterChanged (const QString&);
};


#endif
