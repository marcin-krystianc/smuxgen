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
#include "courseimageseditor.h"

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
    MainWindow();
    ~MainWindow();

private slots:
    void openCourseTemplateSlot(QString filename="");
    bool saveCourseTemplateSlot();
    void importQASlot();
    void exportQASlot();
    bool saveAsCourseTemplateSlot();
    void generateCourseSlot();                      // start/stop generate course
    void courseBrowserOpenCloseSlot();                  // start/stop picture browser
    void courseBrowserVisibleSlot(bool visible);   // picture browser

    void generateCourseFinishedSlot();          // generate finished
    void progressSlot(const QString &);
    void contentChangedSlot();

    void closeSlot();
    void unlockInterface();
    void lockInterface();
    void about();

    void openRecentFile();
protected:
    void closeEvent(QCloseEvent *event);

private:
    void createMenus();
    void createActions();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void updateRecentFileActions();

    QMenu *fileMenu;
    QMenu *recentMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *toolBar;

    static const int MaxRecentFiles = 15;

    QAction *openCourseTemplateAct;
    QAction *saveCourseTemplateAct;
    QAction *importQAAct;
    QAction *exportQAAct;
    QAction *saveAsCourseTemplateAct;
    QAction *generateCourseAct;
    QAction *courseBrowserAct;
    QAction *recentFileActs[MaxRecentFiles];

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *quitAct;

    cOptionsPage *optionsPage;
    cConsolePage *consolePage;
    cContentPage *contentPage;
    cCourseImageEditor *imageWidget;

    QDockWidget *dockOptionsPage;
    QDockWidget *dockConsolePage;
    QDockWidget *dockContentPage;
    QDockWidget *dockCourseBrowser;

private:
    cCourseTemplate courseTemplate;
    void trace (const QString& txt,const unsigned int& flags);
    QString courseTemplateFileName;
    void setTitle();
    void generateStop();
    cCourseGenerator courseGenerator;
    bool contentChanged;
};

#endif // GUI_H
