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

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class ContentTable: public QTableView
{
   Q_OBJECT

public:
   ContentTable();
private:
   QStandardItemModel m_itemModel;
private slots:
   void itemChangedSlot(QStandardItem *item);
};


#endif
