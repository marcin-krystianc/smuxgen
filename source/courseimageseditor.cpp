//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
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
CourseImageEditor::CourseImageEditor(QWidget *parent)
    : QWidget (parent)
{
    m_readyCourseElementList = new ReadyCourseElementList;
    m_imageTargetWidget = new ImageTargetWidget;
    m_imageSearch = new ImageSearch;
    m_mp3TargetWidget = new Mp3TargetWidget;

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    QWidget *tmpWidget = new QWidget;
    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(m_readyCourseElementList);
    l1->addWidget(m_mp3TargetWidget);
    l1->setMargin(0);
    tmpWidget->setLayout(l1);

    splitter->setChildrenCollapsible(false);
    splitter->addWidget(tmpWidget);
    splitter->addWidget(m_imageTargetWidget );
    splitter->addWidget(m_imageSearch);
    splitter->setHandleWidth(10);
    QHBoxLayout *l0 = new QHBoxLayout;
    l0->addWidget(splitter);

    setLayout(l0);

    connect( m_readyCourseElementList , SIGNAL(elementSelectedImgSignal(const QStringList&)),
             this , SLOT(elementSelectedImgSlot (const QStringList&)));

    connect( m_readyCourseElementList , SIGNAL(elementSelectedMP3Signal(const QStringList&)),
             m_mp3TargetWidget , SLOT(elementSelectedMp3Slot(const QStringList&)));

}

/////////////////////////////////////////////////////////////////////////////
void CourseImageEditor::clear()
{
    m_readyCourseElementList->clear();
}

/////////////////////////////////////////////////////////////////////////////
void CourseImageEditor::workWith (const CourseTemplate &courseTemplate)
{
    trace(QString("Course Browser::WorkWith course:")+courseTemplate.m_options.course+" lesson:"+courseTemplate.m_options.subname, traceLevel1);
    clear();

    if (!m_database.open(courseTemplate.m_options.database))
        return;

    int courseID;
    QString courseFileName, courseFileDirectoryName, mediaDirectoryName;
    if (!m_database.getCourseDetails (courseTemplate.m_options.course, &courseID, &courseFileName))
        return;

    QFileInfo courseFileInfo(courseFileName);
    courseFileDirectoryName = QDir::toNativeSeparators(courseFileInfo.dir().path())+QDir::separator();

    QString topicNameA = courseTemplate.m_options.subname;
    QString topicNameB = courseTemplate.m_options.subname+"*";

    int topicIDA, topicIDB;

    if (!m_database.getItemId(topicNameA, courseID, 0, &topicIDA))
        return;

    if (courseTemplate.m_options.bit.oDouble)
    {
        if (!m_database.getItemId(topicNameB, courseID, 0, &topicIDB))
            return;
    }

    mediaDirectoryName = courseFileDirectoryName+"media"+QDir::separator();

    for (int i = 0;i<courseTemplate.m_content.count();++i)
    {
        int id1, id2;
        QString line = (courseTemplate.m_content.at(i)).trimmed();
        if (line.length() == 0) continue;
        QStringList list1 = line.split(":");

        if (list1.count()< 2 )
        {
            trace(QString("cCourseImageEditor::workWith Input error: ")+line, traceWarning);
            continue;
        }

        if (!m_database.getItemId( getTextToPrint(list1.at(0)), courseID, topicIDA, &id1))
            continue;

        QString f1m = mediaDirectoryName+getMediaFileName(id1)+"m.jpg";
        QString f1n = mediaDirectoryName+getMediaFileName(id1)+"n.jpg";
        QString f1a = mediaDirectoryName+getMediaFileName(id1)+"a.mp3";
        QString f1q = mediaDirectoryName+getMediaFileName(id1)+"q.mp3";
        QString k1 = getKeyWord(list1.at(0));
        QString q1 = getTextToPrint(list1.at(0));
        QString a1 = getTextToPrint(list1.at(1));

        QString f2m;
        QString f2n;
        QString f2a;
        QString f2q;
        QString k2;
        QString q2;
        QString a2;

        if (courseTemplate.m_options.bit.oDouble)
        {
            if (!m_database.getItemId(getTextToPrint(list1.at(1)), courseID, topicIDB, &id2))
                continue;

            f2m = mediaDirectoryName+getMediaFileName(id2)+"m.jpg";
            f2n = mediaDirectoryName+getMediaFileName(id2)+"n.jpg";
            f2a = mediaDirectoryName+getMediaFileName(id2)+"a.mp3";
            f2q = mediaDirectoryName+getMediaFileName(id2)+"q.mp3";
            k2 = getKeyWord(list1.at(1));
            q2 = getTextToPrint(list1.at(1));
            a2 = getTextToPrint(list1.at(0));
        }

        QStringList imgList, mp3List;
        imgList<<k1<<f1m<<f1n<<k2<<f2m<<f2n;
        mp3List<<f1a<<f1q<<a1<<q1<<f2a<<f2q<<a2<<q2;
        m_readyCourseElementList->addItem(line, imgList, mp3List);
    }
}

