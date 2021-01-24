#include "settings.h"
#include "settingsmodel.h"
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Settings::Settings() :
    QMainWindow()
{
    this->setWindowFlag(Qt::Widget);
    this->setStyleSheet("background-color: #ffffff");
    auto *container = new QWidget(this);
    setCentralWidget(container);
    auto *layout = new QFormLayout;
    tabStopChoice = new QLineEdit;
    tabStopChoice->setPlaceholderText("Default: 2");
    themeChoice = new QComboBox;
    for (auto theme: SettingsModel::themes())
        themeChoice->addItem(theme);
    fontSizeChoice = new QLineEdit;
    fontSizeChoice->setPlaceholderText("Default: 14");
    auto *controlsLayout = new QHBoxLayout;
    save = new QPushButton("Save Changes");
    backToDefault = new QPushButton("Default");
    controlsLayout->addWidget(save);
    controlsLayout->addWidget(backToDefault);
    layout->addRow("Theme", themeChoice);
    layout->addRow("Tab Length", tabStopChoice);
    layout->addRow("Font Size", fontSizeChoice);
    layout->addRow(controlsLayout);
    container->setLayout(layout);
}
