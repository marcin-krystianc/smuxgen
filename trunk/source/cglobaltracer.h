//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================


#ifndef CGLOBALTRACER_H
#define CGLOBALTRACER_H

#include <QObject>

enum traceFlags
{
    traceError      = 1<<1,
    traceWarning    = 1<<2,
    traceLevel1     = 1<<3,
    traceLevel2     = 1<<4,
    traceLevel3     = 1<<5,
};

class cGlobaTracer : public QObject
{
    Q_OBJECT
    public:
        cGlobaTracer ();
        void trace (const QString &txt,const int& flags);
        void setTraceFlags(const unsigned int& flags);

    private:
        unsigned int flags;

    signals:
        void traceSignal (const QString &txt,const int &);

};

extern cGlobaTracer globalTracer;

#endif // CGLOBALTRACER_H
