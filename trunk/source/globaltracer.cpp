//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include "globaltracer.h"

GlobaTracer globalTracer;

/////////////////////////////////////////////////////////////////////////////
GlobaTracer::GlobaTracer () :
   m_flags (traceWarning | traceError | traceLevel1)
{
}

/////////////////////////////////////////////////////////////////////////////
void GlobaTracer::trace(const QString &text, const int flags)
{
   if (m_flags & flags)
      emit traceSignal (text, m_flags & flags);
}

/////////////////////////////////////////////////////////////////////////////
void GlobaTracer::setTraceFlags(const unsigned int flags)
{
   m_flags = flags;
}

