//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include "csmuxgenwidgets.h"
#include "coursetemplate.h"
#include "ccoursegenerator.h"

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QStringList &inputFileList);

private slots:
    void openCourseTemplateSlot();
    void saveCourseTemplateSlot();
    void saveAsCourseTemplateSlot();
    void generateCourseSlot();                  // start/stop generate course
    void generateCourseFinishedSlot();          // generate finished
    void progressSlot(const QString &);
    void contentChangedSlot();

    void closeSlot();
    void unlockInterface();
    void lockInterface();
    void about();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void createMenus();
    void createActions();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *toolBar;

    QAction *openCourseTemplateAct;
    QAction *saveCourseTemplateAct;
    QAction *saveAsCourseTemplateAct;
    QAction *generateCourseAct;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *quitAct;

    cOptionsPage *optionsPage;
    cConsolePage *consolePage;
    cContentPage *contentPage;

private:
    cCourseTemplate courseTemplate;
    void trace (const QString& txt,const unsigned int& flags);
    QString courseTemplateFileName;
    void setTitle();
    void generateStop();
    cCourseGenerator courseGenerator;
    bool contentChanged;
    bool batchMode;
   // bool isRunning;
    QStringList inputFileList;
    void generateCourseBatch();
};

#endif // GUI_H
