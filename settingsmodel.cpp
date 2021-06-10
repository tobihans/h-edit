#include "settingsmodel.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileInfoList>
#include <QList>
#include <QString>
#include <QDir>


SettingsModel::SettingsModel()
{

}

QList<QString> SettingsModel::themes()
{
    QList<QString> themes;
    QDir themeDirectory{"./themes"};
    themeDirectory.setFilter(QDir::Files);
    QFileInfoList files = themeDirectory.entryInfoList();
    for (const auto &file: files)
    {
        if (file.suffix() == "qss")
            themes.append(file.fileName());
    }
    return themes;
}
