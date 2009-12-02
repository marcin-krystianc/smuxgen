#ifndef COURSETEMPLATEOPTIONS_H
#define COURSETEMPLATEOPTIONS_H

#include <QString>
#include <QObject>
#include "cglobaltracer.h"

class cCourseTemplateOptions {

public:
    cCourseTemplateOptions();

    QString course;
    QString database;
    QString subname;

    int voiceIndex;
    int voiceGain;
    double voiceTrim;


    struct
    {
        unsigned int oForce  : 1; // force to create agian all course elements
        unsigned int oDouble : 1; // make also alternative course
        unsigned int oVoice  : 1; // make mp3 files
        unsigned int oImage  : 1; // get Images
    } bit;

    bool fromString  (const QString &line);
    QString toString () ;

    private:
    void trace (const QString &text,const int & flags = traceLevel1|0);

};

#endif // COURSETEMPLATEOPTIONS_H