/////////////////////////////////////////////////////////////////////////////
void CourseImageEditor::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////

void CourseImageEditor::elementSelectedImgSlot (const QStringList &imgData)
{
    if (!imgData.at(0).isEmpty()) // keyword1
        m_imageSearch->setNewKeywordsChangedL(imgData.at(0));

    if (!imgData.at(3).isEmpty()) // keyword2
        m_imageSearch->setNewKeywordsChangedR(imgData.at(3));

    QStringList list = imgData;
    list.removeAt(3); // keyword1
    list.removeAt(0); // keyword2
    m_imageTargetWidget->setFiles(list);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ImageList::ImageList(QWidget *parent, int maxCount )
    : QListWidget (parent)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(e_tileSizeX, e_tileSizeX));
    setSpacing(1);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    maxCount = maxCount;
    resetPosition();
    setMouseTracking(true);
}

/////////////////////////////////////////////////////////////////////////////
void ImageList::resetPosition()
{
    m_rowIndex = 0;
}

/////////////////////////////////////////////////////////////////////////////
void ImageList::addPiece(const QPixmap &pixmap, const QString &hint)
{
    if (count() >= m_maxCount)
        delete takeItem(count()-1);

    QListWidgetItem *pieceItem = new QListWidgetItem();
    pieceItem->setStatusTip(hint);

    if (!hint.isNull())
    {
        for (int i = 0;i<count();i++)
        {
            if(item(i)->data(Qt::UserRole+1).toString() == hint)
            {
                insertItem(m_rowIndex++, takeItem(i)); // move to the beginning of list
                return;
            }
        }
    }

    insertItem(m_rowIndex++, pieceItem);
    pieceItem->setIcon(QIcon(pixmap));

    pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsDragEnabled);
    pieceItem->setData(Qt::UserRole, QVariant(pixmap));
    pieceItem->setData(Qt::UserRole+1, QVariant(hint));


}

