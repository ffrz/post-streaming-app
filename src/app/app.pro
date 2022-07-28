QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    bookmanager.cpp \
    db.cpp \
    locationmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    newprojectdialog.cpp \
    projectdocument.cpp \
    studyeditor.cpp \
    studytexttemplatemanager.cpp \
    teachermanager.cpp

HEADERS += \
    aboutdialog.h \
    bookmanager.h \
    data.h \
    db.h \
    global.h \
    locationmanager.h \
    mainwindow.h \
    newprojectdialog.h \
    projectdocument.h \
    studyeditor.h \
    studytexttemplatemanager.h \
    teachermanager.h

FORMS += \
    aboutdialog.ui \
    bookeditor.ui \
    bookmanager.ui \
    locationeditor.ui \
    locationmanager.ui \
    mainwindow.ui \
    newprojectdialog.ui \
    studyeditor.ui \
    studytexttemplateeditor.ui \
    studytexttemplatemanager.ui \
    teachereditor.ui \
    teachermanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
DESTDIR = $$PWD/../../deploy
TARGET = post-streaming-app
