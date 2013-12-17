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

#include "assert.h"

/////////////////////////////////////////////////////////////////////////////
QTemplateDetailedModel::QTemplateDetailedModel(MyItem *item):
   QAbstractItemModel(), m_item(item)
{
}

/////////////////////////////////////////////////////////////////////////////
QTemplateDetailedModel::~QTemplateDetailedModel()
{
}

/////////////////////////////////////////////////////////////////////////////
QModelIndex QTemplateDetailedModel::index(int row, int column, const QModelIndex&) const
{
   return createIndex(row, column, NULL);
}

/////////////////////////////////////////////////////////////////////////////
QModelIndex QTemplateDetailedModel::parent (const QModelIndex&) const
{
   // returns invalid (no parent) index
   return QModelIndex();
}

/////////////////////////////////////////////////////////////////////////////
int QTemplateDetailedModel::rowCount (const QModelIndex &) const
{
   return 4;
}

/////////////////////////////////////////////////////////////////////////////
int QTemplateDetailedModel::columnCount (const QModelIndex &) const
{
   return 2;
}

/////////////////////////////////////////////////////////////////////////////
QVariant QTemplateDetailedModel::data (const QModelIndex &index, int role) const
{
   if (index.column() == 0 &&
       role == Qt::DisplayRole)
   {
      switch (index.row())
      {
         case e_text:
            return "Visible text";

         default:
            return "?";
      }
   }

   if (index.column() != 1)
      return QVariant();

   if (role != Qt::DisplayRole &&
       role != Qt::EditRole)
      return QVariant();

   switch (index.row())
   {
      case e_text:
         return m_item->text;

      default:
         return "?";
   }

   return QVariant();
}

/////////////////////////////////////////////////////////////////////////////
Qt::ItemFlags QTemplateDetailedModel::flags(const QModelIndex &) const
{
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/////////////////////////////////////////////////////////////////////////////
QTemplateModel::QTemplateModel()
{
}

/////////////////////////////////////////////////////////////////////////////
QTemplateModel::~QTemplateModel()
{

}

/////////////////////////////////////////////////////////////////////////////
QModelIndex QTemplateModel::index(int row, int column, const QModelIndex&) const
{
   return createIndex(row, column, NULL);
}

/////////////////////////////////////////////////////////////////////////////
QModelIndex QTemplateModel::parent (const QModelIndex&) const
{
   // returns invalid (no parent) index
   return QModelIndex();
}

/////////////////////////////////////////////////////////////////////////////
int QTemplateModel::rowCount (const QModelIndex &) const
{
   return m_items.size();
}

/////////////////////////////////////////////////////////////////////////////
int QTemplateModel::columnCount (const QModelIndex &) const
{
   return 2;
}

/////////////////////////////////////////////////////////////////////////////
QVariant QTemplateModel::data (const QModelIndex &index, int role) const
{
   switch (role)
   {
      case Qt::DisplayRole:
      case Qt::EditRole:
      {
         return m_items[index.row()][index.column()].text;
      }

      default:
         break;
   }
   return QVariant();
}

/////////////////////////////////////////////////////////////////////////////
bool QTemplateModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   if (role != Qt::EditRole ||
       index.column() > 1)
      return false;

   int nRows = rowCount();
   int row = index.row();

   if (nRows <= index.row() + 1) {
      insertRows (nRows - 1, 1);
   }

   m_items[index.row()][index.column()].text = value.toString();
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
Qt::ItemFlags QTemplateModel::flags(const QModelIndex &) const
{
   return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/////////////////////////////////////////////////////////////////////////////
bool QTemplateModel::insertRows (int row, int count, const QModelIndex &)
{
   size_t nRows = rowCount();
   beginInsertRows (QModelIndex(), row, row + count - 1);
   m_items.resize(nRows + count);
   std::rotate (&m_items[row], &m_items[row], &m_items[rowCount()-1]);
   endInsertRows();
   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool QTemplateModel::removeRows (int row, int count, const QModelIndex &)
{
   if (count <= 0)
       return false;

   int nRows = rowCount();
   beginRemoveRows (QModelIndex(), row, row + count - 1);
   for (int i=row; i<nRows-count; ++i) {
      m_items[i] = m_items[i+count];
   }
   m_items.resize(nRows - count);
   endRemoveRows();
   return true;
}

/////////////////////////////////////////////////////////////////////////////
void QTemplateModel::fromCourseTemplate(const std::vector<ContentItem> &content)
{
   removeRows(0, rowCount());
   for (size_t i = 0; i < content.size(); ++i) {
      int row = rowCount();
      insertRows(row, 1);
      m_items[row].resize(2);
      m_items[row][0].text = content[i].question;
      m_items[row][1].text = content[i].answer;
   }
   insertRows(rowCount(), 1);

   emit dataChanged (createIndex(0, 0), createIndex(rowCount() - 1, columnCount()-1));
}

/////////////////////////////////////////////////////////////////////////////
void QTemplateModel::toCourseTemplate (std::vector<ContentItem> *content)
{
   content->clear();
   for (int i = 0; i < rowCount() - 1; ++i) {
      ContentItem item;
      item.question = m_items[i][0].text;
      item.answer = m_items[i][1].text;
      content->push_back(item);
   }
}

/////////////////////////////////////////////////////////////////////////////
QTemplateDetailedModel* QTemplateModel::getDetailedModel (const QModelIndex &index)
{
   if (index.row() >= m_items.size())
      return NULL;

   return new QTemplateDetailedModel(&m_items[index.row()][index.column()]);
}

/////////////////////////////////////////////////////////////////////////////
ContentTable::ContentTable()
   : m_detailedModel(NULL)
{
   //m_templateModel.setHorizontalHeaderLabels(QStringList() << "Question" << "Answer");

   connect(&m_templateView, SIGNAL(clicked (const QModelIndex &))
           , this, SLOT(selectionChanged(const QModelIndex&)));

   m_templateView.setModel(&m_templateModel);
   m_templateView.horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   m_templateView.horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(&m_templateView);
   layout->addWidget(&m_detailedView);
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
ContentTable::~ContentTable()
{
   delete m_detailedModel;
}

/////////////////////////////////////////////////////////////////////////////
void ContentTable::selectionChanged(const QModelIndex &index)
{
   delete m_detailedModel;
   m_detailedModel = m_templateModel.getDetailedModel(index);
   m_detailedView.setModel(m_detailedModel);
   m_detailedView.horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   m_detailedView.horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
   m_detailedView.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
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


