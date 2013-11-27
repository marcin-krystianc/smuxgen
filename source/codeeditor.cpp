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
QMyItemModel::QMyItemModel()
{
   MyItem m1 = {"asdf"};
   MyItem m2 = {"qwerty"};

   m_items[0].push_back(m1);
   m_items[1].push_back(m2);
}

/////////////////////////////////////////////////////////////////////////////
QMyItemModel::~QMyItemModel()
{

}

/////////////////////////////////////////////////////////////////////////////
QModelIndex QMyItemModel::index(int row, int column, const QModelIndex&) const
{
   return createIndex(row, column, NULL);
}

/////////////////////////////////////////////////////////////////////////////
QModelIndex QMyItemModel::parent (const QModelIndex&) const
{
   // returns invalid (no parent) index
   return QModelIndex();
}

/////////////////////////////////////////////////////////////////////////////
int QMyItemModel::rowCount (const QModelIndex &) const
{
   return m_items[0].size();
}

/////////////////////////////////////////////////////////////////////////////
int QMyItemModel::columnCount (const QModelIndex &) const
{
   return 2;
}

/////////////////////////////////////////////////////////////////////////////
QVariant QMyItemModel::data (const QModelIndex &index, int role) const
{
   switch (role)
   {
      case Qt::DisplayRole:
      case Qt::EditRole:
      {
         return m_items[index.column()][index.row()].text;
      }

      default:
         break;
   }
   return QVariant();
}

/////////////////////////////////////////////////////////////////////////////
bool QMyItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   if (role != Qt::EditRole ||
       index.column() > 1)
      return false;

   int nRows = rowCount();
   int row = index.row();

   if (nRows <= index.row() + 1) {
      insertRows (nRows - 1, 1);
   }

   m_items[index.column()][index.row()].text = value.toString();
   emit dataChanged (index, index);
   if (index.column() != 0)
      return true;

   if (!value.isValid() ||
       value.toString().trimmed().isEmpty()) {
      removeRows(row, 1);
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
Qt::ItemFlags QMyItemModel::flags(const QModelIndex &) const
{
   return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/////////////////////////////////////////////////////////////////////////////
bool QMyItemModel::insertRows (int row, int count, const QModelIndex &)
{
   size_t nRows = rowCount();
   beginInsertRows (QModelIndex(), row, row + count - 1);
   m_items[0].resize(nRows + count);
   m_items[1].resize(nRows + count);
   std::rotate (&m_items[0][row], &m_items[0][row], &m_items[0][rowCount()-1]);
   std::rotate (&m_items[1][row], &m_items[1][row], &m_items[1][rowCount()-1]);
   endInsertRows();
   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool QMyItemModel::removeRows (int row, int count, const QModelIndex &)
{
   int nRows = rowCount();
   beginRemoveRows (QModelIndex(), row, row + count - 1);
   for (int i=row; i<nRows-count; ++i) {
      m_items[0][i] = m_items[0][i+count];
      m_items[1][i] = m_items[1][i+count];
   }
   m_items[0].resize(nRows - count);
   m_items[1].resize(nRows - count);
   endRemoveRows();
   return true;
}

/////////////////////////////////////////////////////////////////////////////
void QMyItemModel::fromCourseTemplate(const std::vector<ContentItem> &content)
{
   removeRows(0, rowCount());
   for (size_t i = 0; i < content.size(); ++i) {
      int row = rowCount();
      insertRows(row, 1);
      m_items[0][row].text = content[i].question;
      m_items[1][row].text = content[i].answer;
   }
   insertRows(rowCount(), 1);

   emit dataChanged (createIndex(0, 0), createIndex(rowCount() - 1, columnCount()-1));
}

/////////////////////////////////////////////////////////////////////////////
void QMyItemModel::toCourseTemplate (std::vector<ContentItem> *content)
{
   content->clear();
   for (int i = 0; i < rowCount(); ++i) {
      if (m_items[0][i].text.trimmed() == "" ||
          m_items[1][i].text.trimmed() == ""  )
         continue;

      ContentItem item;
      item.question = m_items[0][i].text;
      item.answer = m_items[1][i].text;
      content->push_back(item);
   }
}

/////////////////////////////////////////////////////////////////////////////
ContentTable::ContentTable()
{
   //m_templateModel.setHorizontalHeaderLabels(QStringList() << "Question" << "Answer");

   connect(&m_templateModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex &))
           , this , SLOT(dataChangedSlot(const QModelIndex&, const QModelIndex &)));


   m_templateView.setModel(&m_templateModel);
   m_templateView.horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   m_templateView.horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

   //m_itemModel.setRowCount(2);
   //m_itemModel.setColumnCount(2);
   //m_itemView.setModel(&m_itemModel);

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(&m_templateView);
   //layout->addWidget(&m_itemView);
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
void ContentTable::dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
   int x=0;
}

/////////////////////////////////////////////////////////////////////////////
void ContentTable::fromCourseTemplate(const std::vector<ContentItem> &content)
{
   m_templateModel.fromCourseTemplate(content);

   //m_itemModel.setData(m_itemModel.index(0,0), Qt::lightGray, Qt::BackgroundColorRole);
   //m_templateModel.setRowCount(1);
   //m_templateModel.setData(m_itemModel.index(0,0), QString("asdf"), Qt::UserRole+1);
   //m_templateModel.setData(m_itemModel.index(0,0), QImage(), Qt::UserRole+2);
   //m_templateModel.setData(m_itemModel.index(0,0), QStandardItemModel(), Qt::UserRole+3);
}

/////////////////////////////////////////////////////////////////////////////
void ContentTable::toCourseTemplate (std::vector<ContentItem> *content)
{
   m_templateModel.toCourseTemplate(content);
}


