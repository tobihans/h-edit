#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QPixmap>
#include <QFont>
#include <QFontDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QTimerEvent>
#include <QTabWidget>
#include <QTabBar>
#include <QLabel>
#include <QTextStream>
#include "codeedit.h"
#include "editor.h"

static int fontSize;
// For counting the number untitled files opened
int Editor::untitled_files_nb = 1;

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
{
    fontSize = 14;
    // General style
    QString style{
        "QMainWindow { font-size: 12px; }"
        "QMainWindow, QPlainTextEdit{ background: #002b36; color: #f5f5f5}"
        "QMenu QAction:hover { background: #098890; }"
    };
    qApp->setStyleSheet(style);
    qApp->setFont(QFont("monospace"));
    connect(this, &Editor::checkboxToggled, this, &Editor::toggleCheckbox);
    tabs = new QTabWidget;
    tabs->setDocumentMode(true);
    tabs->setTabsClosable(true);
    tabs->setTabBarAutoHide(true);
    setCentralWidget(tabs);

    // Create the first tab
    editAreas.append(new CodeEdit);
    informativeText.append("");
    files.append("");
    tabs->addTab(editAreas[0], QPixmap("icons/icons8-file-64.png"),
        QString("Untitled %1").arg(untitled_files_nb));
    setWindowTitle("h-edit");
    toolbar = addToolBar("Main Toolbar");

    // Menus
    menuBar()->setStyleSheet("font-size: 14px; color: #0f4851");
    file = menuBar()->addMenu("&File");
    edit = menuBar()->addMenu("&Edit");
    view = menuBar()->addMenu("&View");
    preferences = menuBar()->addMenu("&Preferences");

        // Actions
    AnewFile = new QAction(QPixmap("icons/icons8-file-64.png"),
        "&New File", this);
    AnewFile->setShortcut(tr("Ctrl+N"));
    connect(AnewFile, &QAction::triggered, this, &Editor::newF);
    file->addAction(AnewFile);

    AopenFile = new QAction(QPixmap("icons/icons8-opened-folder-64.png"),
        "&Open File", this);
    AopenFile->setShortcut(tr("Ctrl+O"));
    connect(AopenFile, &QAction::triggered, this, &Editor::open);
    file->addAction(AopenFile);

    AsaveFile = new QAction(QPixmap("icons/icons8-save-64.png"),
        "Save File", this);
    AsaveFile->setShortcut(tr("Ctrl+S"));
    connect(AsaveFile, &QAction::triggered, this, &Editor::save);
    file->addAction(AsaveFile);

    AsaveFileAs = new QAction(QPixmap("icons/icons8-save-as-64.png"),
        "Save File As", this);
    AsaveFileAs->setShortcut(tr("Ctrl+Shift+S"));
    connect(AsaveFileAs, &QAction::triggered, this, &Editor::saveAs);
    file->addAction(AsaveFileAs);

    Aquit = new QAction(QPixmap("icons/icons8-exit-64.png"), "Quit",
        this);
    Aquit->setShortcut(tr("Ctrl+Q"));
    connect(Aquit, &QAction::triggered, this, &Editor::quit);
    file->addSeparator();
    file->addAction(Aquit);

    Acut = new QAction(QPixmap("icons/icons8-cut-64.png"), "Cut", this);
    Acut->setShortcut(tr("Ctrl+X"));
    connect(Acut, &QAction::triggered, this, &Editor::cut);
    edit->addAction(Acut);

    Acopy = new QAction(QPixmap("icons/icons8-copy-64.png"), "Copy", this);
    Acopy->setShortcut(tr("Ctrl+C"));
    connect(Acopy, &QAction::triggered, this, &Editor::copy);
    edit->addAction(Acopy);

    Apaste = new QAction(QPixmap("icons/icons8-paste-64.png"), "Paste", this);
    Apaste->setShortcut(tr("Ctrl+P"));
    connect(Apaste, &QAction::triggered, this, &Editor::paste);
    edit->addAction(Apaste);

    AselectAll = new QAction(QPixmap("icons/icons8-select-all-64.png"), "Select &All", this);
    AselectAll->setShortcut(tr("Ctrl+A"));
    connect(AselectAll, &QAction::triggered, this, &Editor::selectAll);
    edit->addSeparator();
    edit->addAction(AselectAll);

    Aundo = new QAction(QPixmap("icons/icons8-undo-64.png"), "Undo", this);
    Aundo->setShortcut(tr("Ctrl+Z"));
    connect(Aundo, &QAction::triggered, this, &Editor::undo);
    edit->addSeparator();
    edit->addAction(Aundo);


    Aredo = new QAction(QPixmap("icons/icons8-redo-64.png"), "Redo", this);
    Aredo->setShortcut(tr("Ctrl+R"));
    connect(Aredo, &QAction::triggered, this, &Editor::redo);
    edit->addAction(Aredo);

    AincreaseFontSize = new QAction(QPixmap("icons/icons8-zoom-in-64.png"), "Zoom &in", this);
    AincreaseFontSize->setShortcut(tr("Ctrl++"));
    connect(AincreaseFontSize, &QAction::triggered, this, &Editor::increaseFontSize);
    view->addSeparator();
    view->addAction(AincreaseFontSize);

    AdecreaseFontSize = new QAction(QPixmap("icons/icons8-zoom-out-64.png"), "Zoom &out", this);
    AdecreaseFontSize->setShortcut(tr("Ctrl+-"));
    connect(AdecreaseFontSize, &QAction::triggered, this, &Editor::decreaseFontSize);
    view->addAction(AdecreaseFontSize);

    AoriginalFontSize = new QAction(QPixmap("icons/icons8-original-size-64.png"), "Default &Size", this);
    AoriginalFontSize->setShortcut(tr("Ctrl+0"));
    connect(AoriginalFontSize, &QAction::triggered, this, &Editor::originalFontSize);
    view->addAction(AoriginalFontSize);

    AhideStatusBar = new QAction(QPixmap("icons/icons8-unchecked-checkbox-64.png"),
        "Hide Status Bar", this);
    AhideStatusBar->setShortcut(tr("Alt+S"));
    AhideStatusBar->setCheckable(true);
    AhideStatusBar->setChecked(false);
    connect(AhideStatusBar, &QAction::toggled, this, &Editor::hideStatusBar);
    view->addAction(AhideStatusBar);

    AhideToolBar = new QAction(QPixmap("icons/icons8-unchecked-checkbox-64.png"),
        "Hide Tool Bar", this);
    AhideToolBar->setShortcut(tr("Alt+T"));
    AhideToolBar->setCheckable(true);
    AhideToolBar->setChecked(false);
    connect(AhideToolBar, &QAction::toggled, this, &Editor::hideToolBar);
    view->addAction(AhideToolBar);

    AautoSave = new QAction(QPixmap("icons/icons8-unchecked-checkbox-64.png"),
        "Auto-save current file", this);
    AautoSave->setCheckable(true);
    AautoSave->setChecked(false);
    connect(AautoSave, &QAction::triggered, this, &Editor::autoSave);
    preferences->addAction(AautoSave);

    AsetFont = new QAction(QPixmap("icons/icons8-choose-font-48.png"), "Choose font", this);
    AsetFont->setShortcut(tr("Ctrl+F"));
    connect(AsetFont, &QAction::triggered, this, &Editor::font);
    preferences->addAction(AsetFont);

    // ToolBar
    toolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::RightToolBarArea);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->addAction(AnewFile);
    toolbar->addAction(AopenFile);
    toolbar->addAction(AsaveFile);
    toolbar->addSeparator();
    toolbar->addAction(AselectAll);
    toolbar->addAction(Acut);
    toolbar->addAction(Acopy);
    toolbar->addAction(Apaste);
    toolbar->addSeparator();
    toolbar->addAction(Aundo);
    toolbar->addAction(Aredo);
    toolbar->addSeparator();
    toolbar->addAction(AincreaseFontSize);
    toolbar->addAction(AdecreaseFontSize);
    toolbar->addSeparator();
    toolbar->addAction(Aquit);

    // Status Bar
    statusBar()->setStyleSheet("background: #0f4851; color: #f5f5f5; font-size: 14px;");
    statusBarText = new QLabel;
    statusBarText->setAlignment(Qt::AlignLeft);
    statusBar()->addPermanentWidget(statusBarText);
}

Editor::~Editor()
{
    delete file;
    delete edit;
    delete view;
    delete preferences;

    delete toolbar;
    delete AnewFile;
    delete AopenFile;
    delete AsaveFile;
    delete AsaveFileAs;
    delete Aquit;
    delete Acut;
    delete Acopy;
    delete Apaste;
    delete AselectAll;
    delete Aundo;
    delete Aredo;
    delete AhideStatusBar;
    delete AhideToolBar;
    delete AautoSave;
    delete AsetFont;
    delete AoriginalFontSize;
    delete AincreaseFontSize;
    delete AdecreaseFontSize;
    delete statusBarText;
    // Delete Widgets for tab pages
    for (auto *editArea: editAreas)
        delete editArea;
}

void Editor::newF()
{
    Editor::untitled_files_nb++;
    editAreas.append(new CodeEdit);
    files.append("");
    informativeText.append("");
    int i = editAreas.size() - 1;
    tabs->addTab(editAreas[i], QPixmap("icons/icons8-file-64.png"),
        QString("Untitled%1").arg(Editor::untitled_files_nb));
}

void Editor::open()
{
    int index{tabs->currentIndex()};
    auto *currentTab{editAreas[index]};
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
    if (fileName.isEmpty()) return;
    QFile file{fileName};
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Warning", "Cannot open file");
        return;
    }
    QTextStream in{&file};
    currentTab->setPlainText(in.readAll());
    tabs->setTabText(index, QFileInfo(fileName).fileName());
    files[index] = fileName;
    tabs->tabBar()->show();
}

