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
#include "smuxgenwidgets.h"
#include "coursetemplate.h"
#include "coursegenerator.h"
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
   void openCourseTemplateSlot(const QString &fileNamePar = "");
   bool saveCourseTemplateSlot();
   void importQASlot();
   void exportQASlot();
   bool saveAsCourseTemplateSlot();
   void buildCourseSlot(bool rebuild=false);
   void rebuildCourseSlot();
   void stopBuildSlot();

   void courseBrowserOpenCloseSlot(); // start/stop picture browser

   void buildCourseFinishedSlot(); // generate finished
   void progressSlot(const QString &);
   void contentChangedSlot();

   void closeSlot();
   void about();

   void openRecentFile();

   void updateUserInterface();

protected:
   void closeEvent(QCloseEvent *event);

private:
   static const int MAX_RECENT_FILES = 15;

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
   QAction *m_buildCourseAction;
   QAction *m_rebuildCourseAction;
   QAction *m_stopBuildAction;
   QAction *m_courseBrowserAction;

   QAction *m_aboutAction;
   QAction *m_aboutQtAction;
   QAction *m_quitAction;

   OptionsPage *m_optionsPage;
   ConsolePage *m_consolePage;
   ContentPage *m_contentPage;
   CourseImageEditor *m_imageWidget;

   QDockWidget *m_dockOptionsPage;
   QDockWidget *m_dockConsolePage;
   QDockWidget *m_dockContentPage;
   QDockWidget *m_dockCourseBrowser;

   QVector <QAction*> m_recentFileActions;

private:
   void trace (const QString& txt, const unsigned int& flags);
   void setTitle();


   CourseGenerator m_courseGenerator;
   CourseTemplate m_courseTemplate;
   QString m_courseTemplateFileName;
   bool m_contentChanged;
};

#endif // GUI_H
