//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include "cglobaltracer.h"

cGlobaTracer globalTracer;

/////////////////////////////////////////////////////////////////////////////
void cGlobaTracer::trace(const QString &text, const int& flags)
{
    if (m_flags & flags)
        emit traceSignal (text, m_flags & flags);
};

/////////////////////////////////////////////////////////////////////////////
cGlobaTracer::cGlobaTracer ()
{
    m_flags = traceWarning | traceError | traceLevel1;
}

/////////////////////////////////////////////////////////////////////////////
void cGlobaTracer::setTraceFlags(const unsigned int& flags)
{
    m_flags = flags;
}

