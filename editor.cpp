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
#include <QFile>
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
#include <QDockWidget>
#include <QIcon>
#include <QKeySequence>
#include "codeedit.h"
#include "editor.h"
#include "settings.h"
#include "workingdirwidget.h"

static int fontSize;
Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
{
    init();
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
    for (auto *editArea: qAsConst(editAreas))
        delete editArea;
}

/**
 * @brief Editor::init
 */
void Editor::init() {
    // General config
    untitled_files_nb = 1;
    setWindowTitle("h-edit");
    settings = new Settings;
    isSettingsTabOpened = false; // Track whether a tab is already opened for settings or not.
    fontSize = 14;
    qApp->setFont(QFont("monospace"));
    connect(this, &Editor::checkboxToggled, this, &Editor::toggleCheckbox);

    //Tabs
    initTabWidget();
    // Menus
    initMenuBar();
    // Actions
    initActions();
    // ToolBar
    initToolBar();
    // Status Bar
    initStatusBar();
    // init Working directories widget
    initWorkingDir();
}

/**
 * @brief Editor::initTabWidget
 */
void Editor::initTabWidget()
{
    QFont mono("monospace");
    mono.setPixelSize(14);
    tabs = new QTabWidget;
    tabs->setObjectName("tabwidget");
    tabs->setFont(mono);
    tabs->setDocumentMode(true);
    tabs->setTabsClosable(true);
    this->setCentralWidget(tabs);
    connect(tabs, &QTabWidget::tabCloseRequested, this, &Editor::closeTab);
    // Create the first tab
    editAreas.append(new CodeEdit);
    informativeText.append("");
    files.append("");
    tabs->addTab(editAreas[0], QPixmap(":/icons/icons8-file-64.png"),
            QString("Untitled %1").arg(untitled_files_nb));
    connect(tabs, static_cast<void(QTabWidget::*)(int)>(&QTabWidget::tabCloseRequested), [this](int i){
        if (this->tabs->widget(i) == this->settings)
        {
            this->isSettingsTabOpened = false;
            this->tabs->removeTab(i);
        }
        if (this->tabs->count() == 1)
            this->close();
        files.removeAt(i);
        tabs->removeTab(i);
    });
}

/**
 * @brief Editor::initMenuBar
 */
void Editor::initMenuBar()
{
    menuBar()->setStyleSheet("font-size: 14px; color: #0f4851");
    menuBar()->setObjectName("menubar");
    file = menuBar()->addMenu("&File");
    edit = menuBar()->addMenu("&Edit");
    view = menuBar()->addMenu("&View");
    preferences = menuBar()->addMenu("&Preferences");
    help = menuBar()->addMenu("Help");
}

/**
 * @brief Editor::initFileMenuActions
 */
void Editor::initFileMenuActions()
{
    AnewFile = new QAction(QPixmap(":/icons/icons8-file-64.png"),
                           "&New File", this);
    AnewFile->setShortcut(QKeySequence::AddTab);
    connect(AnewFile, &QAction::triggered, this, &Editor::newF);
    file->addAction(AnewFile);

    AopenFile = new QAction(QPixmap(":/icons/icons8-opened-folder-64.png"),
                            "&Open File", this);
    AopenFile->setShortcut(QKeySequence::Open);
    connect(AopenFile, &QAction::triggered, this, &Editor::open);
    file->addAction(AopenFile);

    newWindow = new QAction(QPixmap(":/icons/icons8-new-window-64.png"), "New Window", this);
    newWindow->setShortcut(QKeySequence::New);
    file->addAction(newWindow);
    connect(newWindow, &QAction::triggered, this, [=](){
        emit newWindowRequest();
    });

    AaddProjectFolder = new QAction("Add project folder");
    file->addSeparator();
    file->addAction(AaddProjectFolder);
    connect(AaddProjectFolder, &QAction::triggered, this, [=](){
        workingDirWidget->addprojectFolderRequested();
        this->workingDirDockWidget->show();
    });

    AsaveFile = new QAction(QPixmap(":/icons/icons8-save-64.png"),
                            "Save File", this);
    AsaveFile->setShortcut(QKeySequence::Save);
    connect(AsaveFile, &QAction::triggered, this, &Editor::save);
    file->addSeparator();
    file->addAction(AsaveFile);

    AsaveFileAs = new QAction(QPixmap(":/icons/icons8-save-as-64.png"),
                              "Save File As", this);
    AsaveFileAs->setShortcut(QKeySequence::SaveAs);
    connect(AsaveFileAs, &QAction::triggered, this, &Editor::saveAs);
    file->addAction(AsaveFileAs);

    Aquit = new QAction(QPixmap(":/icons/icons8-exit-64.png"), "Quit",
                        this);
    Aquit->setShortcut(QKeySequence::Quit);
    connect(Aquit, &QAction::triggered, this, &Editor::quit);
    file->addSeparator();
    file->addAction(Aquit);
}

