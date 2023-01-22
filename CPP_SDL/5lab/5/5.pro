TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

LIBS += -L D:/progDEV/sdl/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
INCLUDEPATH += D:/progDEV/sdl/include/SDL2


