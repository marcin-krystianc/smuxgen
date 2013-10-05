//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include "globalsmuxgentools.h"
#include "sapi.h"

/////////////////////////////////////////////////////////////////////////////
QStringList getVoiceEngines()
{
   QByteArray result;
   QStringList arguments;
   arguments.append("-list");
   if (!runExternalTool("sapi2wav.exe", arguments, &result))
       return QStringList();

   QStringList retlist = QString::fromLocal8Bit(result.constData(), result.length()).split("\r\n", QString::SkipEmptyParts);

   for (int i = 0; i<retlist.count(); i++)
      retlist[i] = retlist[i].right(retlist[i].length()-retlist[i].indexOf(" ")-1);

   return (retlist);
}

/////////////////////////////////////////////////////////////////////////////
int getVoiceEngineIndex(QString voiceEngine) // if failed then returns 0
{
   QStringList voices = getVoiceEngines();
   for (int i = 0; i<voices.count(); ++i) {
      if (voices[i] == voiceEngine)
         return i + 1;
   }

   return 0;
}
