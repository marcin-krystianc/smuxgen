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
   m_itemModel.setColumnCount(2);
   m_itemModel.setHorizontalHeaderLabels(labels);
   m_itemModel.setRowCount(2);

   setModel(&m_itemModel);
   horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
   /*
   setRowCount(2);
   setColumnCount(2);
   QStringList labels;
   labels.push_back("Question");
   labels.push_back("Answer");
   setHorizontalHeaderLabels(labels);
   horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
   */
}

