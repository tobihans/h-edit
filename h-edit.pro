QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codeedit.cpp \
    main.cpp \
    editor.cpp \
    settings.cpp \
    settingsmodel.cpp \
    workingdirwidget.cpp

HEADERS += \
    codeedit.h \
    editor.h \
    settings.h \
    settingsmodel.h \
    workingdirwidget.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    README \
    icons/icons8-checked-64.png \
    icons/icons8-choose-font-48.png \
    icons/icons8-copy-64.png \
    icons/icons8-cut-64.png \
    icons/icons8-decrease-font-48.png \
    icons/icons8-exit-64.png \
    icons/icons8-file-64.png \
    icons/icons8-increase-font-48.png \
    icons/icons8-opened-folder-64.png \
    icons/icons8-paste-64.png \
    icons/icons8-redo-64.png \
    icons/icons8-save-64.png \
    icons/icons8-save-as-64.png \
    icons/icons8-select-all-64.png \
    icons/icons8-unchecked-checkbox-64.png \
    icons/icons8-undo-64.png \
    icons/icons8-zoom-in-64.png \
    icons/icons8-zoom-out-64.png
