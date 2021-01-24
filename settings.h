#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

class Settings : public QMainWindow
{
    Q_OBJECT
public:
    Settings();

private:
    QLineEdit *tabStopChoice;
    QComboBox *themeChoice;
//    QComboBox *fontChoice;
    QLineEdit *fontSizeChoice;
    QPushButton *save;
    QPushButton *backToDefault;
};

#endif // SETTINGS_H
