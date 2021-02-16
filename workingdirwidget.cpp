#include "workingdirwidget.h"
#include <QFont>
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtDebug>

WorkingDirWidget::WorkingDirWidget(QWidget *parent) :
    QTreeWidget(parent), modalsParent(nullptr)
{

    // General style
    QFont font("monospace");
    font.setPixelSize(14.75);
    this->setFont(font);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setColumnCount(1);
    auto *header = this->header();
    header->setSectionResizeMode(QHeaderView::Stretch);
    header->hide();
    initContextMenu();
    makeConnections();
    connectToMonitoringEvents();
}

WorkingDirWidget::WorkingDirWidget(QString directory, QWidget *parent) :
    WorkingDirWidget(parent)
{
    this->addDirectory(directory);
}

WorkingDirWidget::WorkingDirWidget(QStringList directories, QWidget *parent) :
    WorkingDirWidget(parent)
{
    this->addDirectories(directories);
}

WorkingDirWidget::~WorkingDirWidget()
{
    qDebug() << monitoredDirsWidgets;
    delete addProjectFolder;
    delete removeProjectFolder;
    delete newFolder;
    delete newFile;
    delete deleteItem;
    delete renameItem;
    delete contextMenu;
}

void WorkingDirWidget::addDirectory(QString directory)
{
    QDir dir{directory};
    this->absolutePaths.append(directory);
    auto *topLevelItem = new QTreeWidgetItem(QStringList(dir.dirName()));
    topLevelItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    this->addTopLevelItem(topLevelItem);
    topLevelItem->setExpanded(true);
    this->setCurrentItem(topLevelItem);
}

void WorkingDirWidget::addDirectories(QStringList directories)
{
    for (auto dir : directories)
        this->addDirectory(dir);
}


void WorkingDirWidget::attachModalsTo(QWidget *widget)
{
    this->modalsParent = widget;
}

void WorkingDirWidget::initContextMenu()
{
    QFont font("monospace");
    font.setPixelSize(14.75);
    contextMenu = new QMenu(this);
    contextMenu->setFont(font);
    initContextMenuActions();
}

void WorkingDirWidget::initContextMenuActions()
{
    addProjectFolder = new QAction(tr("&Add Project Folder"));
    removeProjectFolder = new QAction(tr("&Remove Project Folder"));
    newFolder = new QAction(tr("New &Folder"));
    newFile = new QAction(tr("New &File"));
    open = new QAction(tr("&Open"));
    deleteItem = new QAction(tr("Delete"));
    renameItem = new QAction(tr("Rename"));

    contextMenu->addActions(QList<QAction*>{open, newFile, newFolder});
    contextMenu->addSeparator();
    contextMenu->addActions(QList<QAction*>{renameItem, deleteItem});
    contextMenu->addSeparator();
    contextMenu->addActions(QList<QAction*>{addProjectFolder, removeProjectFolder});
    connectContextMenuActions();
}

void WorkingDirWidget::connectContextMenuActions()
{
    connect(addProjectFolder, &QAction::triggered,
            this, &WorkingDirWidget::addprojectFolderRequested);
    connect(removeProjectFolder, &QAction::triggered,
            this, &WorkingDirWidget::removeprojectFolderRequested);
    connect(renameItem, &QAction::triggered,
            this, &WorkingDirWidget::renameItemRequested);
    connect(deleteItem, &QAction::triggered,
            this,&WorkingDirWidget::deleteItemRequested);
    connect(newFolder, &QAction::triggered,
            this, &WorkingDirWidget::newFolderRequested);
    connect(newFile, &QAction::triggered, this,
            &WorkingDirWidget::newFileRequested);
    connect(open, &QAction::triggered, this, &WorkingDirWidget::openFileRequested);
}

void WorkingDirWidget::makeConnections()
{
    // For folders
    connect(this, &WorkingDirWidget::itemExpanded,
            this, &WorkingDirWidget::showFolderContent);
    connect(this, &WorkingDirWidget::itemCollapsed, this,
            &WorkingDirWidget::stopMonitoringFolder);
    // For files
    connect(this, &WorkingDirWidget::itemDoubleClicked,
            this, &WorkingDirWidget::openFileRequested);
    // Context Menu => perform basic fileSystem operations(del, create, rename)
    connect(this, &WorkingDirWidget::customContextMenuRequested,
            this, &WorkingDirWidget::showContextMenu);
}

QString WorkingDirWidget::itemAbsPath(QTreeWidgetItem *item)
{
    if(item->parent() == nullptr) // TopLevelItem
    {
        return absolutePaths[this->indexOfTopLevelItem(item)];
    }
    QString path;
    while(item->parent() != nullptr)
    {
        path.prepend(item->text(0) + "/");
        item = item->parent();
    }
    path.prepend(absolutePaths[this->indexOfTopLevelItem(item)] + "/");
    return path.remove(path.length() - 1, 1); // Remove the "/" at the end of the path
}

