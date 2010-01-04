//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================


#include "gui.h"

#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QStringList>
#include <QDir>


/*
systray

*/
/////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
    Q_INIT_RESOURCE(gui);

    QApplication app(argc, argv);   // sql will not work without that
    app.addLibraryPath(("plugins"));

    QDir *pDir= new QDir;
    if (!pDir->exists("tmp"))
       pDir->mkdir("tmp");
    delete(pDir);

    QStringList argList;
    while (argc >1)
        argList.push_front(argv[--argc]);

    MainWindow mainWin (argList);
    mainWin.show();

    return app.exec();
}

