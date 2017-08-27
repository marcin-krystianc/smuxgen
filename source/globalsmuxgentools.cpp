//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <QImage>
#include <QSettings>
#include <QProcess>
#include <QCoreApplication>
#include <QRegularExpressionMatchIterator>

#include "globalsmuxgentools.h"
#include "globaltracer.h"

/////////////////////////////////////////////////////////////////////////////
QStringList parseImagesHtml (const QString &fileName, const QString &regex)
{
   QStringList result;
   globalTracer.trace(QString("parseImagesHtml: ")+fileName, traceLevel3);

   QFile inputFile(fileName);
   if (!inputFile.open(QIODevice::ReadOnly)) {
      globalTracer.trace(QString("Cannot open file: ")+fileName, traceError);
      return result;
   }

   QTextStream inputFileStream;
   inputFileStream.setDevice(&inputFile);
   inputFileStream.setCodec("UTF-8");
   QString html = inputFileStream.readAll();
   QRegularExpression re(regex);
   QRegularExpressionMatchIterator i = re.globalMatch(html);

   while (i.hasNext()) {
       QRegularExpressionMatch match = i.next();
       QString word = match.captured(1);
       result << word;

       globalTracer.trace(QString("parseImagesHtml: found link:")+word, traceLevel3);
   }

   return result;
}

/////////////////////////////////////////////////////////////////////////////
void deleteFile (const QString &fileName)
{
   QFile fileObject (fileName);
   globalTracer.trace(QString("deleteFile: ")+fileName, traceLevel3);
   fileObject.remove();
}
/////////////////////////////////////////////////////////////////////////////
QString getTranscript (const QString &input) // get text to read
{
   QStringList special;
   special<<"("<<")"<<"["<<"]"<<"{"<<"}";
   QString ret = removeAllBetween (input, "(", ")");
   return removeAllSpecialCharacters(ret, special)+" ";
}

/////////////////////////////////////////////////////////////////////////////
QString getTextToPrint (const QString &input) // get text to show to user
{
   QStringList special;
   special<<"["<<"]"<<"{"<<"}";
   QString ret = removeAllBetween (input, "{", "}");
   return (removeAllSpecialCharacters(ret, special)).trimmed();
}

/////////////////////////////////////////////////////////////////////////////
QStringList getKeyWord (const QString &input)
{
   const QString A = QString::fromUtf8("ĄĆŻŹŚŃÓŁĘąćżźśńółę!@#$%^&*()_- = +, ./<>?; ':\"[]{}|");
   const QString B = QString::fromUtf8("ACZXSNOLEaczzsnole                                   ");

   QString temp = input;

   for (int i = A.length()-1; i >= 0; --i)
      temp.replace(A.at(i), B.at(i));

   QStringList tmp = temp.split(" ", QString::SkipEmptyParts);

   QStringList retList;

   // add keywords defined by user ([keyword])
   QStringList tmpList = getAllBetween(input, "[", "]");
   QStringList special;
   special<<"("<<")"<<"["<<"]"<<"{"<<"}";

   if (tmpList.count()>0) {
      for (int i = 0; i<tmpList.count(); ++i)
         retList.insert(0, removeAllSpecialCharacters(tmpList.at(i), special));
   }

   else {
      retList.append("");
      retList.append("");

      for (int i = 0; i<tmp.count(); ++i) {
         for (int j = 0; j<retList.count(); j++) {
            if ((tmp.at(i).length()>(retList.at(j)).length())) {
               retList.insert(j, tmp[i]);
               retList.pop_back();
               break;
            }
         }
      }
   }

   return retList;
}

/////////////////////////////////////////////////////////////////////////////
QString removeAllBetween (const QString &input, const QString &first, const QString &second)
{
   QString ret = input;

   for (;;) {
      int a = ret.indexOf(first, 0);
      if (a == -1)
         return ret;

      int b = ret.indexOf(second, a);
      if (b == -1)
         return ret;

      ret.remove(a, b-a+1);
   }
}