/**
 * @brief Editor::initEditMenuActions
 */
void Editor::initEditMenuActions()
{
    Acut = new QAction(QPixmap(":/icons/icons8-cut-64.png"), "Cut", this);
    Acut->setShortcut(QKeySequence::Cut);
    connect(Acut, &QAction::triggered, this, &Editor::cut);
    edit->addAction(Acut);

    Acopy = new QAction(QPixmap(":/icons/icons8-copy-64.png"), "Copy", this);
    Acopy->setShortcut(QKeySequence::Copy);
    connect(Acopy, &QAction::triggered, this, &Editor::copy);
    edit->addAction(Acopy);

    Apaste = new QAction(QPixmap(":/icons/icons8-paste-64.png"), "Paste", this);
    Apaste->setShortcut(QKeySequence::Paste);
    connect(Apaste, &QAction::triggered, this, &Editor::paste);
    edit->addAction(Apaste);

    AselectAll = new QAction(QPixmap(":/icons/icons8-select-all-64.png"), "Select &All", this);
    AselectAll->setShortcut(QKeySequence::SelectAll);
    connect(AselectAll, &QAction::triggered, this, &Editor::selectAll);
    edit->addSeparator();
    edit->addAction(AselectAll);

    Aundo = new QAction(QPixmap(":/icons/icons8-undo-64.png"), "Undo", this);
    Aundo->setShortcut(QKeySequence::Undo);
    connect(Aundo, &QAction::triggered, this, &Editor::undo);
    edit->addSeparator();
    edit->addAction(Aundo);


    Aredo = new QAction(QPixmap(":/icons/icons8-redo-64.png"), "Redo", this);
    Aredo->setShortcut(QKeySequence::Redo);
    connect(Aredo, &QAction::triggered, this, &Editor::redo);
    edit->addAction(Aredo);
}

/**
 * @brief Editor::initViewMenuActions
 */
void Editor::initViewMenuActions()
{
    AincreaseFontSize = new QAction(QPixmap(":/icons/icons8-zoom-in-64.png"), "Zoom &in", this);
    AincreaseFontSize->setShortcut(QKeySequence::ZoomIn);
    connect(AincreaseFontSize, &QAction::triggered, this, &Editor::increaseFontSize);
    view->addAction(AincreaseFontSize);

    AdecreaseFontSize = new QAction(QPixmap(":/icons/icons8-zoom-out-64.png"), "Zoom &out", this);
    AdecreaseFontSize->setShortcut(QKeySequence::ZoomOut);
    connect(AdecreaseFontSize, &QAction::triggered, this, &Editor::decreaseFontSize);
    view->addAction(AdecreaseFontSize);

    AoriginalFontSize = new QAction(QPixmap(":/icons/icons8-original-size-64.png"), "Default &Size", this);
    AoriginalFontSize->setShortcut(tr("Ctrl+0"));
    connect(AoriginalFontSize, &QAction::triggered, this, &Editor::originalFontSize);
    view->addAction(AoriginalFontSize);

    AhideStatusBar = new QAction(QPixmap(":/icons/icons8-unchecked-checkbox-64.png"),
                                 "Hide Status Bar", this);
    AhideStatusBar->setShortcut(tr("Alt+Shift+S"));
    AhideStatusBar->setCheckable(true);
    AhideStatusBar->setChecked(false);
    connect(AhideStatusBar, &QAction::toggled, this, &Editor::hideStatusBar);
    view->addSeparator();
    view->addAction(AhideStatusBar);

    AhideToolBar = new QAction(QPixmap(":/icons/icons8-unchecked-checkbox-64.png"),
                               "Hide Tool Bar", this);
    AhideToolBar->setShortcut(tr("Alt+Shift+T"));
    AhideToolBar->setCheckable(true);
    AhideToolBar->setChecked(false);
    connect(AhideToolBar, &QAction::toggled, this, &Editor::hideToolBar);
    view->addAction(AhideToolBar);
}

/**
 * @brief Editor::initPreferencesMenuActions
 */
