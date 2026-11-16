QT += widgets network     # network : QUdpSocket

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# protocole.c : decoupage de SCAN en C (client_udp.c et test_client.c
# ne servent qu'au test console, ils ne sont pas dans l'appli Qt)
SOURCES += \
    main.cpp \
    interfacescan.cpp \
    protocole.c

HEADERS += \
    interfacescan.h \
    protocole.h

FORMS += \
    interfacescan.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