void WorkingDirWidget::insertFolders(QTreeWidgetItem *item)
{
    QDir working_dir{this->itemAbsPath(item)};
    auto dirs = working_dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (auto dir: dirs)
    {
        auto *item_child = new QTreeWidgetItem(QStringList(dir.fileName()));
        item_child->setExpanded(true);
        item_child->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        item->addChild(item_child);
    }
}

void WorkingDirWidget::insertFiles(QTreeWidgetItem *item)
{
    QDir working_dir{this->itemAbsPath(item)};
    auto files = working_dir.entryInfoList(QDir::Files, QDir::Name);
    for (auto file: files)
    {
        auto *item_child = new QTreeWidgetItem(QStringList(file.fileName()));
        item_child->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
        item->addChild(item_child);
    }
}

void WorkingDirWidget::clearItem(QTreeWidgetItem *item)
{
    auto children = item->takeChildren();
    for (auto *child : children)
        delete child;
}

void WorkingDirWidget::connectToMonitoringEvents()
{
    connect(&monitor, &QFileSystemWatcher::directoryChanged, this, [=](QString path){
        auto *item = monitoredDirsWidgets[path];
        this->clearItem(item);
        this->insertFolders(item);
        this->insertFiles(item);
    });
}

void WorkingDirWidget::warn(QString title, QString msg)
{
    QMessageBox::warning(modalsParent, title, msg);
}

void WorkingDirWidget::addprojectFolderRequested()
{
    QString folder = QFileDialog::getExistingDirectory(modalsParent, QString("Open Folder"), QDir::home().absolutePath());
    if (!folder.isEmpty() && !absolutePaths.contains(folder))
        this->addDirectory(folder);
}

void WorkingDirWidget::removeprojectFolderRequested()
{
    auto *item = this->currentItem();
    if (!item)
        return;
    auto *topLevelItem = this->getTopLevelParent(item);
    int index = this->indexOfTopLevelItem(topLevelItem);
    absolutePaths.removeAt(index);
    this->clearItem(topLevelItem);
    delete topLevelItem;
    topLevelItem = nullptr;
}

QTreeWidgetItem* WorkingDirWidget::getTopLevelParent(QTreeWidgetItem *item)
{
    int index = this->indexOfTopLevelItem(item);
    if (index > -1)
        return item;
    else
        while(item->parent() != nullptr)
            item = item->parent();
    return item;
}

QString WorkingDirWidget::getParentPathForTopLevelItem(QTreeWidgetItem *item)
{
    int index{this->indexOfTopLevelItem(item)};
    if (index  > -1)
    {
        QString itemPath{absolutePaths[index]};
        QDir dir(itemPath);
        QString parentPath{dir.absolutePath()};
        parentPath = parentPath.left(parentPath.length() - dir.dirName().length());
        return parentPath;
    }
    return "";
}

void WorkingDirWidget::showContextMenu(const QPoint& pos)
{
    contextMenu->exec(this->mapToGlobal(QPoint(pos)));
}

void WorkingDirWidget::showFolderContent(QTreeWidgetItem *item)
{
    if(item->childIndicatorPolicy() == QTreeWidgetItem::DontShowIndicator)
        return;
    if (item->childCount() != 0)
        this->clearItem(item);
    this->insertFolders(item);
    this->insertFiles(item);
    monitor.addPath(this->itemAbsPath(item));
    monitoredDirsWidgets[this->itemAbsPath(item)] = item;
}

void WorkingDirWidget::openFileRequested()
{
    auto *item = this->currentItem();
    if (!item)
        return;
    if (item->childIndicatorPolicy() == QTreeWidgetItem::DontShowIndicator)
        emit openFile(this->itemAbsPath(item));
}

