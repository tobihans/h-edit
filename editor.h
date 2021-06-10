#ifndef EDITOR_H
#define EDITOR_H

#include "workingdirwidget.h"
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
    void viewDocumentation();
    void toggleCheckbox(QAction *action);

private:
    void init();
    void initTabWidget();
    void initMenuBar();
    void initFileMenuActions();
    void initEditMenuActions();
    void initViewMenuActions();
    void initPreferencesMenuActions();
    void initHelpMenuActions();
    void initActions();
    void initToolBar();
    void initStatusBar();
    void initWorkingDir();
    void connectToWorkingDirSignals();
    void openFileFromProjectWidget(QString filePath);

    int untitled_files_nb;

    // Menu
    QMenu *file;
    QMenu *edit;
    QMenu *view;
    QMenu *preferences;
    QMenu *help;

    QToolBar *toolbar;

    // Actions
    QAction *AnewFile;
    QAction *AopenFile;
    QAction *newWindow;
    QAction *AaddProjectFolder;
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
    QAction *AviewDocumentation;

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

    //Working dirs items
    QDockWidget *workingDirDockWidget;
    WorkingDirWidget *workingDirWidget;


signals:
    // To set the right icon for checkable actions
    void checkboxToggled(QAction *action);
    void newWindowRequest();
};
#endif // EDITOR_H
