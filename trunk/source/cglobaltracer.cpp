#include "cglobaltracer.h"

cGlobaTracer globalTracer;

/////////////////////////////////////////////////////////////////////////////
void cGlobaTracer::trace(const QString &text,const int& flags)
{
    if (this->flags & flags)
        emit traceSignal (text,this->flags & flags);
};

/////////////////////////////////////////////////////////////////////////////
cGlobaTracer::cGlobaTracer ()
{
    this->flags = traceWarning | traceError | traceLevel1;
    //| traceLevel2 | traceLevel3;
}