void WorkingDirWidget::renameItemRequested()
{
    auto *item = this->currentItem();
    if (!item)
        return;
    QString old{this->currentItem()->text(0)};
    bool ok;
    QString newName = QInputDialog::getText(modalsParent, tr("Rename folder"), tr("Enter the new file name:"),                                            QLineEdit::Normal, old, &ok);
    if (ok && !newName.isEmpty())
    {
        int index = indexOfTopLevelItem(item);
        if (index > -1) // It's a top level item
        {
            QString newPath{getParentPathForTopLevelItem(item) + newName};
            bool done = QDir::home().rename(this->itemAbsPath(item), newPath);
            if (done)
            {
                item->setText(0, newName);
                absolutePaths[index] = newPath;
                emit folderRenamed(old, newName);
            }
            else
                this->warn("Warning", tr("Unable to rename <b>") + this->itemAbsPath(item) + "</b> to <b>" + newPath + "</b>");
            return;
        }
        // Otherwise, it's not a top level widget
        if (item->childIndicatorPolicy() == QTreeWidgetItem::DontShowIndicator) // It's a file
        {
            QString old{this->itemAbsPath(item)}, newPath{this->itemAbsPath(item->parent()) + "/" + newName};
            bool done = QFile::rename(old, newPath);
            if (done)
            {
                item->setText(0, newName);
                emit fileRenamed(old, newName);
            }
            else
                this->warn("Warning", tr("Unable to rename <b>") + old + "</b> to <b>" + newPath + "</b>");
        }
        else //It's a folder
        {
            QString old{this->itemAbsPath(item)}, newPath{this->itemAbsPath(item->parent()) + "/" + newName};
            bool done = QDir::home().rename(old, newPath);
            if (done)
            {
                item->setText(0, newName);
                emit folderRenamed(old, newName);
            }
            else
                this->warn("Warning", tr("Unable to rename <b>") + old + "</b> to <b>" + newPath + "</b>");
        }
    }
}

void WorkingDirWidget::deleteItemRequested()
{
    auto *item = this->currentItem();
    if (!item)
        return;
    if (item->childIndicatorPolicy() == QTreeWidgetItem::ShowIndicator)
    {
        item->setExpanded(false); // Ensure that the dir is removed from monitored dirs if it's one.
        QString path{this->itemAbsPath(item)};
        QDir folder{path};
        bool done = folder.removeRecursively();
        if (done)
        {
            this->clearItem(item);
            auto *parent = item->parent();
            item->parent()->removeChild(item);
            delete item;
            this->setCurrentItem(parent);
            emit folderDeleted(path);
        }
        else
            this->warn("Warning", "Unable to remove folder <b>" + folder.absolutePath() + "</b>");
    }
    else
    {
        QString filePath{this->itemAbsPath(item)};
        bool done = QFile::remove(filePath);
        if (done)
        {
            auto *parent = item->parent();
            item->parent()->removeChild(item);
            delete item;
            this->setCurrentItem(parent);
            emit fileDeleted(filePath);
        }
        else
            this->warn("Warning", "Unable to remove file <b>\"" + filePath + "\"</b>");
    }
}

void WorkingDirWidget::newFolderRequested()
{
    // Get the folder
    auto *item = this->currentItem();
    if (!item)
        return;
    if (item->childIndicatorPolicy() == QTreeWidgetItem::DontShowIndicator)
        item = item->parent(); //Ensure that the pathis always a folder path
    QString newName = QInputDialog::getText(modalsParent, tr("<b>New Folder</b>"), tr("<i>Enter the name of the new folder below: </i>"));
    if (!newName.isEmpty())
    {
        QString path{this->itemAbsPath(item) + "/" + newName};
        bool done = QDir::home().mkpath(path);
        if (!done)
            this->warn("Warning", "Unable to create directory <b>" + path + "</b>");
    }
}

void WorkingDirWidget::newFileRequested()
{
    // Get the folder
    auto *item = this->currentItem();
    if (!item)
        return;
    if (item->childIndicatorPolicy() == QTreeWidgetItem::DontShowIndicator)
        item = item->parent();
    QString newName = QInputDialog::getText(modalsParent, tr("<b>New File</b>"), tr("<i>Enter the name of the new file below: </i>"));
    if (!newName.isEmpty())
    {
        if (!newName.contains("/"))
        {
            QString path{this->itemAbsPath(item) + "/" + newName};
            QFile file{path};
            bool done = file.open(QIODevice::WriteOnly);
            if (done)
                file.close();
            else
                this->warn("Warning", tr("Unable to create file <b>") + path + tr("</b>"));
        }
        else
        {
            int index = newName.lastIndexOf("/");
            QString parentDir{this->itemAbsPath(item) + "/" + newName.left(index + 1)},
            fileName{newName.right(newName.length() - index - 1)};
            if (fileName.isEmpty())
                this->warn("Warning", "Cannot create file without filename: " + tr("<b>") + parentDir + tr("\?\?</b>"));
            else
            {
                bool mkpathDone = QDir::home().mkpath(parentDir);
                if (mkpathDone)
                {
                    QFile file{parentDir + fileName};
                    bool done = file.open(QIODevice::WriteOnly);
                    if (done)
                        file.close();
                    else
                        this->warn("Warning", tr("Unable to create file <b>") + parentDir + fileName + tr("</b>"));
                }
                else
                    this->warn("Warning", tr("Unable to create file <b>") + parentDir + fileName + tr("</b>"));
            }
        }
    }
}

void WorkingDirWidget::stopMonitoringFolder(QTreeWidgetItem *folder)
{
    monitor.removePath(this->itemAbsPath(folder));
    this->clearItem(folder);
    monitoredDirsWidgets.remove(this->itemAbsPath(folder));
}
