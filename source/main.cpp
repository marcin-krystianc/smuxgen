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


/////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
    Q_INIT_RESOURCE(gui);
    QApplication app(argc, argv);

    QDir::setCurrent(QCoreApplication::applicationDirPath());

    app.addLibraryPath(("plugins")); // sql will not work without that

    QDir pDir;
    pDir.mkdir("tmp");

    MainWindow mainWin;
    mainWin.show();

    return app.exec();
}

