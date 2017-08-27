//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
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

   app.addLibraryPath(("plugins")); // sql will not work without that
   app.addLibraryPath(("C:\\Users\\Ja\\Documents\\smuxgen\\build-smuxgen-Desktop_Qt_5_8_0_MSVC2015_64bit-Debug\\debug\\plugins")); // sql will not work without that

   QDir pDir;
   pDir.mkdir("tmp");

   MainWindow mainWin;
   mainWin.show();

   return app.exec();
}

