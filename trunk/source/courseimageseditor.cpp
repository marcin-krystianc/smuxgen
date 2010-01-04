//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QtGui>
#include <QTextEdit>
#include <QColorGroup>
#include <QPalette>
#include <QDateTime>
#include <Phonon>
#include <QDomDocument>

#include "courseimageseditor.h"
#include "globalsmuxgentools.h"


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cCourseImageEditor::cCourseImageEditor(QWidget *parent)
    : QWidget (parent)
{
    this->readyCourseElementList    = new cReadyCourseElementList;
    this->imageTargetWidget         = new cImageTargetWidget;
    this->imageSearch               = new cImageSearch;


    QHBoxLayout *l0 = new QHBoxLayout;

    l0->addWidget(this->readyCourseElementList,1);
    l0->addWidget(this->imageTargetWidget,1 );
    l0->addWidget(this->imageSearch,2);

    setLayout(l0);

    connect(    this->readyCourseElementList  , SIGNAL(elementSelectedSignal (const QString &,const QString &,const QString &,const QString &,const QString &,const QString &)),
                this ,SLOT(elementSelectedSlot (const QString &,const QString &,const QString &,const QString &,const QString &,const QString &)));

}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::clear()
{
    this->readyCourseElementList->clear();
}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::workWith (const cCourseTemplate &courseTemplate)
{
    this->trace(QString("Picture Browser:;WorkWith course:")+courseTemplate.options.course+" lesson:"+courseTemplate.options.subname,traceLevel1);
    this->clear();

    if (!this->database.open(courseTemplate.options.database))
        return;

    int courseID;
    QString courseFileName,courseFileDirectoryName,mediaDirectoryName;

    if (!this->database.getCourseIdPath (courseTemplate.options.course, courseID, courseFileName))
        return;

    QFileInfo courseFileInfo(courseFileName);
    courseFileDirectoryName = QDir::toNativeSeparators(courseFileInfo.dir().path())+QDir::separator();

    QString topicNameA = courseTemplate.options.subname;
    QString topicNameB = courseTemplate.options.subname+"*";

    int topicIDA,topicIDB;

    if (!this->database.getElementID(courseID,0,topicNameA,topicIDA))
        return;

    if (!this->database.getElementID(courseID,0,topicNameB,topicIDB))
        return;

    mediaDirectoryName = courseFileDirectoryName+"media"+QDir::separator();

    for (int i=0;i<courseTemplate.content.count();++i)
    {
        int id1,id2;
        QString line=(courseTemplate.content.at(i)).trimmed();
        if (line.length()==0) continue;
        QStringList list1 = line.split(":");

        if (list1.count()<2)
        {
            trace(QString("cCourseImageEditor::workWith Input error: ")+line,traceWarning);
            continue;
        }

        if (!this->database.getElementID(courseID,topicIDA,list1.at(0),id1))
            continue;

        if (!this->database.getElementID(courseID,topicIDB,list1.at(1),id2))
            continue;

        QString f1 = mediaDirectoryName+getMediaFileName(id1)+"m.jpg";
        QString f2 = mediaDirectoryName+getMediaFileName(id1)+"n.jpg";
        QString f3 = mediaDirectoryName+getMediaFileName(id2)+"m.jpg";
        QString f4 = mediaDirectoryName+getMediaFileName(id2)+"n.jpg";

        this->readyCourseElementList->addItem(line,getKeyWord(list1.at(0)),f1,f2,getKeyWord(list1.at(1)),f3,f4);
    }
}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::trace(const QString &text,const int & flags)
{
    globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::elementSelectedSlot (const QString &keyword1,const QString &file1m,const QString &file1n,const QString &keyword2,const QString &file2m,const QString &file2n)
{
    this->imageSearch->setNewKeywordsChangedL(keyword1);
    this->imageSearch->setNewKeywordsChangedR(keyword2);

    QStringList list;
    list<<file1m<<file1n<<file2m<<file2n;
    this->imageTargetWidget->setFiles(list);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cImageList::cImageList(QWidget *parent,int maxCount )
    : QListWidget (parent)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(tileSizeX, tileSizeX));
    setSpacing(1);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    this->maxCount = maxCount;
    this->resetPosition();
    this->setMouseTracking(true);

    connect (   this,   SIGNAL  (itemClicked        (QListWidgetItem* )),
                this,   SLOT    (itemClickedSlot    (QListWidgetItem* )));
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::resetPosition()
{
    this->rowIndex = 0;
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::addPiece(const QPixmap &pixmap,const QString &hint)
{
    QListWidgetItem *pieceItem = new QListWidgetItem();
    pieceItem->setStatusTip(hint);

    this->insertItem(this->rowIndex++,pieceItem);
    pieceItem->setIcon(QIcon(pixmap));

    pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsDragEnabled);
    pieceItem->setData(Qt::UserRole, QVariant(pixmap));

   if (this->count()>this->maxCount)
       delete takeItem(this->count()-1);
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::addPieceSlot(const QPixmap &pixmap,const QString& hint)
{
    this->addPiece(pixmap,hint);
}
/////////////////////////////////////////////////////////////////////////////
void cImageList::trace(const QString &text,const int & flags)
{
    globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::startDrag(Qt::DropActions /*supportedActions*/)
{
    QListWidgetItem *item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap = qVariantValue<QPixmap>(item->data(Qt::UserRole));

    dataStream << pixmap ;
    pixmap = pixmap.scaled(tileSizeX,tileSizeY);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(tileMimeFormat(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);

    drag->exec(Qt::MoveAction);

}

/////////////////////////////////////////////////////////////////////////////
void cImageList::itemClickedSlot(  QListWidgetItem * item )
{
    /*
    if (!item)
        return;
    trace ("cImageList::itemClickedSlot ");
    QPixmap pixmap = qVariantValue<QPixmap>(item->data(Qt::UserRole));
    item->setIcon(QIcon(pixmap.scaled(tileSizeX*2,tileSizeY*2)));
    */
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::setIconSizeSlot    (int size )
{
    setIconSize(QSize(size, size));
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cImageSearch::cImageSearch(QWidget *parent)
    : QWidget (parent)
{


    this->leftEdit  = new QLineEdit;
    this->rightEdit = new QLineEdit;
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(this->leftEdit);
    l1->addWidget(this->rightEdit);

    this->leftProgress  = new QProgressBar;
    this->rightProgress = new QProgressBar;
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->addWidget(this->leftProgress);
    l2->addWidget(this->rightProgress);

    this->zoomSlider = new QSlider(Qt::Horizontal);
    this->zoomSlider->setMinimum(100);
    this->zoomSlider->setMaximum(500);
    QHBoxLayout *l3 = new QHBoxLayout;
    l3->addWidget(new QLabel("Zoom:"));
    l3->addWidget(this->zoomSlider,1);

    this->imagelist = new cImageList();
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addLayout(l1);
    l0->addLayout(l2);
    l0->addLayout(l3);
    l0->addWidget(this->imagelist);

    setLayout(l0);

    this->timerL = new QTimer;
    this->timerR = new QTimer;

    this->timerL->setSingleShot(true);
    this->timerR->setSingleShot(true);

    connect(this->leftEdit  , SIGNAL(textChanged(const QString& ))  ,this ,SLOT(newKeywordsChangedL(const QString&)));
    connect(this->rightEdit , SIGNAL(textChanged(const QString& ))  ,this ,SLOT(newKeywordsChangedR(const QString&)));
    connect(this->timerL    ,SIGNAL(timeout())  , this, SLOT(newKeywordsL()));
    connect(this->timerR    ,SIGNAL(timeout())  , this, SLOT(newKeywordsR()));




    this->imageDownloader[0] = new cImageDownloader("L");
    this->imageDownloader[1] = new cImageDownloader("R");

    connect(this->imageDownloader[0]    ,SIGNAL(signalImage(const QPixmap&,const QString&))  , this->imagelist, SLOT(addPieceSlot(const QPixmap&,const QString&)));
    connect(this->imageDownloader[1]    ,SIGNAL(signalImage(const QPixmap&,const QString&))  , this->imagelist, SLOT(addPieceSlot(const QPixmap&,const QString&)));

    connect(this->imageDownloader[0]    ,SIGNAL(sProgressRange (int ,int))  , this->leftProgress    , SLOT(setRange (int ,int)));
    connect(this->imageDownloader[1]    ,SIGNAL(sProgressRange (int ,int))  , this->rightProgress   , SLOT(setRange (int ,int)));
    connect(this->imageDownloader[0]    ,SIGNAL(sProgressValue(int))  , this->leftProgress    , SLOT(setValue(int)));
    connect(this->imageDownloader[1]    ,SIGNAL(sProgressValue(int))  , this->rightProgress   , SLOT(setValue(int)));

    connect (this->zoomSlider           ,SIGNAL(valueChanged(int))      ,this->imagelist    , SLOT(setIconSizeSlot(int)));
/*
    QHBoxLayout *textFiledsLayout = new QHBoxLayout;
    textFiledsLayout->addWidget(courseLabel);
    textFiledsLayout->addWidget(courseCombo);
 */

}
/////////////////////////////////////////////////////////////////////////////
cImageSearch::~cImageSearch()
{
    delete this->imageDownloader[0];
    delete this->imageDownloader[1];
}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::newKeywords (const QString &txt,int id)
{
    this->imagelist->resetPosition();
    this->trace(txt+QString::fromUtf8(" id:")+QString::number(id),traceLevel2);
    this->imageDownloader[id]->getImages(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::newKeywordsL ()
{
    this->newKeywords(textL,0);
}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::newKeywordsR ()
{
    this->newKeywords(textR,1);
}
/////////////////////////////////////////////////////////////////////////////
void cImageSearch::trace(const QString &text,const int & flags)
{
    globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::newKeywordsChangedL (const QString &txt)
{
    this->timerL->stop();
    this->timerL->setInterval(200);
    this->timerL->start();
    this->textL = txt;
}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::newKeywordsChangedR (const QString &txt)
{
    this->timerR->stop();
    this->timerR->setInterval(200);
    this->timerR->start();
    this->textR = txt;

}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::setNewKeywordsChangedL (const QString &txt)
{
    this->leftEdit->setText(txt);
}

/////////////////////////////////////////////////////////////////////////////
void cImageSearch::setNewKeywordsChangedR (const QString &txt)
{
    this->rightEdit->setText(txt);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cImageButtonWidget::cImageButtonWidget(QWidget *parent)
    : QWidget (parent)
{

    this->label   = new QLabel;
    this->label->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    this->label->setMinimumSize(QSize(50,50));
    this->label->setMaximumSize(QSize(500,500));

    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(this->label);
    l0->setMargin(0);
    setLayout(l0);

    setAcceptDrops(true);

    this->setFile("");
}

/////////////////////////////////////////////////////////////////////////////
void cImageButtonWidget::setPixmap (const QPixmap &pixmap)
{
    this->label->setPixmap(pixmap.scaled(this->size()));
}

/////////////////////////////////////////////////////////////////////////////
bool cImageButtonWidget::setFile(const QString &path)
{
    this->filePath = path;
    QImage tmpImg;

    if (!tmpImg.load(this->filePath))
    {
        trace(QString("cImageButtonWidget::setFile cannot open file:")+filePath,traceError);
        this->setStatusTip(QString("Cannot open file:")+filePath);
        this->setPixmap(QPixmap(":/images/monkey_off.png"));
        return false;
    }

    this->setStatusTip(filePath);
    this->setPixmap(QPixmap(this->filePath));
    return true;
}
/////////////////////////////////////////////////////////////////////////////
void cImageButtonWidget::trace(const QString &text,const int & flags)
{
    globalTracer.trace(text,flags);
}


/////////////////////////////////////////////////////////////////////////////
void cImageButtonWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(tileMimeFormat()))
        event->accept();
    else
        event->ignore();
}

/////////////////////////////////////////////////////////////////////////////
void cImageButtonWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(tileMimeFormat())) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else
        event->ignore();
}

/////////////////////////////////////////////////////////////////////////////
void cImageButtonWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(tileMimeFormat()))
    {
        QByteArray pieceData = event->mimeData()->data(this->tileMimeFormat());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;

        dataStream >> pixmap;

        if (!this->filePath.isEmpty())
        {
            if (pixmap.toImage().save(this->filePath))
                this->setPixmap(pixmap);
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else
        event->ignore();
}
/////////////////////////////////////////////////////////////////////////////
void cImageButtonWidget::resizeEvent(QResizeEvent *event)
{
   this->setPixmap(*this->label->pixmap());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cImageTargetWidget::cImageTargetWidget(QWidget *parent)
    : QWidget (parent)
{
    this->imageButtonWidget[0][0] = new cImageButtonWidget;
    this->imageButtonWidget[0][1] = new cImageButtonWidget;
    this->imageButtonWidget[1][0] = new cImageButtonWidget;
    this->imageButtonWidget[1][1] = new cImageButtonWidget;

    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(imageButtonWidget[0][0],1);
    l1->addWidget(imageButtonWidget[0][1],1);
    l1->addWidget(imageButtonWidget[1][0],1);
    l1->addWidget(imageButtonWidget[1][1],1);

    setLayout(l1);
    //this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

}

/////////////////////////////////////////////////////////////////////////////
void cImageTargetWidget::setFiles (const QStringList &list)
{
    this->imageButtonWidget[0][0]->setFile(list.at(0));
    this->imageButtonWidget[0][1]->setFile(list.at(1));
    this->imageButtonWidget[1][0]->setFile(list.at(2));
    this->imageButtonWidget[1][1]->setFile(list.at(3));
}
/////////////////////////////////////////////////////////////////////////////
void cImageTargetWidget::resizeEvent ( QResizeEvent * event )
{
   //this->setMinimumSize(QSize(0,0));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cReadyCourseElementList::cReadyCourseElementList(QWidget *parent)
    : QWidget (parent)
{

    this->listWidget =new QListWidget;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(this->listWidget);
    setLayout(l0);

    connect(this->listWidget    ,SIGNAL(currentItemChanged( QListWidgetItem*,QListWidgetItem*))  , this, SLOT(itemActivatedSlot( QListWidgetItem*)));

}

/////////////////////////////////////////////////////////////////////////////
void cReadyCourseElementList::addItem (const QString &text,const QString &keyword1,const QString &file1m,const QString &file1n,const QString &keyword2,const QString &file2m,const QString &file2n)
{
    //QByteArray itemData;
    QStringList dataStream;

    dataStream<<keyword1<<file1m<<file1n<<keyword2<<file2m<<file2n;

    this->listWidget->insertItem(0,text);
    this->listWidget->item(0)->setData(Qt::UserRole,dataStream);
}

/////////////////////////////////////////////////////////////////////////////
void cReadyCourseElementList::clear ()
{
    this->listWidget->clear();
}

/////////////////////////////////////////////////////////////////////////////
void cReadyCourseElementList::itemActivatedSlot   ( QListWidgetItem * item )
{
    if (!item)
        return;

    QStringList dS;
    dS = item->data(Qt::UserRole).toStringList();


    emit elementSelectedSignal (dS[0],dS[1],dS[2],dS[3],dS[4],dS[5]);
}
