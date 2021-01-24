#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QList>
#include <QString>

class SettingsModel
{
public:
    SettingsModel();
    QList<QString> static themes();
};

#endif // SETTINGSMODEL_H
