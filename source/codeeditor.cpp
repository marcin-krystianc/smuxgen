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
   m_templateModel.setHorizontalHeaderLabels(QStringList() << "Question" << "Answer");

   connect(&m_templateModel, SIGNAL(itemChanged(QStandardItem*)), this , SLOT(itemChangedSlot(QStandardItem *)));

   m_templateView.setModel(&m_templateModel);
   m_templateView.horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   m_templateView.horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

   m_itemModel.setRowCount(2);
   m_itemModel.setColumnCount(2);
   m_itemView.setModel(&m_itemModel);

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(&m_templateView);
   layout->addWidget(&m_itemView);
   setLayout(layout);

   /*
   //setRowHeight(0,5);
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
      m_templateModel.removeRow(m_templateModel.indexFromItem(item).row());
   }

   int rowCount = m_templateModel.rowCount();
   QStandardItem *lastItem = m_templateModel.item(rowCount - 1, 0);
   if (!lastItem ||
       lastItem->data(Qt::EditRole).isNull() ||
       lastItem->data(Qt::EditRole).toString().isEmpty()) {
      // last row is empty
   }
   else {
      // last row is not empty, add new one
      m_templateModel.setRowCount(rowCount+1);
   }

   if (rowCount == 0)
      m_templateModel.setRowCount(rowCount+1);
}

/////////////////////////////////////////////////////////////////////////////
bool ContentTable::fromCourseTemplate(const CourseTemplate &courseTemplate)
{
   //m_itemModel.setData(m_itemModel.index(0,0), Qt::lightGray, Qt::BackgroundColorRole);
   m_templateModel.setRowCount(1);
}
