# When compiling for publishing the app, activate this
# to change to the 0x2... UID and the right UID for the
# Smart Installer. Using those UIDs requires a development
# certificate.
# For self signed versions, remove / comment the following line.
#DEFINES += DEPLOY_VERSION

TARGET = nfcchat
VERSION = 2.00.0

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the
# MOBILITY variable.
CONFIG += mobility qt-components
MOBILITY += connectivity systeminfo

# Define QMLJSDEBUGGER to allow debugging of QML in debug builds
# (This might significantly increase build time)
# DEFINES += QMLJSDEBUGGER

# Define for detecting Harmattan in .cpp files.
# Only needed for experimental / beta Harmattan SDKs.
# Will be defined by default in the final SDK.
exists($$QMAKE_INCDIR_QT"/../qmsystem2/qmkeys.h"):!contains(MEEGO_EDITION,harmattan): {
  MEEGO_VERSION_MAJOR     = 1
  MEEGO_VERSION_MINOR     = 2
  MEEGO_VERSION_PATCH     = 0
  MEEGO_EDITION           = harmattan
  DEFINES += MEEGO_EDITION_HARMATTAN
}

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

OTHER_FILES += \
    qml/images/*.svg \
    qml/images/*.png \
    qml/symbian/SettingsPage.qml

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    nfcpeertopeer.cpp \
    appsettings.cpp

HEADERS += \
    nfcpeertopeer.h \
    appsettings.h



simulator {
    # The simulator uses the QML and images from Symbian,
    # as it doesn't have support for simulating Qt Quick Components for
    # MeeGo Harmattan.
    OTHER_FILES += \
        qml/symbian/*.qml

    qmlFolder.source = qml/symbian/*
    qmlFolder.target = qml
    QML_IMPORT_PATH = qml/symbian

    OTHER_FILES += \
        qml/images-symbian/*.qml
    imagesFolderSymbian.source = qml/images-symbian/*
    imagesFolderSymbian.target = qml
    DEPLOYMENTFOLDERS += imagesFolderSymbian
}

symbian {
    DEPLOYMENT.display_name = "NfcChat"
    contains(DEFINES,DEPLOY_VERSION) {
        TARGET.UID3 = 0x2005CE04
    } else {
        TARGET.UID3 = 0xE0C6DF99
    }

    TARGET.CAPABILITY += NetworkServices LocalServices

    # Smart Installer package's UID
    # This UID is from the protected range and therefore the package will
    # fail to install if self-signed. By default qmake uses the unprotected
    # range value if unprotected UID is defined for the application and
    # 0x2002CCCF value if protected UID is given to the application
    contains(DEFINES,DEPLOY_VERSION) {
        DEPLOYMENT.installer_header = 0x2002CCCF
    }

    # add NfcSettings support
    include(nfcsettings/nfcsettings.pri)

    OTHER_FILES += \
        qml/symbian/*.qml

    qmlFolder.source = qml/symbian/*
    qmlFolder.target = qml
    QML_IMPORT_PATH = qml/symbian

    # Symbian specific images
    OTHER_FILES += \
        qml/images-symbian/*.qml
    imagesFolderSymbian.source = qml/images-symbian/*
    imagesFolderSymbian.target = qml
    DEPLOYMENTFOLDERS += imagesFolderSymbian

    # Autostart
    ndefhandler.sources = ndefhandler_nfcchat.xml
    ndefhandler.path = c:/private/2002AC7F/import/
    DEPLOYMENT += ndefhandler

    # Localisation support.
    CODECFORTR = UTF-8
    TRANSLATIONS += loc/$${TARGET}_en.ts \
                    loc/$${TARGET}_ar.ts \
                    loc/$${TARGET}_zh_HK.ts \
                    loc/$${TARGET}_zh_CN.ts \
                    loc/$${TARGET}_zh_TW.ts \
                    loc/$${TARGET}_cs.ts \
                    loc/$${TARGET}_da.ts \
                    loc/$${TARGET}_nl.ts \
                    loc/$${TARGET}_en_US.ts \
                    loc/$${TARGET}_fi.ts \
                    loc/$${TARGET}_fr.ts \
                    loc/$${TARGET}_fr_CA.ts \
                    loc/$${TARGET}_de.ts \
                    loc/$${TARGET}_el.ts \
                    loc/$${TARGET}_he.ts \
                    loc/$${TARGET}_hi.ts \
                    loc/$${TARGET}_hu.ts \
                    loc/$${TARGET}_id.ts \
                    loc/$${TARGET}_it.ts \
                    loc/$${TARGET}_ms.ts \
                    loc/$${TARGET}_no.ts \
                    loc/$${TARGET}_pl.ts \
                    loc/$${TARGET}_pt.ts \
                    loc/$${TARGET}_pt_BR.ts \
                    loc/$${TARGET}_ro.ts \
                    loc/$${TARGET}_ru.ts \
                    loc/$${TARGET}_sk.ts \
                    loc/$${TARGET}_es.ts \
                    loc/$${TARGET}_es_419.ts \
                    loc/$${TARGET}_sv.ts \
                    loc/$${TARGET}_th.ts \
                    loc/$${TARGET}_tr.ts \
                    loc/$${TARGET}_uk.ts \
                    loc/$${TARGET}_vi.ts

    translationfiles.source = loc/*.qm
    DEPLOYMENTFOLDERS += translationfiles

    vendorName = "Andreas Jakl"
    vendorinfo = \
        "; Localised Vendor name" \
        "%{$$addLanguageDependentPkgItem(vendorName)}" \
        " " \
        "; Unique Vendor name" \
        ":\"$$vendorName\"" \
        " "

    deployment_vendor.pkg_prerules += vendorinfo
    DEPLOYMENT += deployment_vendor
}


contains(MEEGO_EDITION,harmattan) {
    # Temp
    DEFINES += MEEGO_EDITION_HARMATTAN

    # Speed up launching on MeeGo/Harmattan when using applauncherd daemon
    CONFIG += qdeclarative-boostable

    OTHER_FILES += qtc_packaging/debian_harmattan/*

    OTHER_FILES += \
        qml/meego/*.qml

    qmlFolder.source = qml/meego/*
    qmlFolder.target = qml
    QML_IMPORT_PATH = qml/meego

    # Don't use nfcinfo_harmattan.desktop. Otherwise,
    # the NDEF Autostart handler won't find the desktop file and
    # will not be able to auto-launch this app on tag-touch.
    # See: https://bugreports.qt.nokia.com/browse/QTMOBILITY-1848
    harmattandesktopfile.files = nfcchat.desktop
    harmattandesktopfile.path = /usr/share/applications
    INSTALLS += harmattandesktopfile

    # To avoid conflicts, recommended to name this file according to the
    # full service name instead of just the app name.
    # See: https://bugreports.qt.nokia.com/browse/QTMOBILITY-1848
    ndefhandler_service.files = com.nokia.qtmobility.nfc.nfcchat.service
    ndefhandler_service.path = /usr/share/dbus-1/services/
    INSTALLS += ndefhandler_service

    launchericon.files = nfcchat80.png splash-nfcchat-l.png splash-nfcchat-p.png
    launchericon.path = /opt/nfcchat/
    INSTALLS += launchericon
}

imagesFolder.source = qml/images/*
imagesFolder.target = qml
DEPLOYMENTFOLDERS += qmlFolder imagesFolder

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()



