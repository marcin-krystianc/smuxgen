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

#endif // GLOBALSMUXGENTOOLS_H
