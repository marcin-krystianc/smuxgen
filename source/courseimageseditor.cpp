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
    this->mp3TargetWidget           = new cMp3TargetWidget;

    QSplitter *splitter             = new QSplitter(Qt::Horizontal);

    QWidget  *tmpWidget = new QWidget;
    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(this->readyCourseElementList);
    l1->addWidget(this->mp3TargetWidget);
    l1->setMargin(0);
    tmpWidget->setLayout(l1);

    splitter->setChildrenCollapsible(false);
    splitter->addWidget(tmpWidget);
    splitter->addWidget(this->imageTargetWidget );
    splitter->addWidget(this->imageSearch);
    splitter->setHandleWidth(10);
    QHBoxLayout *l0 = new QHBoxLayout;
    l0->addWidget(splitter);

    setLayout(l0);

    connect(    this->readyCourseElementList    ,SIGNAL(elementSelectedImgSignal(const QStringList&)),
                this                            ,SLOT(elementSelectedImgSlot (const QStringList&)));

    connect(    readyCourseElementList          ,SIGNAL(elementSelectedMP3Signal(const QStringList&)),
                mp3TargetWidget                 ,SLOT(elementSelectedMp3Slot(const QStringList&)));

}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::clear()
{
    this->readyCourseElementList->clear();
}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::workWith (const cCourseTemplate &courseTemplate)
{
    this->trace(QString("Course Browser::WorkWith course:")+courseTemplate.options.course+" lesson:"+courseTemplate.options.subname,traceLevel1);
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

    if (courseTemplate.options.bit.oDouble)
    {
        if (!this->database.getElementID(courseID,0,topicNameB,topicIDB))
            return;
    }

    mediaDirectoryName = courseFileDirectoryName+"media"+QDir::separator();

    for (int i=0;i<courseTemplate.content.count();++i)
    {
        int id1,id2;
        QString line=(courseTemplate.content.at(i)).trimmed();
        if (line.length()==0) continue;
        QStringList list1 = line.split(":");

        if (list1.count()< 2 )
        {
            trace(QString("cCourseImageEditor::workWith Input error: ")+line,traceWarning);
            continue;
        }

        if (!this->database.getElementID(courseID,topicIDA,getTextToPrint(list1.at(0)),id1))
            continue;

        QString f1m = mediaDirectoryName+getMediaFileName(id1)+"m.jpg";
        QString f1n = mediaDirectoryName+getMediaFileName(id1)+"n.jpg";
        QString f1a = mediaDirectoryName+getMediaFileName(id1)+"a.mp3";
        QString f1q = mediaDirectoryName+getMediaFileName(id1)+"q.mp3";
        QString k1  = getKeyWord(list1.at(0));
        QString q1  = getTextToPrint(list1.at(0));
        QString a1  = getTextToPrint(list1.at(1));

        QString f2m;
        QString f2n;
        QString f2a;
        QString f2q;
        QString k2;
        QString q2;
        QString a2;

        if (courseTemplate.options.bit.oDouble)
        {
            if (!this->database.getElementID(courseID,topicIDB,getTextToPrint(list1.at(1)),id2))
                continue;

            f2m = mediaDirectoryName+getMediaFileName(id2)+"m.jpg";
            f2n = mediaDirectoryName+getMediaFileName(id2)+"n.jpg";
            f2a = mediaDirectoryName+getMediaFileName(id2)+"a.mp3";
            f2q = mediaDirectoryName+getMediaFileName(id2)+"q.mp3";
            k2  = getKeyWord(list1.at(1));
            q2  = getTextToPrint(list1.at(1));
            a2  = getTextToPrint(list1.at(0));
        }

        QStringList imgList,mp3List;
        imgList<<k1<<f1m<<f1n<<k2<<f2m<<f2n;
        mp3List<<f1a<<f1q<<a1<<q1<<f2a<<f2q<<a2<<q2;
        this->readyCourseElementList->addItem(line,imgList,mp3List);
    }
}

/////////////////////////////////////////////////////////////////////////////
void cCourseImageEditor::trace(const QString &text,const int & flags)
{
    globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////

void cCourseImageEditor::elementSelectedImgSlot (const QStringList &imgData)
{
    if (!imgData.at(0).isEmpty())   // keyword1
        this->imageSearch->setNewKeywordsChangedL(imgData.at(0));

    if (!imgData.at(3).isEmpty())   // keyword2
        this->imageSearch->setNewKeywordsChangedR(imgData.at(3));

    QStringList list  = imgData;
    list.removeAt(3); // keyword1
    list.removeAt(0); // keyword2
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
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::resetPosition()
{
    this->rowIndex = 0;
}

/////////////////////////////////////////////////////////////////////////////
void cImageList::addPiece(const QPixmap &pixmap,const QString &hint)
{
    if (this->count()>=this->maxCount)
        delete takeItem(this->count()-1);

    QListWidgetItem *pieceItem = new QListWidgetItem();
    pieceItem->setStatusTip(hint);

    if (!hint.isNull())
    {
        for (int i=0;i<this->count();i++)
        {
            if(this->item(i)->data(Qt::UserRole+1).toString()==hint)
            {
                 this->insertItem(this->rowIndex++,this->takeItem(i));  // move to the beginning of list
                 return;
            }
        }
    }

    this->insertItem(this->rowIndex++,pieceItem);
    pieceItem->setIcon(QIcon(pixmap));

    pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsDragEnabled);
    pieceItem->setData(Qt::UserRole, QVariant(pixmap));
    pieceItem->setData(Qt::UserRole+1, QVariant(hint));


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

    this->setMinimumWidth(200);

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

    QGroupBox *groubBox=new QGroupBox ("Picture search");
    groubBox->setLayout(l0);

    QVBoxLayout *l4 = new QVBoxLayout;
    l4->addWidget(groubBox);
    l4->setMargin(0);
    setLayout(l4);

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

    this->label     = new QLabel;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(this->label);
    l0->setMargin(0);
    setLayout(l0);
    setAcceptDrops(true);
    this->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    this->resize(1,1);
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
        this->pixmap = QPixmap(":/images/monkey_off.png");
        this->setPixmap(this->pixmap);

        return false;
    }

    this->setStatusTip(filePath);
    this->pixmap = QPixmap(this->filePath);
    this->setPixmap(this->pixmap);

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
   this->setPixmap(this->pixmap);
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

    QGroupBox  *groupBox =new QGroupBox ("Picture panel");
    groupBox->setLayout(l1);

    QVBoxLayout *l2 = new QVBoxLayout;
    l2->addWidget(groupBox);
    l2->setMargin(0);
    setLayout(l2);


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
   this->setMaximumWidth(this->imageButtonWidget[0][0]->height()*2);
   this->setMinimumWidth(this->imageButtonWidget[0][0]->height()*1);
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

    QGroupBox  *groupBox =new QGroupBox ("Course items");
    groupBox->setLayout(l0);

    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(groupBox);
    l1->setMargin(0);
    setLayout(l1);

    this->setMinimumWidth(200);

    connect(this->listWidget    ,SIGNAL(currentItemChanged( QListWidgetItem*,QListWidgetItem*))  , this, SLOT(itemActivatedSlot( QListWidgetItem*)));

}

