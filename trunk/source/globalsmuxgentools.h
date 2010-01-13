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
QString getKeyWord (QString iString);
bool scalePicture (QString path,int x,int y);
bool checkIsFileOk(const QString &fileName);
QString getFileName (int i);
QString getMediaFileName (int i);

#endif // GLOBALSMUXGENTOOLS_H
