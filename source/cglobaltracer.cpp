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
    if (this->flags & flags)
        emit traceSignal (text, this->flags & flags);
};

/////////////////////////////////////////////////////////////////////////////
cGlobaTracer::cGlobaTracer ()
{
    this->flags = traceWarning | traceError | traceLevel1;
}

/////////////////////////////////////////////////////////////////////////////
void cGlobaTracer::setTraceFlags(const unsigned int& flags)
{
    this->flags = flags;
}

