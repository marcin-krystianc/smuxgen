//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QStringList>


#ifndef SAPI_H
#define SAPI_H

/////////////////////////////////////////////////////////////////////////////
QStringList getVoiceEngines();

/////////////////////////////////////////////////////////////////////////////
int getVoiceEngineIndex(QString voiceEngine);

#endif // CSAPI_H