void Editor::initPreferencesMenuActions()
{
    AautoSave = new QAction(QPixmap(":/icons/icons8-unchecked-checkbox-64.png"),
                            "Auto-save current file", this);
    AautoSave->setCheckable(true);
    AautoSave->setChecked(false);
    connect(AautoSave, &QAction::triggered, this, &Editor::autoSave);
    preferences->addAction(AautoSave);

    AsetFont = new QAction(QPixmap(":/icons/icons8-choose-font-48.png"), "Choose font", this);
    AsetFont->setShortcut(tr("Alt+Shift+F"));
    connect(AsetFont, &QAction::triggered, this, &Editor::font);
    preferences->addAction(AsetFont);

    AeditSettings = new QAction(QPixmap(":/icons/icons8-settings-64.png"), "&Settings", this);
    AeditSettings->setShortcut(tr("Ctrl+P"));
    connect(AeditSettings, &QAction::triggered, this, &Editor::openSettingsTab);
    preferences->addAction(AeditSettings);
}

/**
 * @brief Editor::initHelpMenuActions
 */
void Editor::initHelpMenuActions()
{
    AviewDocumentation = new QAction("Documentation");
    help->addAction(AviewDocumentation);
    connect(AviewDocumentation, &QAction::triggered, this, &Editor::viewDocumentation);
}

/**
 * @brief Editor::initActions
 */
void Editor::initActions()
{
    initFileMenuActions();
    initEditMenuActions();
    initViewMenuActions();
    initPreferencesMenuActions();
    initHelpMenuActions();
}

/**
 * @brief Editor::initToolBar
 */
void Editor::initToolBar()
{
    toolbar = addToolBar("Main Toolbar");
    toolbar->setObjectName("toolbar");
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
}

/**
 * @brief Editor::initStatusBar
 */
void Editor::initStatusBar()
{
    statusBar()->setObjectName("status-bar");
    statusBar()->setStyleSheet("background: #0f4851; color: #f5f5f5; font-size: 14px;");
    statusBarText = new QLabel;
    statusBarText->setAlignment(Qt::AlignLeft);
    statusBar()->addPermanentWidget(statusBarText);
}

/**
 * @brief Editor::initWorkingDir
 */
void Editor::initWorkingDir()
{
    workingDirDockWidget = new QDockWidget;
    workingDirDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    workingDirDockWidget->setTitleBarWidget(new QWidget());
    workingDirWidget = new WorkingDirWidget(/**QDir::homePath()**/);
    workingDirWidget->attachModalsTo(this);
    workingDirDockWidget->setWidget(workingDirWidget);
    this->addDockWidget(Qt::LeftDockWidgetArea, workingDirDockWidget);
    workingDirDockWidget->hide();
    connectToWorkingDirSignals();
}

/**
 * @brief Editor::connectToWorkingDirSignals
 */
void Editor::connectToWorkingDirSignals() {
    connect(workingDirWidget, &WorkingDirWidget::openFile,
            this, &Editor::openFileFromProjectWidget);
}

/**
 * @brief Editor::openFileFromProjectWidget
 * @param filePath
 */
void Editor::openFileFromProjectWidget(QString filePath)
{
    QFile file{filePath};
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Warning", "Cannot open file");
        return;
    }
    QTextStream in{&file};
    editAreas.append(new CodeEdit);
    files.append(filePath);
    informativeText.append("");
    int i = editAreas.size() - 1;
    tabs->addTab(editAreas[i], QPixmap(":/icons/icons8-file-64.png"),
                 QFileInfo(filePath).fileName());
    tabs->setCurrentWidget(editAreas[i]);
    editAreas[i]->setPlainText(in.readAll());
    tabs->setTabText(i, QFileInfo(filePath).fileName());
    files[i] = filePath;
}

/**
 * @brief Editor::newF
 */
void Editor::newF()
{
    Editor::untitled_files_nb++;
    editAreas.append(new CodeEdit);
    files.append("");
    informativeText.append("");
    int i = editAreas.size() - 1;
    tabs->addTab(editAreas[i], QPixmap(":/icons/icons8-file-64.png"),
                 QString("Untitled %1").arg(untitled_files_nb));
}

/**
 * @brief Editor::open
 */
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
}

/**
 * @brief Editor::save
 */
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

/**
 * @brief Editor::saveAs
 */
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

/**
 * @brief Editor::quit
 */
void Editor::quit()
{
    this->close();
}

/**
 * @brief Editor::cut
 */
