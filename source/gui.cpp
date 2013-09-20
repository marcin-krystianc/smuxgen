//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QtGui>

#include "gui.h"
#include "csmuxgenwidgets.h"
#include "coursetemplate.h"
#include "cglobaltracer.h"
#include "ccoursegenerator.h"
#include "globalsmuxgentools.h"

/////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow()
{
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    m_contentChanged = false;
    setTitle();
    unlockInterface();
    resize(800, 600);
    setWindowIcon(QIcon(":/images/smuxgen.png"));

    connect(&m_courseGenerator, SIGNAL(finished()) , this , SLOT(generateCourseFinishedSlot()));
    connect(&m_courseGenerator, SIGNAL(progressSignal(const QString&)) , this , SLOT(progressSlot(const QString&)));
}

/////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::about()
{
    QMessageBox::about(this, "SMUXGEN",
                       "<center><b> SMUXGEN </b> - SuperMemo UX generator</center>"
                       "<center><br/>License : GPL"
                       "<center><br/><a href = \"http://code.google.com/p/smuxgen\">http://code.google.com/p/smuxgen</a> ");
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    m_fileMenu->addAction(m_openCourseTemplateAction);
    m_fileMenu->addAction(m_saveCourseTemplateAction);
    m_fileMenu->addAction(m_saveAsCourseTemplateAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_importQAAction);
    m_fileMenu->addAction(m_exportQAAction);

    m_fileMenu->addSeparator();

    m_recentMenu = m_fileMenu->addMenu("&Recent");
    for (int i = 0; i < m_recentFileActions.size(); ++i)
        m_recentMenu->addAction(m_recentFileActions[i]);

    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_generateCourseAction);
    m_fileMenu->addAction(m_courseBrowserAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addSeparator();

    m_fileMenu->addAction(m_quitAction);

    m_viewMenu = menuBar()->addMenu(tr("&View"));

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
    m_helpMenu->addAction(m_aboutQtAction);

    updateRecentFileActions();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createActions()
{

    m_openCourseTemplateAction = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    m_openCourseTemplateAction->setShortcuts(QKeySequence::Open);
    m_openCourseTemplateAction->setStatusTip(tr("Open..."));
    connect(m_openCourseTemplateAction, SIGNAL(triggered()), this, SLOT(openCourseTemplateSlot()));

    m_saveCourseTemplateAction = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    m_saveCourseTemplateAction->setShortcuts(QKeySequence::Save);
    m_saveCourseTemplateAction->setStatusTip(tr("Save"));
    connect(m_saveCourseTemplateAction, SIGNAL(triggered()), this, SLOT(saveCourseTemplateSlot()));

    m_importQAAction = new QAction(QIcon(":/images/redo.png"), tr("&Import Q&A"), this);
    m_importQAAction->setShortcuts(QKeySequence::UnknownKey);
    m_importQAAction->setStatusTip(tr("Import from Q&A"));
    connect(m_importQAAction, SIGNAL(triggered()), this, SLOT(importQASlot()));

    m_exportQAAction = new QAction(QIcon(":/images/undo.png"), tr("&Export to Q&A"), this);
    m_exportQAAction->setShortcuts(QKeySequence::UnknownKey);
    m_exportQAAction->setStatusTip(tr("Export to Q&A"));
    connect(m_exportQAAction, SIGNAL(triggered()), this, SLOT(exportQASlot()));

    m_saveAsCourseTemplateAction = new QAction(QIcon(":/images/save.png"), tr("&Save As..."), this);
    m_saveAsCourseTemplateAction->setShortcuts(QKeySequence::SaveAs);
    m_saveAsCourseTemplateAction->setStatusTip(tr("Save as..."));
    connect(m_saveAsCourseTemplateAction, SIGNAL(triggered()), this, SLOT(saveAsCourseTemplateSlot()));

    m_generateCourseAction = new QAction(this);
    m_generateCourseAction->setShortcuts(QKeySequence::UnknownKey);
    connect(m_generateCourseAction, SIGNAL(triggered()), this, SLOT(generateCourseSlot()));

    m_courseBrowserAction = new QAction(QIcon(":/images/imgedit.png"), tr("&Course browser"), this);
    m_courseBrowserAction->setShortcuts(QKeySequence::UnknownKey);
    m_courseBrowserAction->setStatusTip(tr("Course browser"));
    connect(m_courseBrowserAction, SIGNAL(triggered()), this, SLOT(courseBrowserOpenCloseSlot()));

    m_quitAction = new QAction(tr("&Quit"), this);
    m_quitAction->setShortcut(tr("Ctrl+Q"));
    m_quitAction->setStatusTip(tr("Quit the application"));
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(closeSlot()));

    m_aboutAction = new QAction(tr("&About"), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAction = new QAction(tr("About &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_recentFileActions.push_back(new QAction(this));
        m_recentFileActions[i]->setVisible(false);
        connect(m_recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createToolBars()
{
    m_toolBar = new QToolBar;
    m_toolBar = addToolBar(tr("Toolbar"));

    m_toolBar->addAction(m_openCourseTemplateAction);
    m_toolBar->addAction(m_saveCourseTemplateAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_importQAAction);
    m_toolBar->addAction(m_exportQAAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_generateCourseAction);
    m_toolBar->addAction(m_courseBrowserAction);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::createDockWindows()
{
    m_dockOptionsPage = new QDockWidget(tr("Options"), this);
    m_dockOptionsPage->setAllowedAreas( Qt::LeftDockWidgetArea );
    addDockWidget(Qt::LeftDockWidgetArea, m_dockOptionsPage);
    m_optionsPage = new cOptionsPage(m_dockOptionsPage);
    m_dockOptionsPage->setWidget(m_optionsPage);
    m_viewMenu->addAction(m_dockOptionsPage->toggleViewAction());

    m_dockContentPage = new QDockWidget(tr("Word list"), this);
    m_dockContentPage->setAllowedAreas(Qt::RightDockWidgetArea );
    addDockWidget(Qt::RightDockWidgetArea, m_dockContentPage);
    m_contentPage = new cContentPage(m_dockContentPage);
    m_dockContentPage->setWidget(m_contentPage);
    m_viewMenu->addAction(m_dockContentPage->toggleViewAction());

    m_dockConsolePage = new QDockWidget(tr("Console"), this);
    m_dockConsolePage->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_dockConsolePage);
    m_consolePage = new cConsolePage(m_dockConsolePage);
    m_dockConsolePage->setWidget(m_consolePage);
    m_viewMenu->addAction(m_dockConsolePage->toggleViewAction());
    m_dockConsolePage->hide();

    m_dockCourseBrowser = new QDockWidget(tr("Course browser"), this);
    m_dockCourseBrowser->setAllowedAreas(Qt::TopDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, m_dockCourseBrowser);
    m_imageWidget = new cCourseImageEditor(m_dockCourseBrowser);
    m_dockCourseBrowser->setWidget(m_imageWidget);
    m_dockCourseBrowser->hide();
    connect( m_dockCourseBrowser , SIGNAL(visibilityChanged(bool )), this, SLOT(courseBrowserVisibleSlot(bool)));


    connect(&globalTracer, SIGNAL(traceSignal(const QString &, const int&)), m_consolePage , SLOT(traceSlot(const QString&, const int&)));
    connect( m_contentPage , SIGNAL(contentChangedSignal()), this, SLOT(contentChangedSlot()));
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

    }

    setLastDir(strippedDir(fileName));

    if (!m_courseTemplate.open(fileName))
    {
        trace (tr("Error opening file:")+fileName, traceError);
        return;
    }

    m_courseTemplateFileName = fileName;
    trace (QString("Opened: ")+m_courseTemplateFileName, traceLevel1);

    m_optionsPage->setOptions(m_courseTemplate.options);
    m_contentPage->setContent(m_courseTemplate.content);

    QSettings settings("Smuxgen", "Smuxgen");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentFileList", files);
    updateRecentFileActions();

    m_contentChanged = false;
    setTitle();
}

/////////////////////////////////////////////////////////////////////////////
bool MainWindow::saveCourseTemplateSlot()
{
    if (m_courseTemplateFileName.isEmpty())
        return saveAsCourseTemplateSlot();

    QFile::remove(m_courseTemplateFileName+"~");
    QFile::copy(m_courseTemplateFileName, m_courseTemplateFileName+"~");

    m_contentChanged = false;

    m_courseTemplate.options = m_optionsPage->getOptions();
    m_courseTemplate.content = m_contentPage->getContent();
    m_courseTemplate.save(m_courseTemplateFileName);
    trace (QString("Saved: ")+m_courseTemplateFileName, traceLevel1);
    setTitle();

    return true;
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
    m_courseTemplate.content = m_contentPage->getContent();
    if (!m_courseTemplate.importQA(fileName)) {
        trace (tr("Error importing Q&A file:")+fileName, traceError);
        return;
    }
    m_contentPage->setContent(m_courseTemplate.content);
    trace (QString::fromUtf8("Imported Q&A:")+fileName, traceLevel1);
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

    m_courseTemplate.content = m_contentPage->getContent();
    if (!m_courseTemplate.exportQA(fileName)) {
        trace (tr("Error exporting Q&A file:")+fileName, traceError);
        return;
    }
    trace (QString::fromUtf8("Exported Q&A:")+fileName, traceLevel1);
}

/////////////////////////////////////////////////////////////////////////////
bool MainWindow::saveAsCourseTemplateSlot()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save file"),
                                                    getLastDir(),
                                                    tr("Smuxgen files (*.smuxgen);;All Files (*)"));
    if (fileName.isEmpty())
        return false;

    setLastDir(strippedDir(fileName));

    QSettings settings("Smuxgen", "Smuxgen");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentFileList", files);
    updateRecentFileActions();

    m_courseTemplateFileName = fileName;
    setTitle();
    saveCourseTemplateSlot();
    return true;
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateCourseSlot()
{
    if (m_courseGenerator.isRunning()) {
        return generateStop();
    }

    m_courseTemplate.options = m_optionsPage->getOptions();
    m_courseTemplate.content = m_contentPage->getContent();
    m_courseGenerator.generate(m_courseTemplate);
    lockInterface();
    setTitle();
    trace (QString("Started to generate: ")+m_courseTemplate.options.subname, traceLevel1);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::courseBrowserOpenCloseSlot()
{
    if (m_dockCourseBrowser->isVisible())
        m_dockCourseBrowser->hide();
    else
        m_dockCourseBrowser->show();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::courseBrowserVisibleSlot(bool visible)
{
    if (!visible) {
        m_generateCourseAction->setEnabled(true);
        m_dockContentPage->show();
        m_dockOptionsPage->show();
        m_dockContentPage->toggleViewAction()->setEnabled(true);
        m_dockOptionsPage->toggleViewAction()->setEnabled(true);
    }
    else {
        m_courseTemplate.options = m_optionsPage->getOptions();
        m_courseTemplate.content = m_contentPage->getContent();
        m_imageWidget->workWith(m_courseTemplate);

        m_generateCourseAction->setEnabled(false);
        m_dockContentPage->hide();
        m_dockOptionsPage->hide();
        m_dockContentPage->toggleViewAction()->setEnabled(false);
        m_dockOptionsPage->toggleViewAction()->setEnabled(false);
    }
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::setTitle()
{
    QFileInfo fileInfo(m_courseTemplateFileName);

    QString title = "SMUXGEN: ";
    if (m_contentChanged)
        title += QString("*");

    title += fileInfo.fileName()+QString(" ");

    if (m_courseGenerator.isRunning())
        title += QString("[Running]");

    setWindowTitle(title);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::unlockInterface()
{
    m_courseBrowserAction->setEnabled(true);
    m_generateCourseAction->setIcon(QIcon(":/images/generate.png"));
    m_generateCourseAction->setStatusTip(tr("Generate course"));
    m_generateCourseAction->setText(tr("Generate "));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::lockInterface()
{
    m_courseBrowserAction->setEnabled(false);
    m_generateCourseAction->setIcon(QIcon(":/images/stop.png"));
    m_generateCourseAction->setStatusTip(tr("Stop"));
    m_generateCourseAction->setText(tr("Stop "));
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateCourseFinishedSlot()
{
    if (m_courseGenerator.isFailed())
        trace (QString("Course generation failed !"), traceLevel1);
    else
        trace (QString("Course generatad successfully"), traceLevel1);

    unlockInterface();
    statusBar()->showMessage("");

    setTitle();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::progressSlot(const QString &progress)
{
    statusBar()->showMessage(progress);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::contentChangedSlot()
{
    m_contentChanged = true;
    setTitle();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::closeSlot ()
{
    close();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_courseGenerator.isRunning()) {
        event->ignore();
        return generateStop();
    }

    if (m_contentChanged) {
        int q = QMessageBox::question(0, "Save changes", "Do You want to save changes ?", "Yes", "No", "Cancel");

        switch (q)
        {
        case 0:
            if (!saveCourseTemplateSlot())
                event->ignore();
            break;
        case 1:
            break;
        case 2:
        default:
            event->ignore();
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::generateStop()
{
    trace (QString("Stopping "), traceLevel1);
    m_courseGenerator.stop();
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::trace (const QString& text, const unsigned int& flags = traceLevel1)
{
    globalTracer.trace(text, flags);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::updateRecentFileActions()
{
    QSettings settings("Smuxgen", "Smuxgen");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), m_recentFileActions.size());

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedFile(files[i]));
        m_recentFileActions[i]->setText(text);
        m_recentFileActions[i]->setData(files[i]);
        m_recentFileActions[i]->setStatusTip(files[i]);
        m_recentFileActions[i]->setVisible(true);
    }

    for (int i = numRecentFiles; i < m_recentFileActions.size(); ++i)
        m_recentFileActions[i]->setVisible(false);
}

/////////////////////////////////////////////////////////////////////////////
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openCourseTemplateSlot(action->data().toString());
}
