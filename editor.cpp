#include <QApplication>
#include <QMenu>
#include <QMenuBar>
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
#include <QLabel>
#include <QTextStream>
#include "editor.h"

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
{
    // General style
    QString style{
        "QMainWindow { font-size: 12px; }"
        "QMainWindow, QPlainTextEdit{ background: #002b36; color: #f5f5f5}"
        "QMenu QAction:hover { background: #098890; }"
    };
    qApp->setStyleSheet(style);
    qApp->setFont(QFont("Comic Sans MS"));
    connect(this, &Editor::checkboxToggled, this, &Editor::toggleCheckbox);
    editArea = new QPlainTextEdit;
    editArea->setFont(QFont("monospace"));
    setCentralWidget(editArea);
    setWindowTitle("h-edit");

    // Menus
    menuBar()->setStyleSheet("color: #0f4851");
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
    edit->addSeparator();
    edit->addAction(Aundo);

    auto *redo = new QAction(QPixmap("icons/icons8-undo-64.png"), "Redo", this);
    redo->setShortcut(tr("Ctrl+R"));
    edit->addAction(redo);

    AhideStatusBar = new QAction(QPixmap("icons/icons8-unchecked-checkbox-64.png"),
        "Hide Status Bar", this);
    AhideStatusBar->setShortcut(tr("Alt+S"));
    AhideStatusBar->setCheckable(true);
    AhideStatusBar->setChecked(false);
    connect(AhideStatusBar, &QAction::toggled, this, &Editor::hideStatusBar);
    view->addAction(AhideStatusBar);

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

    // Status Bar
    statusBar()->setStyleSheet("background: #0f4851; color: #f5f5f5");
    statusBarText = new QLabel;
    statusBarText->setAlignment(Qt::AlignLeft);
    statusBar()->addPermanentWidget(statusBarText);
}

Editor::~Editor()
{
    delete editArea;
}

void Editor::newF()
{
//    // If there is unsaved changes
//    if (!currentFile.isEmpty())
//    {
//        QFile file{currentFile};
//        if (!file.open(QIODevice::ReadOnly))
//        {
//            QMessageBox::warning(this, "Error", "There was an error reading file:" + file.errorString(),
//                QMessageBox::Ok);
//        }
//        else
//        {
//            QTextStream in{&file};
//            QString content{in.readAll()};
//            if (content != editArea->toPlainText())
//            {
//                QMessageBox warning;
//                warning.setText("Unsaved Changes");
//                warning.setInformativeText(QString("%1 was modified.").arg(currentFile));
//                warning.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
//                warning.setDefaultButton(QMessageBox::Save);
//                int ans = warning.exec();
//                switch (ans)
//                {
//                    case QMessageBox::Discard:
//                        setWindowTitle("Untitled");
//                        editArea->insertPlainText("");
//                        break;
//                    case QMessageBox::Cancel:
//                        return;
//                    case QMessageBox::Save:
//                        this->save();
//                        setWindowTitle("Untitled");
//                        editArea->insertPlainText("");
//                        break;
//                    default:
//                    QMessageBox::warning(this, "Hans", "Hans");
//                        break;

//                }
//            }
//        }
//        return;
//    }
//    if (currentFile.isEmpty() && editArea->toPlainText() != "")
//    {
//        QMessageBox warning;
//        warning.setText("Unsaved Changes");
//        warning.setInformativeText(" The file was modified.");
//        warning.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
//        warning.setDefaultButton(QMessageBox::Save);
//        int ans = warning.exec();
//        switch (ans)
//        {
//            case QMessageBox::Discard:
//                setWindowTitle("Untitled");
//                editArea->insertPlainText("");
//                break;
//            case QMessageBox::Cancel:
//                return;
//            case QMessageBox::Save:
//                this->save();
//                setWindowTitle("Untitled");
//                editArea->insertPlainText("");
//                break;
//            default:
//            QMessageBox::warning(this, "Hans", "Hans");
//                break;

//        }
//    }
//    else
    currentFile.clear();
    editArea->clear();
    setWindowTitle("Untitled");
    editArea->insertPlainText("");
}

void Editor::open()
{
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
    editArea->setPlainText(in.readAll());
    currentFile = fileName;
}

void Editor::save()
{
    QString fileName;
    // If  no fileName
    if (currentFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save");
        currentFile = fileName;
//        // Verify if the file already exists
//        QFile file{fileName};
//        if (file.exists())
//        {
//            QMessageBox overwrite;
//            overwrite.setText(currentFile + " already exists");
//            overwrite.setInformativeText("Do you want to overwrite its content ?");
//            overwrite.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//            overwrite.setDefaultButton(QMessageBox::No);
//            int answer = overwrite.exec();
//            if (answer == QMessageBox::No)
//                return;
//        }

    } else {
        fileName = currentFile;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(QFileInfo(fileName).baseName());
    QTextStream out(&file);
    QString text = editArea->toPlainText();
    out << text;
    file.close();
}

void Editor::saveAs()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, "Save");
    currentFile = fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(QFileInfo(fileName).baseName());
    QTextStream out(&file);
    QString text = editArea->toPlainText();
    out << text;
    file.close();
}

void Editor::quit()
{
    qApp->quit();
}

void Editor::cut()
{
    editArea->cut();
}

void Editor::copy()
{
    editArea->copy();
}

void Editor::paste()
{
    editArea->paste();
}

void Editor::selectAll()
{
    editArea->selectAll();
}

void Editor::undo()
{
    editArea->undo();
}

void Editor::redo()
{
    editArea->redo();
}

void Editor::font()
{
    bool fontSelected;
       QFont font = QFontDialog::getFont(&fontSelected, this);
       if (fontSelected)
           editArea->setFont(font);
}

void Editor::hideStatusBar()
{
    if (statusBar()->isVisible())
        statusBar()->hide();
    else
        statusBar()->show();
    emit checkboxToggled(AhideStatusBar);
}

void Editor::autoSave()
{
    static int id;
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
    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file automatically : " + file.errorString());
        return;
    }
    QTextStream out(&file);
    QString text = editArea->toPlainText();
    out << text;
    file.close();
    statusBarText->setText("Last Saved " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz  ddd dd MMM yyyy"));
}
