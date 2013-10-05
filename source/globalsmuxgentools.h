//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QString>
#include <QStringList>

#ifndef GLOBALSMUXGENTOOLS_H
#define GLOBALSMUXGENTOOLS_H

const QString TMPDIR = QString("tmp\\");
const int IMG_WIDTH = 320;
const int IMG_HEIGHT = 320;

QStringList parseGoogleHtml (const QString &fileName);
void deleteFile (const QString &fileName);
bool scalePicture (QString path, int x, int y);
bool checkIsFileOk(const QString &fileName);
QString getFileName (int i);
QString getMediaFileName (int i);

QString removeAllBetween (const QString &input, const QString &first, const QString &second);
QStringList getAllBetween (const QString &input, const QString &first, const QString &second);
QString removeAllSpecialCharacters (const QString &input, const QStringList &list);

QString getKeyWord (const QString &input); // get keywords for google search
QString getTranscript (const QString &input); // get text to read
QString getTextToPrint (const QString &input); // get text to show to user

QString strippedFile(const QString &fullFileName);
QString strippedDir (const QString &fullFileName);
QString getLastDir ();

void generateMp3(const QString &filePath, const QString &mp3Text, int voiceEngineIndex, int voiceGain, double voiceTrim);

void setLastDir (const QString &dir);

#endif // GLOBALSMUXGENTOOLS_H