/////////////////////////////////////////////////////////////////////////////
void cReadyCourseElementList::addItem (const QString &text,const QStringList &imgData,const QStringList &mp3Data)
{
    this->listWidget->insertItem(0,text);
    this->listWidget->item(0)->setData(Qt::UserRole     ,imgData);
    this->listWidget->item(0)->setData(Qt::UserRole+1   ,mp3Data);
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


    QStringList imgData = item->data(Qt::UserRole).toStringList();
    QStringList mp3Data = item->data(Qt::UserRole+1).toStringList();

    emit elementSelectedImgSignal (imgData);
    emit elementSelectedMP3Signal (mp3Data);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cMp3Widget::cMp3Widget (QWidget *parent)
    : QWidget (parent)
{
    label       = new QLabel;
    playButton  = new QPushButton ("Play");
    openButton  = new QPushButton ("Load");

    QHBoxLayout *l0 = new QHBoxLayout;
    l0->addWidget(label);
    l0->addStretch(1);
    l0->addWidget(playButton);
    l0->addWidget(openButton);
    l0->setMargin(0);
    setLayout(l0);

    this->audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
    this->mediaObject = new Phonon::MediaObject (this);

    connect(playButton    ,SIGNAL(clicked())  , this , SLOT(play()));
    connect(openButton    ,SIGNAL(clicked())  , this , SLOT(openFile()));
}

/////////////////////////////////////////////////////////////////////////////
void cMp3Widget::setData (const QString &label,const QString &path)
{
    this->label->setText(label);
    this->filePath = path;

    playButton->setStatusTip(path);
    openButton->setStatusTip(path);

    if (path.isEmpty())
    {
        playButton->setEnabled(false);
        openButton->setEnabled(false);
        return;
    }

    playButton->setEnabled(true);
    if (checkIsFileOk(path))
        playButton->setEnabled(true);
    else
        playButton->setEnabled(false);

}

/////////////////////////////////////////////////////////////////////////////
void cMp3Widget::play()
{
    Phonon::createPath(mediaObject, audioOutput);
    mediaObject->setCurrentSource(Phonon::MediaSource(filePath));
    mediaObject->play();
}

/////////////////////////////////////////////////////////////////////////////
void cMp3Widget::openFile()
{
    if (filePath.isEmpty())
        return;

    QFileDialog::Options opt = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;

    QString selectedFilter ="";
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Open mp3 file"),
                                getLastDir(),
                                tr("MP3 files (*.mp3);;All Files (*)"),
                                &selectedFilter,
                                opt);
    if (fileName.isEmpty())
        return;

    setLastDir(strippedDir(fileName));

    QFile::remove(filePath);          // remove old file
    QFile::copy(fileName,filePath);   //

    if (checkIsFileOk(filePath))
        playButton->setEnabled(true);
    else
        playButton->setEnabled(false);

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
cMp3TargetWidget::cMp3TargetWidget (QWidget *parent)
    : QWidget (parent)
{

    mp3Widget[0]    = new cMp3Widget;
    mp3Widget[1]    = new cMp3Widget;
    mp3Widget[2]    = new cMp3Widget;
    mp3Widget[3]    = new cMp3Widget;

    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(mp3Widget[0]);
    l0->addWidget(mp3Widget[1]);
    l0->addSpacing(10);
    l0->addWidget(mp3Widget[2]);
    l0->addWidget(mp3Widget[3]);

    QGroupBox  *groupBox =new QGroupBox ("Mp3 panel");
    groupBox->setLayout(l0);

    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(groupBox);
    l1->setMargin(0);
    setLayout(l1);
}

/////////////////////////////////////////////////////////////////////////////
void cMp3TargetWidget::setData (const QStringList &list)
{
    mp3Widget[0]->setData(QString("Q:")+list.at(3),list.at(1));
    mp3Widget[1]->setData(QString("A:")+list.at(2),list.at(0));
    mp3Widget[2]->setData(QString("Q:")+list.at(7),list.at(5));
    mp3Widget[3]->setData(QString("A:")+list.at(6),list.at(4));
}

/////////////////////////////////////////////////////////////////////////////
void cMp3TargetWidget::elementSelectedMp3Slot (const QStringList& mp3Data)
{
    setData(mp3Data);
}
