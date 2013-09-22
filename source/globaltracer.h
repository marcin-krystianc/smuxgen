//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef GLOBALTRACER_H
#define GLOBALTRACER_H

#include <QObject>

enum traceFlags
{
   traceError = 1<<1,
   traceWarning = 1<<2,
   traceLevel1 = 1<<3,
   traceLevel2 = 1<<4,
   traceLevel3 = 1<<5
};

class GlobaTracer : public QObject
{
   Q_OBJECT
public:
   GlobaTracer ();
   void trace (const QString &txt, const int& m_flags);
   void setTraceFlags(const unsigned int& m_flags);

private:
   unsigned int m_flags;

signals:
   void traceSignal (const QString &txt, const int &);
};

extern GlobaTracer globalTracer;

#endif // CGLOBALTRACER_H
