#ifndef WORKINGDIRWIDGET_H
#define WORKINGDIRWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QFileSystemWatcher>

class WorkingDirWidget : public QTreeWidget
{
    Q_OBJECT
public:
    WorkingDirWidget(QWidget *parent = nullptr);
    WorkingDirWidget(QString directory, QWidget *parent = nullptr);
    WorkingDirWidget(QStringList directories, QWidget *parent = nullptr);
    virtual ~WorkingDirWidget();
    void addDirectory(QString directory);
    void addDirectories(QStringList directories);
    void attachModalsTo(QWidget *widget);

public slots:
    void addprojectFolderRequested();
    void removeprojectFolderRequested();

private slots:
    void showContextMenu(const QPoint& pos);
    void showFolderContent(QTreeWidgetItem *item);
    void renameItemRequested();
    void deleteItemRequested();
    void newFolderRequested();
    void newFileRequested();
    void openFileRequested();
    void stopMonitoringFolder(QTreeWidgetItem *folder);

private:
    void init();
    void initContextMenu();
    void initContextMenuActions();
    void connectContextMenuActions();
    void makeConnections();
    QString itemAbsPath(QTreeWidgetItem *item);
    void insertFolders(QTreeWidgetItem *item);
    void insertFiles(QTreeWidgetItem *item);
    void clearItem(QTreeWidgetItem *item);
    QTreeWidgetItem* getTopLevelParent(QTreeWidgetItem *item);
    QString getParentPathForTopLevelItem(QTreeWidgetItem *item);
    void connectToMonitoringEvents();
    void warn(QString title, QString msg);

    QStringList absolutePaths; // Record the absolute path for toplevel items
    // Context Menu
    QMenu *contextMenu;
    QAction *addProjectFolder;
    QAction *removeProjectFolder;
    QAction *newFolder;
    QAction *newFile;
    QAction *open;
    QAction *deleteItem;
    QAction *renameItem;

    QWidget *modalsParent;

    // Monitor changes in expanded dirs
    QFileSystemWatcher monitor;
    QMap<QString,QTreeWidgetItem*> monitoredDirsWidgets;

signals:
    void openFile(QString path);
    void fileDeleted(QString path);
    void fileRenamed(QString oldPath, QString newPath);
    void folderDeleted(QString path);
    void folderRenamed(QString oldPath, QString newPath);
};

#endif // WORKINGDIRWIDGET_H
