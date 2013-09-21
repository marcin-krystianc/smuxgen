//============================================================================
// Author : Marcin Krystianc (marcin.krystianc@gmail.com)
// Version : 2.0
// License : GPL
// URL : http://code.google.com/p/smuxgen/
// Description : SMUXGEN - SuperMemo UX generator
//============================================================================

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

//![codeeditordefinition]

class CodeEditor : public QPlainTextEdit
{
   Q_OBJECT

public:
   CodeEditor(QWidget *parent = 0);

   void lineNumberAreaPaintEvent(QPaintEvent *event);
   int lineNumberAreaWidth();

protected:
   void resizeEvent(QResizeEvent *event);
   void wheelEvent(QWheelEvent *event);

private slots:
   void updateLineNumberAreaWidth(int newBlockCount);
   void updateLineNumberArea(const QRect &, int);

private:
   QWidget *m_lineNumberArea;
};

//![codeeditordefinition]
//![extraarea]

class LineNumberArea : public QWidget
{
public:
   LineNumberArea(CodeEditor *editor) : QWidget(editor) {
      m_codeEditor = editor;
   }

   QSize sizeHint() const {
      return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
   }

protected:
   void paintEvent(QPaintEvent *event) {
      m_codeEditor->lineNumberAreaPaintEvent(event);
   }

private:
   CodeEditor *m_codeEditor;
};

//![extraarea]

#endif