/////////////////////////////////////////////////////////////////////////////
void ImageList::addPieceSlot(const QPixmap &pixmap, const QString& hint)
{
    addPiece(pixmap, hint);
}
/////////////////////////////////////////////////////////////////////////////
void ImageList::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void ImageList::startDrag(Qt::DropActions /*supportedActions*/)
{
    QListWidgetItem *item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap = qVariantValue<QPixmap>(item->data(Qt::UserRole));

    dataStream << pixmap ;
    pixmap = pixmap.scaled(e_tileSizeX, e_tileSizeY);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(tileMimeFormat(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);

    drag->exec(Qt::MoveAction);

}

/////////////////////////////////////////////////////////////////////////////
void ImageList::setIconSizeSlot (int size)
{
    setIconSize(QSize(size, size));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ImageSearch::ImageSearch(QWidget *parent)
    : QWidget (parent)
{

    setMinimumWidth(200);

    m_leftEdit = new QLineEdit;
    m_rightEdit = new QLineEdit;
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(m_leftEdit);
    l1->addWidget(m_rightEdit);

    m_leftProgress = new QProgressBar;
    m_rightProgress = new QProgressBar;
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->addWidget(m_leftProgress);
    l2->addWidget(m_rightProgress);

    m_zoomSlider = new QSlider(Qt::Horizontal);
    m_zoomSlider->setMinimum(100);
    m_zoomSlider->setMaximum(500);
    QHBoxLayout *l3 = new QHBoxLayout;
    l3->addWidget(new QLabel("Zoom:"));
    l3->addWidget(m_zoomSlider, 1);

    m_imagelist = new ImageList();
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addLayout(l1);
    l0->addLayout(l2);
    l0->addLayout(l3);
    l0->addWidget(m_imagelist);

    QGroupBox *groubBox = new QGroupBox ("Picture search");
    groubBox->setLayout(l0);

    QVBoxLayout *l4 = new QVBoxLayout;
    l4->addWidget(groubBox);
    l4->setMargin(0);
    setLayout(l4);

    m_timerL = new QTimer;
    m_timerR = new QTimer;

    m_timerL->setSingleShot(true);
    m_timerR->setSingleShot(true);

    connect(m_leftEdit , SIGNAL(textChanged(const QString& )) , this , SLOT(newKeywordsChangedL(const QString&)));
    connect(m_rightEdit , SIGNAL(textChanged(const QString& )) , this , SLOT(newKeywordsChangedR(const QString&)));
    connect(m_timerL , SIGNAL(timeout()) , this, SLOT(newKeywordsL()));
    connect(m_timerR , SIGNAL(timeout()) , this, SLOT(newKeywordsR()));




    m_imageDownloader[0] = new ImageDownloader("L");
    m_imageDownloader[1] = new ImageDownloader("R");

    connect(m_imageDownloader[0] , SIGNAL(signalImage(const QPixmap&, const QString&)) , m_imagelist, SLOT(addPieceSlot(const QPixmap&, const QString&)));
    connect(m_imageDownloader[1] , SIGNAL(signalImage(const QPixmap&, const QString&)) , m_imagelist, SLOT(addPieceSlot(const QPixmap&, const QString&)));

    connect(m_imageDownloader[0] , SIGNAL(sProgressRange (int , int)) , m_leftProgress , SLOT(setRange (int , int)));
    connect(m_imageDownloader[1] , SIGNAL(sProgressRange (int , int)) , m_rightProgress , SLOT(setRange (int , int)));
    connect(m_imageDownloader[0] , SIGNAL(sProgressValue(int)) , m_leftProgress , SLOT(setValue(int)));
    connect(m_imageDownloader[1] , SIGNAL(sProgressValue(int)) , m_rightProgress , SLOT(setValue(int)));

    connect (m_zoomSlider , SIGNAL(valueChanged(int)) , m_imagelist , SLOT(setIconSizeSlot(int)));
    /*
 QHBoxLayout *textFiledsLayout = new QHBoxLayout;
 textFiledsLayout->addWidget(courseLabel);
 textFiledsLayout->addWidget(courseCombo);
 */

}
/////////////////////////////////////////////////////////////////////////////
ImageSearch::~ImageSearch()
{
    delete m_imageDownloader[0];
    delete m_imageDownloader[1];
}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::newKeywords (const QString &txt, int id)
{
    m_imagelist->resetPosition();
    trace(txt+QString::fromUtf8(" id:")+QString::number(id), traceLevel2);
    m_imageDownloader[id]->getImages(txt);
}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::newKeywordsL ()
{
    newKeywords(m_textL, 0);
}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::newKeywordsR ()
{
    newKeywords(m_textR, 1);
}
/////////////////////////////////////////////////////////////////////////////
void ImageSearch::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::newKeywordsChangedL (const QString &txt)
{
    m_timerL->stop();
    m_timerL->setInterval(200);
    m_timerL->start();
    m_textL = txt;
}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::newKeywordsChangedR (const QString &txt)
{
    m_timerR->stop();
    m_timerR->setInterval(200);
    m_timerR->start();
    m_textR = txt;

}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::setNewKeywordsChangedL (const QString &txt)
{
    m_leftEdit->setText(txt);
}

/////////////////////////////////////////////////////////////////////////////
void ImageSearch::setNewKeywordsChangedR (const QString &txt)
{
    m_rightEdit->setText(txt);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ImageButtonWidget::ImageButtonWidget(QWidget *parent)
    : QWidget (parent)
{

    m_label = new QLabel;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(m_label);
    l0->setMargin(0);
    setLayout(l0);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    resize(1, 1);
    setFile("");
}

/////////////////////////////////////////////////////////////////////////////
void ImageButtonWidget::setPixmap (const QPixmap &pixmap)
{
    m_label->setPixmap(pixmap.scaled(size()));
}

/////////////////////////////////////////////////////////////////////////////
bool ImageButtonWidget::setFile(const QString &path)
{
    m_filePath = path;
    QImage tmpImg;

    if (!tmpImg.load(m_filePath))
    {
        trace(QString("cImageButtonWidget::setFile cannot open file:")+m_filePath, traceError);
        setStatusTip(QString("Cannot open file:")+m_filePath);
        m_pixmap = QPixmap(":/images/monkey_off.png");
        setPixmap(m_pixmap);

        return false;
    }

    setStatusTip(m_filePath);
    m_pixmap = QPixmap(m_filePath);
    setPixmap(m_pixmap);

    return true;
}
/////////////////////////////////////////////////////////////////////////////
void ImageButtonWidget::trace(const QString &text, const int & flags)
{
    globalTracer.trace(text, flags);
}


/////////////////////////////////////////////////////////////////////////////
void ImageButtonWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(tileMimeFormat()))
        event->accept();
    else
        event->ignore();
}

/////////////////////////////////////////////////////////////////////////////
void ImageButtonWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(tileMimeFormat())) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else
        event->ignore();
}