void Editor::cut()
{
    if (tabs->currentWidget() != settings)
    {
        int index{tabs->currentIndex()};
        auto *editArea{editAreas[index]};
        editArea->cut();
    }
}

/**
 * @brief Editor::copy
 */
void Editor::copy()
{
    if (tabs->currentWidget() != settings)
    {
        int index{tabs->currentIndex()};
        auto *editArea{editAreas[index]};
        editArea->copy();
    }
}

/**
 * @brief Editor::paste
 */
void Editor::paste()
{
    if (tabs->currentWidget() != settings)
    {
        int index{tabs->currentIndex()};
        auto *editArea{editAreas[index]};
        editArea->paste();
    }
}

/**
 * @brief Editor::selectAll
 */
void Editor::selectAll()
{
    if (tabs->currentWidget() != settings)
    {
        int index{tabs->currentIndex()};
        auto *editArea{editAreas[index]};
        editArea->selectAll();
    }
}

/**
 * @brief Editor::undo
 */
void Editor::undo()
{
    if (tabs->currentWidget() != settings)
    {
        int index{tabs->currentIndex()};
        auto *editArea{editAreas[index]};
        editArea->undo();
    }
}

/**
 * @brief Editor::redo
 */
void Editor::redo()
{
    if (tabs->currentWidget() != settings)
    {
        int index{tabs->currentIndex()};
        auto *editArea{editAreas[index]};
        editArea->redo();
    }
}

/**
 * @brief Editor::font
 */
void Editor::font()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected)
        for (auto *editArea: qAsConst(editAreas))
            editArea->setFont(font);
}

/**
 * @brief Editor::increaseFontSize
 */
void Editor::increaseFontSize()
{
    int size{fontSize + (10 * Editor::DEFAULT_FONT_SIZE / 100)};
    if (size < Editor::MAX_FONT_SIZE)
    {
        fontSize = size;
        for (auto *editArea: qAsConst(editAreas))
            editArea->setStyleSheet(QString("font-size: %1px").arg(size));
    }
}

/**
 * @brief Editor::decreaseFontSize
 */
void Editor::decreaseFontSize()
{
    int size{fontSize - (10 * Editor::DEFAULT_FONT_SIZE / 100)};
    if (size > Editor::MIN_FONT_SIZE)
    {
        fontSize = size;
        for (auto *editArea: qAsConst(editAreas))
            editArea->setStyleSheet(QString("font-size: %1px").arg(size));
    }
}

/**
 * @brief Editor::originalFontSize
 */
void Editor::originalFontSize()
{
    for (auto *editArea: qAsConst(editAreas))
        editArea->setStyleSheet(QString("font-size: %1px").arg(Editor::DEFAULT_FONT_SIZE));
}

/**
 * @brief Editor::hideStatusBar
 */
void Editor::hideStatusBar()
{
    if (statusBar()->isVisible())
        statusBar()->hide();
    else
        statusBar()->show();
    emit checkboxToggled(AhideStatusBar);
}

/**
 * @brief Editor::hideToolBar
 */
void Editor::hideToolBar()
{
    if (toolbar->isVisible())
        toolbar->hide();
    else
        toolbar->show();
    emit checkboxToggled(AhideToolBar);
}

/**
 * @brief Editor::autoSave
 */
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

/**
 * @brief Editor::closeTab
 */
void Editor::closeTab()
{

}

/**
 * @brief Editor::openSettingsTab
 */
void Editor::openSettingsTab()
{
    if (!isSettingsTabOpened)
    {
        //        settings = new QMainWindow(nullptr, Qt::Widget);
        tabs->addTab(settings , "Settings");
        tabs->setCurrentWidget(settings);
        isSettingsTabOpened = true;
    }
    else
        tabs->setCurrentWidget(settings);
}

/**
 * @brief Editor::viewDocumentation
 */
void Editor::viewDocumentation()
{
    QMessageBox::information(this, "Help", "No help");
}

/**
 * @brief Editor::toggleCheckbox
 * @param action
 */
void Editor::toggleCheckbox(QAction *action)
{
    if (action->isCheckable())
    {
        if(action->isChecked())
        {
            action->setChecked(true);
            action->setIcon(QPixmap(":/icons/icons8-checked-64.png"));
        }
        else
        {
            action->setChecked(false);
            action->setIcon(QPixmap(":/icons/icons8-unchecked-checkbox-64.png"));
        }
    }
}

/**
 * @brief Editor::timerEvent
 * @param event
 */
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
