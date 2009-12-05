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
    fileMenu = menuBar()->addMenu(tr("&File"));
    if (!this->batchMode)
    {
        fileMenu->addAction(openCourseTemplateAct);
        fileMenu->addAction(saveCourseTemplateAct);
        fileMenu->addAction(saveAsCourseTemplateAct);

        fileMenu->addSeparator();
        fileMenu->addAction(generateCourseAct);
        fileMenu->addSeparator();
        fileMenu->addSeparator();
    }
    fileMenu->addAction(quitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

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

    saveAsCourseTemplateAct = new QAction(QIcon(":/images/save.png"), tr("&Save As..."), this);
    saveAsCourseTemplateAct->setShortcuts(QKeySequence::SaveAs);
    saveAsCourseTemplateAct->setStatusTip(tr("Save as..."));
    connect(saveAsCourseTemplateAct, SIGNAL(triggered()), this, SLOT(saveAsCourseTemplateSlot()));

    generateCourseAct = new QAction(this);
    generateCourseAct->setShortcuts(QKeySequence::UnknownKey);
    connect(generateCourseAct, SIGNAL(triggered()), this, SLOT(generateCourseSlot()));

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
    }
    this->toolBar->addAction(generateCourseAct);
}
/////////////////////////////////////////////////////////////////////////////
void MainWindow::createDockWindows()
{
    QDockWidget *dock;

    dock = new QDockWidget(tr("Parameters"), this);
    dock->setAllowedAreas( Qt::LeftDockWidgetArea );
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    this->optionsPage =new cOptionsPage(dock);
    dock->setWidget(this->optionsPage);

    if (!this->batchMode)
        viewMenu->addAction(dock->toggleViewAction());
    else
        dock->hide();

    dock = new QDockWidget(tr("Course template"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea );
    addDockWidget(Qt::RightDockWidgetArea, dock);
    this->contentPage = new cContentPage(dock);
    dock->setWidget(this->contentPage);

    if (!this->batchMode)
        viewMenu->addAction(dock->toggleViewAction());
    else
        dock->hide();


    dock = new QDockWidget(tr("Console"), this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    this->consolePage = new cConsolePage(dock);
    dock->setWidget(this->consolePage);
    viewMenu->addAction(dock->toggleViewAction());

    if (!this->batchMode)
        dock->hide();

    connect(&globalTracer, SIGNAL(traceSignal(const QString &,const int&)), this->consolePage , SLOT(traceSlot(const QString&,const int&)));
    connect( this->contentPage , SIGNAL(contentChangedSignal()),this, SLOT(contentChangedSlot()));

}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::openCourseTemplateSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Open file"),
                                "",
                                tr("Smuxgen files (*.smuxgen);;All Files (*)"));
    if (fileName.isEmpty())
        return;



    if (!courseTemplate.open(fileName))
    {
        trace (tr("Error opening file:")+fileName,traceError);
        return;
    }

    this->courseTemplateFileName    = fileName;
    trace (QString("Opened: ")+this->courseTemplateFileName,traceLevel1);

    this->optionsPage->setOptions(courseTemplate.options);
    this->contentPage->setContent(courseTemplate.content);


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
void MainWindow::saveAsCourseTemplateSlot()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save file"),
                                "",
                                tr("Smuxgen files (*.smuxgen);;All Files (*)"));
    if (fileName.isEmpty())
        return;

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
    this->generateCourseAct->setIcon(QIcon(":/images/generate.png"));
    this->generateCourseAct->setStatusTip(tr("Generate course"));
    this->generateCourseAct->setText(tr("Generate "));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::lockInterface()
{
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
