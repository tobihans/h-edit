#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>
#include <QToolBar>
#include <QTabWidget>
#include <QTimer>
#include <QTimerEvent>
#include <QDialog>
#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDir>

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    static int untitled_files_nb;
    static int const MIN_FONT_SIZE = 7;
    static int const DEFAULT_FONT_SIZE = 14;
    static int const MAX_FONT_SIZE = 24;
    Editor(QWidget *parent = nullptr);
    virtual ~Editor();

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
    void increaseFontSize();
    void decreaseFontSize();
    void originalFontSize();
    void hideStatusBar();
    void hideToolBar();
    void autoSave();
    void closeTab();
    void openSettingsTab();
    void toggleCheckbox(QAction *action);
private:
    void initTabWidget();
    void initMenu();
    void initActions();
    void initToolBar();
    void initStatusBar();

    // Menu
    QMenu *file;
    QMenu *edit;
    QMenu *view;
    QMenu *preferences;

    QToolBar *toolbar;

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
    QAction *AhideToolBar;
    QAction *AautoSave;
    QAction *AsetFont;
    QAction *AeditSettings;
    QAction *AoriginalFontSize;
    QAction *AincreaseFontSize;
    QAction *AdecreaseFontSize;

    // Multiple files
    QTabWidget *tabs;
    QList<QPlainTextEdit*> editAreas;
    QList<QString> files;
    QList<QString> informativeText;
    //////////
    QLabel *statusBarText;

    // Settings Widget
    bool isSettingsTabOpened;
    QWidget *settings;

    //Working dir widgets
    QDockWidget *wdDock;
    QFileSystemModel *model;
    QTreeView *tr_view;

signals:
    // To set the right icon for checkable actions
    void checkboxToggled(QAction *action);
};
#endif // EDITOR_H