void Editor::save()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    QString currentFile{{files[index]}};
    QString fileName;
    // If  no fileName
    if (currentFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save");
        files[index] = fileName;
    }
    else
    {
        fileName = currentFile;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    tabs->setTabText(index, QFileInfo(fileName).fileName());
    QTextStream out(&file);
    QString text = editArea->toPlainText();
    out << text;
    file.close();
    tabs->tabBar()->show();
//    statusBarText->setText("Last Saved " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));
}

void Editor::saveAs()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, "Save");
    files[index] = fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    tabs->setTabText(index, QFileInfo(fileName).fileName());
    QTextStream out(&file);
    QString text = editArea->toPlainText();
    out << text;
    file.close();
    tabs->tabBar()->show();
//    statusBarText->setText("Last Saved " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));
}

void Editor::quit()
{
    qApp->quit();
}

void Editor::cut()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    editArea->cut();
}

void Editor::copy()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    editArea->copy();
}

void Editor::paste()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    editArea->paste();
}

void Editor::selectAll()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    editArea->selectAll();
}

void Editor::undo()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    editArea->undo();
}

void Editor::redo()
{
    int index{tabs->currentIndex()};
    auto *editArea{editAreas[index]};
    editArea->redo();
}

void Editor::font()
{
    bool fontSelected;
       QFont font = QFontDialog::getFont(&fontSelected, this);
       if (fontSelected)
           for (auto *editArea: editAreas)
               editArea->setFont(font);
}

void Editor::increaseFontSize()
{
    int size{fontSize + (10 * Editor::DEFAULT_FONT_SIZE / 100)};
    if (size < Editor::MAX_FONT_SIZE)
    {
        fontSize = size;
        for (auto *editArea: editAreas)
            editArea->setStyleSheet(QString("font-size: %1px").arg(size));
    }
}

void Editor::decreaseFontSize()
{
    int size{fontSize - (10 * Editor::DEFAULT_FONT_SIZE / 100)};
    if (size > Editor::MIN_FONT_SIZE)
    {
        fontSize = size;
        for (auto *editArea: editAreas)
            editArea->setStyleSheet(QString("font-size: %1px").arg(size));
    }
}

void Editor::originalFontSize()
{
    for (auto *editArea: editAreas)
        editArea->setStyleSheet(QString("font-size: %1px").arg(Editor::DEFAULT_FONT_SIZE));
}

void Editor::hideStatusBar()
{
    if (statusBar()->isVisible())
        statusBar()->hide();
    else
        statusBar()->show();
    emit checkboxToggled(AhideStatusBar);
}

void Editor::hideToolBar()
{
    if (toolbar->isVisible())
        toolbar->hide();
    else
        toolbar->show();
    emit checkboxToggled(AhideToolBar);
}

void Editor::autoSave()
{
    static int id;
    auto currentFile{files[tabs->currentIndex()]};
    if (AautoSave->isChecked())
    {
        if (currentFile.isEmpty()) {
            QMessageBox::warning(this, "Auto Save", "The file need to be saved a first time before enabling autosave",
                QMessageBox::Ok);
            AautoSave->setChecked(false);
            return;
        }
        emit checkboxToggled(AautoSave);
        id = startTimer(300000);
    }
    else
    {
        killTimer(id);
    }

}

void Editor::toggleCheckbox(QAction *action)
{
    if (action->isCheckable())
    {
        if(action->isChecked())
        {
            action->setChecked(true);
            action->setIcon(QPixmap("icons/icons8-checked-64.png"));
        }
        else
        {
            action->setChecked(false);
            action->setIcon(QPixmap("icons/icons8-unchecked-checkbox-64.png"));

        }
    }
}

void Editor::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    auto currentFile{files[tabs->currentIndex()]};
    auto *editArea{editAreas[tabs->currentIndex()]};
    QFile file{currentFile};
    if (!file.open(QIODevice::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file automatically : " + file.errorString());
        return;
    }
    QTextStream out(&file);
    QString text = editArea->toPlainText();
    out << text;
    file.close();
//    statusBarText->setText("Last Saved " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));
}
