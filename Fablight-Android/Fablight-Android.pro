QT += widgets
QT += bluetooth

HEADERS     = slidersgroup.h \
              window.h \
    bluetoothselector.h
SOURCES     = main.cpp \
              slidersgroup.cpp \
              window.cpp \
    bluetoothselector.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/sliders
INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml
