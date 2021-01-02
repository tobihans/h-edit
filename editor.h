#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>
#include <QTimer>
#include <QTimerEvent>
class Editor : public QMainWindow
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr);
    ~Editor();

protected:

    void timerEvent(QTimerEvent *event);

private slots:
    void newF();
    void open();
    void save();
    void saveAs();
    void quit();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void undo();
    void redo();
    void font();
    void hideStatusBar();
    void autoSave();
    void toggleCheckbox(QAction *action);
private:
    // Menu
    QMenu *file;
    QMenu *edit;
    QMenu *view;
    QMenu *preferences;

    // Actions
    QAction *AnewFile;
    QAction *AopenFile;
    QAction *AsaveFile;
    QAction *AsaveFileAs;
    QAction *Aquit;
    QAction *Acut;
    QAction *Acopy;
    QAction *Apaste;
    QAction *AselectAll;
    QAction *Aundo;
    QAction *Aredo;
    QAction *AhideStatusBar;
    QAction *AautoSave;
    QAction *AsetFont;


    QPlainTextEdit *editArea;
    QLabel *statusBarText;
    QString currentFile;
signals:
    // To set the right icon for checkable actions
    void checkboxToggled(QAction *action);

};
#endif // EDITOR_H
