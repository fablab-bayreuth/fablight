QT += widgets
QT += bluetooth

HEADERS     = slidersgroup.h \
              window.h \
    bluetoothselector.h \
    colorsender.h
SOURCES     = main.cpp \
              slidersgroup.cpp \
              window.cpp \
    bluetoothselector.cpp \
    colorsender.cpp

# install
target.path = $$PWD/Fablight-Android
INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml
