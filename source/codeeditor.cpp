//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QtGui>
#include <QStringList>

#include "codeeditor.h"

/////////////////////////////////////////////////////////////////////////////
ContentTable::ContentTable()
{
   QStringList labels;
   labels.push_back("Question");
   labels.push_back("Answer");
   m_itemModel.setHorizontalHeaderLabels(labels);
   m_itemModel.setData(m_itemModel.index(0,0), Qt::lightGray, Qt::BackgroundColorRole);
   setModel(&m_itemModel);
   connect(&m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this , SLOT(itemChangedSlot(QStandardItem *)));

   //setRowHeight(0,5);

   horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
   m_itemModel.setRowCount(1);

   /*

   setColumnCount(2);
   QStringList labels;
   labels.push_back("Question");
   labels.push_back("Answer");
   setHorizontalHeaderLabels(labels);
   horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
   */
}

/////////////////////////////////////////////////////////////////////////////
void ContentTable::itemChangedSlot(QStandardItem *item)
{
   // remove row if element was cleared
   if (item->data(Qt::EditRole).isNull() ||
       item->data(Qt::EditRole).toString().isEmpty()) {
      m_itemModel.removeRow(m_itemModel.indexFromItem(item).row());
   }

   int rowCount = m_itemModel.rowCount();
   QStandardItem *lastItem = m_itemModel.item(rowCount - 1, 0);
   if (!lastItem ||
       lastItem->data(Qt::EditRole).isNull() ||
       lastItem->data(Qt::EditRole).toString().isEmpty()) {
      // last row is empty
   }
   else {
      // last row is not empty, add new one
      m_itemModel.setRowCount(rowCount+1);
   }
}
