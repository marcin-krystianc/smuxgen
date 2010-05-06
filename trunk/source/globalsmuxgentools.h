//============================================================================
// Author       : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version      : 2.0
// License      : GPL
// URL          : http://code.google.com/p/smuxgen/
// Description  : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QString>
#include <QStringList>

#ifndef GLOBALSMUXGENTOOLS_H
#define GLOBALSMUXGENTOOLS_H

extern const QString tmpDir;
extern const int IMG_WIDTH;
extern const int IMG_HEIGHT;

QStringList parseGoogleHtml (const QString &fileName);
void deleteFile (const QString &fileName);
bool scalePicture (QString path,int x,int y);
bool checkIsFileOk(const QString &fileName);
QString getFileName (int i);
QString getMediaFileName (int i);

QString removeAllBetween            (const QString &input,const QString &first,const QString &second);
QStringList getAllBetween           (const QString &input,const QString &first,const QString &second);
QString removeAllSpecialCharacters  (const QString &input,const QStringList &list);

QString getKeyWord      (const QString &input); // get keywords for google search
QString getTranscript   (const QString &input); // get text to read
QString getTextToPrint  (const QString &input); // get text to show to user



QString strippedFile(const QString &fullFileName);
QString strippedDir (const QString &fullFileName);
QString getLastDir ();
void setLastDir (const QString &dir);

#endif // GLOBALSMUXGENTOOLS_H