/////////////////////////////////////////////////////////////////////////////
QStringList getAllBetween (const QString &input, const QString &first, const QString &second)
{
   QStringList ret;

   for (;;) {
      int a = ret.indexOf(first, 0);
      if (a == -1)
         return ret;

      int b = ret.indexOf(second, a);
      if (b == -1)
         return ret;

      ret+= input.mid(a+1, b-a-1);
   }
}

/////////////////////////////////////////////////////////////////////////////
QString removeAllSpecialCharacters (const QString &input, const QStringList &list)
{
   QString ret = input;
   for (int i = 0; i<list.count(); i++)
      ret.remove(list.at(i));

   return ret;
}

/////////////////////////////////////////////////////////////////////////////
bool scalePicture (QString path, int x, int y)
{
   QImage img;

   if (!img.load(path))  {
      globalTracer.trace(QString("Error:scalePicture load:")+path, traceError);
      return false;
   }

   if (!img.scaled(x, y).save(path)) {
      globalTracer.trace(QString("Error:scalePicture save:")+path, traceError);
      return false;
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////
bool checkIsFileOk(const QString &fileName)
{
   QFile fileObject (fileName);
   if (fileObject.size() > 0)
      return true;

   globalTracer.trace(QString("NOT OK: ")+fileName, traceLevel1);
   return false;
}

/////////////////////////////////////////////////////////////////////////////
QString getFileName (int i)
{
   QString name;
   name.fill('0', 5);
   name+= QString::number(i);
   name+= ".xml";
   name = name.right(9);
   return QString("item")+name;
}

/////////////////////////////////////////////////////////////////////////////
QString getMediaFileName (int i)
{
   QString name;
   name.fill('0', 5);
   name+= QString::number(i);
   name = name.right(5);
   return name;
}

/////////////////////////////////////////////////////////////////////////////
QString getLastDir ()
{
   QSettings settings("Smuxgen", "Smuxgen");
   return settings.value("recentDir").toString();
}

/////////////////////////////////////////////////////////////////////////////
void setLastDir (const QString &dir)
{
   QSettings settings("Smuxgen", "Smuxgen");
   settings.setValue("recentDir", dir);
}

/////////////////////////////////////////////////////////////////////////////
QString strippedFile(const QString &fullFileName)
{
   return QFileInfo(fullFileName).fileName();
}

/////////////////////////////////////////////////////////////////////////////
QString strippedDir (const QString &fullFileName)
{
   return QFileInfo(fullFileName).dir().canonicalPath();
}

/////////////////////////////////////////////////////////////////////////////
bool runExternalTool (const QString &tool, const QStringList &arguments, QByteArray *result)
{
   QString toolPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+"\\tools\\"+tool);
   globalTracer.trace(QString("runExternalTool: ")+toolPath+" "+ arguments.join(" "), traceLevel1);

   const int noTimeout = -1;
   QProcess myProcess;
   myProcess.start(toolPath, arguments);
   if (!myProcess.waitForStarted()){
      globalTracer.trace(QString("runExternalTool error: waitForStarted()"), traceError);
      return false;
   }

   myProcess.waitForFinished(noTimeout);
   if (myProcess.exitCode()){
      globalTracer.trace(QString("runExternalTool error:") + myProcess.readAllStandardOutput(), traceError);
      return false;
   }

   if (result)
      *result = myProcess.readAllStandardOutput();

   return true;
}

/////////////////////////////////////////////////////////////////////////////
void generateMp3
(
      const QString &filePath,
      const QString &mp3Text,
      int voiceEngineIndex,
      int voiceGain,
      double voiceTrim
      )
{
   QString escapedText = "\"" + mp3Text + "\"";
   QStringList arguments;
   arguments.append(QDir::toNativeSeparators(filePath));
   arguments.append(mp3Text);
   arguments.append(QString::number(voiceEngineIndex));
   arguments.append(QString::number(voiceTrim));
   arguments.append(QString::number(voiceGain));
   runExternalTool ("createMp3.bat", arguments);
}
