QT       += core gui sql widgets
CONFIG   += c++17
CONFIG   -= app_bundle
DEFINES  += QT_DEPRECATED_WARNINGS

TEMPLATE  = app
TARGET    = isrdo

SOURCES += \
    main.cpp \
    models/databasemanager.cpp \
    models/role.cpp \
    models/user.cpp \
    models/position.cpp \
    models/qualification.cpp \
    models/employee.cpp \
    models/worktype.cpp \
    models/additionalwork.cpp \
    models/assignment.cpp \
    controllers/authcontroller.cpp \
    controllers/workcontroller.cpp \
    controllers/reportcontroller.cpp \
    views/loginwindow.cpp \
    views/mainwindow.cpp \
    views/workdialog.cpp \
    views/worklistview.cpp \
    views/reportview.cpp

HEADERS += \
    models/databasemanager.h \
    models/role.h \
    models/user.h \
    models/position.h \
    models/qualification.h \
    models/employee.h \
    models/worktype.h \
    models/additionalwork.h \
    models/assignment.h \
    controllers/authcontroller.h \
    controllers/workcontroller.h \
    controllers/reportcontroller.h \
    views/loginwindow.h \
    views/mainwindow.h \
    views/workdialog.h \
    views/worklistview.h \
    views/reportview.h

CONFIG   += release
