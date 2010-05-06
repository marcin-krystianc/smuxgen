#include "csapi.h"

#include <QProcess>


/////////////////////////////////////////////////////////////////////////////
QStringList getVoices()
{
    QProcess myProcess;
    QStringList  arguments;
    arguments.append("-list");
    myProcess.start("sapi2wav.exe", arguments );
    if (!myProcess.waitForStarted())
        return QStringList();

    myProcess.waitForFinished();
    if (myProcess.exitCode())
        return QStringList();

    QByteArray result=myProcess.readAllStandardOutput();

    return (QString::fromLocal8Bit(result.constData(),result.length())).split("\r\n");
}

/////////////////////////////////////////////////////////////////////////////
int getVoiceIndex(QString voice) // if failed then returns -1
{
    QStringList voices=getVoices();

    for (int i=0;i<voices.count();++i)
    {
        if (voices[i]==voice)
            return i;
    }
    return -1;
}