/////////////////////////////////////////////////////////////////////////////
void ImageButtonWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(tileMimeFormat()))
    {
        QByteArray pieceData = event->mimeData()->data(tileMimeFormat());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;

        dataStream >> pixmap;

        if (!m_filePath.isEmpty())
        {
            if (pixmap.toImage().save(m_filePath))
                setPixmap(pixmap);
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else
        event->ignore();
}
/////////////////////////////////////////////////////////////////////////////
void ImageButtonWidget::resizeEvent(QResizeEvent *)
{
    setPixmap(m_pixmap);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ImageTargetWidget::ImageTargetWidget(QWidget *parent)
    : QWidget (parent)
{
    imageButtonWidget[0][0] = new ImageButtonWidget;
    imageButtonWidget[0][1] = new ImageButtonWidget;
    imageButtonWidget[1][0] = new ImageButtonWidget;
    imageButtonWidget[1][1] = new ImageButtonWidget;

    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(imageButtonWidget[0][0], 1);
    l1->addWidget(imageButtonWidget[0][1], 1);
    l1->addWidget(imageButtonWidget[1][0], 1);
    l1->addWidget(imageButtonWidget[1][1], 1);

    QGroupBox *groupBox = new QGroupBox ("Picture panel");
    groupBox->setLayout(l1);

    QVBoxLayout *l2 = new QVBoxLayout;
    l2->addWidget(groupBox);
    l2->setMargin(0);
    setLayout(l2);


}

/////////////////////////////////////////////////////////////////////////////
void ImageTargetWidget::setFiles (const QStringList &list)
{
    imageButtonWidget[0][0]->setFile(list.at(0));
    imageButtonWidget[0][1]->setFile(list.at(1));
    imageButtonWidget[1][0]->setFile(list.at(2));
    imageButtonWidget[1][1]->setFile(list.at(3));
}
/////////////////////////////////////////////////////////////////////////////
void ImageTargetWidget::resizeEvent (QResizeEvent*)
{
    setMaximumWidth(imageButtonWidget[0][0]->height()*2);
    setMinimumWidth(imageButtonWidget[0][0]->height()*1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ReadyCourseElementList::ReadyCourseElementList(QWidget *parent)
    : QWidget (parent)
{

    m_listWidget = new QListWidget;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(m_listWidget);

    QGroupBox *groupBox = new QGroupBox ("Course items");
    groupBox->setLayout(l0);

    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(groupBox);
    l1->setMargin(0);
    setLayout(l1);

    setMinimumWidth(200);

    connect(m_listWidget , SIGNAL(currentItemChanged( QListWidgetItem*, QListWidgetItem*)) , this, SLOT(itemActivatedSlot( QListWidgetItem*)));

}

/////////////////////////////////////////////////////////////////////////////
void ReadyCourseElementList::addItem (const QString &text, const QStringList &imgData, const QStringList &mp3Data)
{
    m_listWidget->insertItem(0, text);
    m_listWidget->item(0)->setData(Qt::UserRole , imgData);
    m_listWidget->item(0)->setData(Qt::UserRole+1 , mp3Data);
}

/////////////////////////////////////////////////////////////////////////////
void ReadyCourseElementList::clear ()
{
    m_listWidget->clear();
}

/////////////////////////////////////////////////////////////////////////////
void ReadyCourseElementList::itemActivatedSlot ( QListWidgetItem * item )
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
Mp3Widget::Mp3Widget (QWidget *parent)
    : QWidget (parent)
{
    m_label = new QLabel;
    m_playButton = new QPushButton ("Play");
    m_openButton = new QPushButton ("Load");

    QHBoxLayout *l0 = new QHBoxLayout;
    l0->addWidget(m_label);
    l0->addStretch(1);
    l0->addWidget(m_playButton);
    l0->addWidget(m_openButton);
    l0->setMargin(0);
    setLayout(l0);

    m_audioOutput = new Phonon::AudioOutput (Phonon::MusicCategory, this);
    m_mediaObject = new Phonon::MediaObject (this);

    connect(m_playButton , SIGNAL(clicked()) , this , SLOT(play()));
    connect(m_openButton , SIGNAL(clicked()) , this , SLOT(openFile()));
}

/////////////////////////////////////////////////////////////////////////////
void Mp3Widget::setData (const QString &label, const QString &path)
{
    m_label->setText(label);
    m_filePath = path;

    m_playButton->setStatusTip(path);
    m_openButton->setStatusTip(path);

    if (path.isEmpty())
    {
        m_playButton->setEnabled(false);
        m_openButton->setEnabled(false);
        return;
    }

    m_playButton->setEnabled(true);
    if (checkIsFileOk(path))
        m_playButton->setEnabled(true);
    else
        m_playButton->setEnabled(false);

}

/////////////////////////////////////////////////////////////////////////////
void Mp3Widget::play()
{
    Phonon::createPath(m_mediaObject, m_audioOutput);
    m_mediaObject->setCurrentSource(Phonon::MediaSource(m_filePath));
    m_mediaObject->play();
}

/////////////////////////////////////////////////////////////////////////////
void Mp3Widget::openFile()
{
    if (m_filePath.isEmpty())
        return;

    QFileDialog::Options opt = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;

    QString selectedFilter = "";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open mp3 file"),
                                                    getLastDir(),
                                                    tr("MP3 files (*.mp3);;All Files (*)"),
                                                    &selectedFilter,
                                                    opt);
    if (fileName.isEmpty())
        return;

    setLastDir(strippedDir(fileName));

    QFile::remove(m_filePath); // remove old file
    QFile::copy(fileName, m_filePath); //

    if (checkIsFileOk(m_filePath))
        m_playButton->setEnabled(true);
    else
        m_playButton->setEnabled(false);

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Mp3TargetWidget::Mp3TargetWidget (QWidget *parent)
    : QWidget (parent)
{

    m_mp3Widget[0] = new Mp3Widget;
    m_mp3Widget[1] = new Mp3Widget;
    m_mp3Widget[2] = new Mp3Widget;
    m_mp3Widget[3] = new Mp3Widget;

    QVBoxLayout *l0 = new QVBoxLayout;
    l0->addWidget(m_mp3Widget[0]);
    l0->addWidget(m_mp3Widget[1]);
    l0->addSpacing(10);
    l0->addWidget(m_mp3Widget[2]);
    l0->addWidget(m_mp3Widget[3]);

    QGroupBox *groupBox = new QGroupBox ("Mp3 panel");
    groupBox->setLayout(l0);

    QVBoxLayout *l1 = new QVBoxLayout;
    l1->addWidget(groupBox);
    l1->setMargin(0);
    setLayout(l1);
}

/////////////////////////////////////////////////////////////////////////////
void Mp3TargetWidget::setData (const QStringList &list)
{
    m_mp3Widget[0]->setData(QString("Q:")+list.at(3), list.at(1));
    m_mp3Widget[1]->setData(QString("A:")+list.at(2), list.at(0));
    m_mp3Widget[2]->setData(QString("Q:")+list.at(7), list.at(5));
    m_mp3Widget[3]->setData(QString("A:")+list.at(6), list.at(4));
}

/////////////////////////////////////////////////////////////////////////////
void Mp3TargetWidget::elementSelectedMp3Slot (const QStringList& mp3Data)
{
    setData(mp3Data);
}
