macx{
    INCLUDEPATH += /opt/local/include /usr/include /usr/include/ni /usr/local/include/opencv
    LIBS += -L/opt/local/lib -lglut -lGLU -lGL
    LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
    LIBS += -L/usr/lib -lOpenNI
    LIBS += -L/opt/local/lib/mysql5/mysql -lmysqlclient
}else{
    INCLUDEPATH += /usr/include /usr/local/include /usr/include/ni
    LIBS += -L/usr/lib -lglut -lGLU -lGL -lOpenNI -lmysqlclient
}
QT+=core
TARGET=Test
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt
TEMPLATE = app

SOURCES += \
    main.cpp \
    scenedrawer.cpp \
    database.cpp \
    glinit.cpp

HEADERS += \
    scenedrawer.h \
    database.h \
    glinit.h
