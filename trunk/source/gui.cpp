//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QtGui>

#include "gui.h"
#include "csmuxgenwidgets.h"
#include "coursetemplate.h"
#include "cglobaltracer.h"
#include "ccoursegenerator.h"
#include "globalsmuxgentools.h"

/////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QStringList &inputFileList)
{
    this->batchMode     = !inputFileList.isEmpty();
    this->inputFileList = inputFileList;

    this->createActions();
    this->createMenus();
    this->createToolBars();
    this->createStatusBar();
    this->createDockWindows();

    this->contentChanged = false;
    this->setTitle();
    this->unlockInterface();
    this->resize(800,600);
    this->setWindowIcon(QIcon(":/images/smuxgen.png"));

    connect(&courseGenerator, SIGNAL(finished())                        , this , SLOT(generateCourseFinishedSlot()));
    connect(&courseGenerator, SIGNAL(progressSignal(const QString&))    , this , SLOT(progressSlot(const QString&)));

    if (this->batchMode)
        this->generateCourseBatch();
}
/////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::about()
{
    QMessageBox::about(this, QString("SMUXGEN"),
                       QString   (  "<center><b> SMUXGEN </b> - SuperMemo UX generator</center>"
                                    "<center><br/>License : GPL"
                                    "<center><br/><a href=\"http://code.google.com/p/smuxgen\">http://code.google.com/p/smuxgen</a> "));
}
/////////////////////////////////////////////////////////////////////////////
void MainWindow::createMenus()
{
    fileMenu    = menuBar()->addMenu(tr("&File"));

    if (!this->batchMode)
    {
        fileMenu->addAction(openCourseTemplateAct);
        fileMenu->addAction(saveCourseTemplateAct);
        fileMenu->addAction(saveAsCourseTemplateAct);
        fileMenu->addSeparator();
        fileMenu->addAction(importQAAct);
        fileMenu->addAction(exportQAAct);

        fileMenu->addSeparator();
        recentMenu  = fileMenu->addMenu("&Recent");
        for (int i = 0; i < MaxRecentFiles; ++i)
            recentMenu->addAction(recentFileActs[i]);

        fileMenu->addSeparator();
        fileMenu->addAction(generateCourseAct);
        fileMenu->addAction(courseBrowserAct);
        fileMenu->addSeparator();
        fileMenu->addSeparator();
    }
    fileMenu->addAction(quitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    this->updateRecentFileActions();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createActions()
{

    openCourseTemplateAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openCourseTemplateAct->setShortcuts(QKeySequence::Open);
    openCourseTemplateAct->setStatusTip(tr("Open..."));
    connect(openCourseTemplateAct, SIGNAL(triggered()), this, SLOT(openCourseTemplateSlot()));

    saveCourseTemplateAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveCourseTemplateAct->setShortcuts(QKeySequence::Save);
    saveCourseTemplateAct->setStatusTip(tr("Save"));
    connect(saveCourseTemplateAct, SIGNAL(triggered()), this, SLOT(saveCourseTemplateSlot()));

    importQAAct = new QAction(QIcon(":/images/redo.png"), tr("&Import Q&A"), this);
    importQAAct->setShortcuts(QKeySequence::UnknownKey);
    importQAAct->setStatusTip(tr("Import from Q&A"));
    connect(importQAAct, SIGNAL(triggered()), this, SLOT(importQASlot()));

    exportQAAct = new QAction(QIcon(":/images/undo.png"), tr("&Export to Q&A"), this);
    exportQAAct->setShortcuts(QKeySequence::UnknownKey);
    exportQAAct->setStatusTip(tr("Export to Q&A"));
    connect(exportQAAct, SIGNAL(triggered()), this, SLOT(exportQASlot()));

    saveAsCourseTemplateAct = new QAction(QIcon(":/images/save.png"), tr("&Save As..."), this);
    saveAsCourseTemplateAct->setShortcuts(QKeySequence::SaveAs);
    saveAsCourseTemplateAct->setStatusTip(tr("Save as..."));
    connect(saveAsCourseTemplateAct, SIGNAL(triggered()), this, SLOT(saveAsCourseTemplateSlot()));

    generateCourseAct = new QAction(this);
    generateCourseAct->setShortcuts(QKeySequence::UnknownKey);
    connect(generateCourseAct, SIGNAL(triggered()), this, SLOT(generateCourseSlot()));

    courseBrowserAct = new QAction(QIcon(":/images/imgedit.png"), tr("&Course browser"), this);
    courseBrowserAct->setShortcuts(QKeySequence::UnknownKey);
    courseBrowserAct->setStatusTip(tr("Course browser"));
    connect(courseBrowserAct, SIGNAL(triggered()), this, SLOT(courseBrowserOpenCloseSlot()));
    if (this->batchMode)
        this->courseBrowserAct->setVisible(false);

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(closeSlot()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createToolBars()
{
    this->toolBar = new QToolBar;
    this->toolBar = addToolBar(tr("Toolbar"));
    if (!this->batchMode)
    {
        this->toolBar->addAction(openCourseTemplateAct);
        this->toolBar->addAction(saveCourseTemplateAct);
        this->toolBar->addSeparator();
        this->toolBar->addAction(importQAAct);
        this->toolBar->addAction(exportQAAct);
        this->toolBar->addSeparator();
    }
    this->toolBar->addAction(generateCourseAct);
    this->toolBar->addAction(courseBrowserAct);
}
/////////////////////////////////////////////////////////////////////////////
void MainWindow::createDockWindows()
{
    //QDockWidget *dock;

    this->dockOptionsPage = new QDockWidget(tr("Options"), this);
    this->dockOptionsPage->setAllowedAreas( Qt::LeftDockWidgetArea );
    this->addDockWidget(Qt::LeftDockWidgetArea, this->dockOptionsPage);
    this->optionsPage =new cOptionsPage(this->dockOptionsPage);
    this->dockOptionsPage->setWidget(this->optionsPage);

    if (!this->batchMode)
        viewMenu->addAction(this->dockOptionsPage->toggleViewAction());
    else
        this->dockOptionsPage->hide();

    this->dockContentPage = new QDockWidget(tr("Word list"), this);
    this->dockContentPage->setAllowedAreas(Qt::RightDockWidgetArea );
    addDockWidget(Qt::RightDockWidgetArea, this->dockContentPage);
    this->contentPage = new cContentPage(this->dockContentPage);
    this->dockContentPage->setWidget(this->contentPage);


    if (!this->batchMode)
        viewMenu->addAction(this->dockContentPage->toggleViewAction());
    else
        this->dockContentPage->hide();


    this->dockConsolePage = new QDockWidget(tr("Console"), this);
    this->dockConsolePage->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, this->dockConsolePage);
    this->consolePage = new cConsolePage(this->dockConsolePage);
    this->dockConsolePage->setWidget(this->consolePage);
    viewMenu->addAction(this->dockConsolePage->toggleViewAction());


    if (!this->batchMode)
        this->dockConsolePage->hide();


    this->dockCourseBrowser = new QDockWidget(tr("Course browser"), this);
    this->dockCourseBrowser->setAllowedAreas(Qt::TopDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, this->dockCourseBrowser);
    this->imageWidget = new cCourseImageEditor(this->dockCourseBrowser);
    this->dockCourseBrowser->setWidget(this->imageWidget);
    this->dockCourseBrowser->hide();
    connect( this->dockCourseBrowser , SIGNAL(visibilityChanged(bool )),this, SLOT(courseBrowserVisibleSlot(bool)));


    connect(&globalTracer, SIGNAL(traceSignal(const QString &,const int&)), this->consolePage , SLOT(traceSlot(const QString&,const int&)));
    connect( this->contentPage , SIGNAL(contentChangedSignal()),this, SLOT(contentChangedSlot()));

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::openCourseTemplateSlot(QString fileName)
{
    if (fileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open file"),
                                    getLastDir(),
                                    tr("Smuxgen files (*.smuxgen);;All Files (*)"));
        if (fileName.isEmpty())
            return;

        setLastDir(strippedDir(fileName));
    }

    if (!courseTemplate.open(fileName))
    {
        trace (tr("Error opening file:")+fileName,traceError);
        return;
    }

    this->courseTemplateFileName    = fileName;
    trace (QString("Opened: ")+this->courseTemplateFileName,traceLevel1);

    this->optionsPage->setOptions(courseTemplate.options);
    this->contentPage->setContent(courseTemplate.content);

    QSettings settings("Smuxgen", "Smuxgen");
        QStringList files = settings.value("recentFileList").toStringList();
        files.removeAll(fileName);
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
            files.removeLast();
    settings.setValue("recentFileList", files);
    this->updateRecentFileActions();

    this->contentChanged = false;
    this->setTitle();

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::saveCourseTemplateSlot()
{
    if (this->courseTemplateFileName.isEmpty())
        return this->saveAsCourseTemplateSlot();

    QFile::remove(this->courseTemplateFileName+"~");
    QFile::copy(this->courseTemplateFileName,this->courseTemplateFileName+"~");

    this->contentChanged = false;

    this->courseTemplate.options = this->optionsPage->getOptions();
    this->courseTemplate.content = this->contentPage->getContent();
    this->courseTemplate.save(this->courseTemplateFileName);
    trace (QString("Saved: ")+this->courseTemplateFileName,traceLevel1);
    this->setTitle();

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::importQASlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Open file"),
                                getLastDir(),
                                tr("Q&A files (*)"));
    if (fileName.isEmpty())
        return;

    setLastDir(strippedDir(fileName));
    this->courseTemplate.content = this->contentPage->getContent();
    if (!courseTemplate.importQA(fileName)) {
        trace (tr("Error importing Q&A file:")+fileName,traceError);
        return;
    }
    this->contentPage->setContent(courseTemplate.content);
    trace (QString::fromUtf8("Imported Q&A:")+fileName,traceLevel1);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::exportQASlot()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save file"),
                                getLastDir(),
                                tr("Q&A files (*)"));
    if (fileName.isEmpty())
        return;

    setLastDir(strippedDir(fileName));

    this->courseTemplate.content = this->contentPage->getContent();
    if (!courseTemplate.exportQA(fileName)) {
        trace (tr("Error exporting Q&A file:")+fileName,traceError);
        return;
    }
    trace (QString::fromUtf8("Exported Q&A:")+fileName,traceLevel1);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::saveAsCourseTemplateSlot()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save file"),
                                getLastDir(),
                                tr("Smuxgen files (*.smuxgen);;All Files (*)"));
    if (fileName.isEmpty())
        return;

    setLastDir(strippedDir(fileName));
    this->courseTemplateFileName = fileName;
    this->setTitle();
;
    this->saveCourseTemplateSlot();
}
/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateCourseSlot()
{
    if (this->courseGenerator.isRunning()) {
        this->inputFileList.clear();
        return this ->generateStop();
    }

    if (this->batchMode)
        return;


    this->courseTemplate.options = this->optionsPage->getOptions();
    this->courseTemplate.content = this->contentPage->getContent();
    this->courseGenerator.generate(this->courseTemplate);
    this->lockInterface();
    this->setTitle();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::courseBrowserOpenCloseSlot()
{
    if (this->dockCourseBrowser->isVisible())
        this->dockCourseBrowser->hide();
    else
        this->dockCourseBrowser->show();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::courseBrowserVisibleSlot(bool visible)
{
    if (!visible)
    {   
        this->generateCourseAct->setEnabled(true);
        this->dockContentPage->show();
        this->dockOptionsPage->show();
        this->dockContentPage->toggleViewAction()->setEnabled(true);
        this->dockOptionsPage->toggleViewAction()->setEnabled(true);

    }
    else
    {
        this->courseTemplate.options = this->optionsPage->getOptions();
        this->courseTemplate.content = this->contentPage->getContent();
        this->imageWidget->workWith(this->courseTemplate);

        this->generateCourseAct->setEnabled(false);
        this->dockContentPage->hide();
        this->dockOptionsPage->hide();
        this->dockContentPage->toggleViewAction()->setEnabled(false);
        this->dockOptionsPage->toggleViewAction()->setEnabled(false);
    }
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::setTitle()
{
    QFileInfo fileInfo(this->courseTemplateFileName);

    QString title = "SMUXGEN: ";
    if (this->contentChanged)
        title += QString("*");

    title += fileInfo.fileName()+QString(" ");

    if (this->courseGenerator.isRunning())
        title += QString("[Running]");

    setWindowTitle(title);

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::unlockInterface()
{
    this->courseBrowserAct->setEnabled(true);
    this->generateCourseAct->setIcon(QIcon(":/images/generate.png"));
    this->generateCourseAct->setStatusTip(tr("Generate course"));
    this->generateCourseAct->setText(tr("Generate "));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::lockInterface()
{
    this->courseBrowserAct->setEnabled(false);
    this->generateCourseAct->setIcon(QIcon(":/images/stop.png"));
    this->generateCourseAct->setStatusTip(tr("Stop"));
    this->generateCourseAct->setText(tr("Stop "));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateCourseFinishedSlot()
{
    if (this->courseGenerator.getStatus())
         trace (QString("Course generation failed !"),traceLevel1);
    else   
        trace (QString("Course generatad successfully"),traceLevel1);

    this->unlockInterface();
    statusBar()->showMessage("");

    this->setTitle();
    if (this->batchMode)
        this->generateCourseBatch();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::progressSlot(const QString &progress)
{
    statusBar()->showMessage(progress);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::contentChangedSlot()
{
    this->contentChanged = true;
    this->setTitle();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::closeSlot ()
{
    close();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (this->courseGenerator.isRunning())
    {
        event->ignore();
        return generateStop();
    }

    if (this->contentChanged)
        this->courseTemplateFileName.clear(),this->saveCourseTemplateSlot();

    close();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateStop()
{
    trace (QString("Stopping "),traceLevel1);
    this->courseGenerator.stop();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateCourseBatch()
{
    if (this->inputFileList.count()==0)
    {
        this->courseTemplateFileName = "";
        this->setTitle();
        return;  // nothing to do
    }

    this->courseTemplateFileName = this->inputFileList.at(0);

    if (!this->courseTemplate.open(this->courseTemplateFileName))
    {   // file error - stop batch mode
        this->courseTemplateFileName = "";
        this->setTitle();
        return;  // nothing to do
    }


    trace (QString("Opened: ")+this->courseTemplateFileName,traceLevel1);

    this->inputFileList.removeFirst();
    this->courseGenerator.generate(this->courseTemplate);
    trace (QString("generate started: "),traceLevel3);
    this->lockInterface();
    this->setTitle();
}
/////////////////////////////////////////////////////////////////////////////
void MainWindow::trace (const QString& text,const unsigned int& flags= traceLevel1)
{
    globalTracer.trace(text,flags);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::updateRecentFileActions()
{
    QSettings settings("Smuxgen", "Smuxgen");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedFile(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setStatusTip(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        this->openCourseTemplateSlot(action->data().toString());
}
