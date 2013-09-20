//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
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
    void openCourseTemplateSlot(QString filename = "");
    bool saveCourseTemplateSlot();
    void importQASlot();
    void exportQASlot();
    bool saveAsCourseTemplateSlot();
    void generateCourseSlot(); // start/stop generate course
    void courseBrowserOpenCloseSlot(); // start/stop picture browser
    void courseBrowserVisibleSlot(bool visible); // picture browser

    void generateCourseFinishedSlot(); // generate finished
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
    static const int MaxRecentFiles = 15;

    void createMenus();
    void createActions();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void updateRecentFileActions();

    QMenu *m_fileMenu;
    QToolBar *m_toolBar;
    QMenu *m_recentMenu;
    QMenu *m_helpMenu;

    QMenu *m_viewMenu;

    QAction *m_openCourseTemplateAction;
    QAction *m_saveCourseTemplateAction;
    QAction *m_importQAAction;
    QAction *m_exportQAAction;
    QAction *m_saveAsCourseTemplateAction;
    QAction *m_generateCourseAction;
    QAction *m_courseBrowserAction;

    QAction *m_aboutAction;
    QAction *m_aboutQtAction;
    QAction *m_quitAction;

    cOptionsPage *m_optionsPage;
    cConsolePage *m_consolePage;
    cContentPage *m_contentPage;
    cCourseImageEditor *m_imageWidget;

    QDockWidget *m_dockOptionsPage;
    QDockWidget *m_dockConsolePage;
    QDockWidget *m_dockContentPage;
    QDockWidget *m_dockCourseBrowser;

    QVector <QAction*> m_recentFileActions;
private:
    void trace (const QString& txt, const unsigned int& flags);
    void setTitle();
    void generateStop();

    CourseGenerator m_courseGenerator;
    cCourseTemplate m_courseTemplate;
    QString m_courseTemplateFileName;
    bool m_contentChanged;
};

#endif // GUI_H
