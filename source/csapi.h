//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QStringList>


#ifndef CSAPI_H
#define CSAPI_H

/////////////////////////////////////////////////////////////////////////////
QStringList getVoiceEngines();

/////////////////////////////////////////////////////////////////////////////
int getVoiceEngineIndex(QString voiceEngine); // if failed then returns -1


#endif // CSAPI_H
