#ifndef CODEEDIT_H
#define CODEEDIT_H
#include <QPlainTextEdit>
#include <QWidget>
#include <QResizeEvent>

class LineNumber;

class CodeEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    CodeEdit(QWidget *parent=nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
private:
    QWidget *lineNumberArea;
};

#endif // CODEEDIT_H
