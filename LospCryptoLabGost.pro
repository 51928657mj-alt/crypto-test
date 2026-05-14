#-------------------------------------------------
#
# Project created by QtCreator 2015-03-05T11:08:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LospCryptoLabGost
TEMPLATE = app


SOURCES += main.cpp \
    Utils/FilePath.cpp \
    Utils/FileSize.cpp \
    Utils/HexUtils.cpp \
    Crypt/CryptGost.cpp \
    Crypt/CryptGostGamma.cpp \
    Crypt/CryptGostGammFeedBack.cpp \
    Crypt/CryptGostSR.cpp \
    Crypt/GryptGostImito.cpp \
    Crypt/SRUtil.cpp \
    CryptDlg/GostCrypt.cpp \
    ThreadsUtils/GostSleeperThread.cpp \
    Exc/GenericExc.cpp \
    Utils/TreeItemUtils/algorithmgamm.cpp \
    Utils/TreeItemUtils/algorithmsr.cpp \
    Utils/TreeItemUtils/algorithmtreeitem.cpp \
    Utils/TreeItemUtils/algoritmgammfeedback.cpp \
    Utils/TreeItemUtils/algorithmimito.cpp

HEADERS  += Utils/FilePath.h \
    Utils/FileSize.h \
    Utils/HexUtils.h \
    Crypt/CryptGost.h \
    Crypt/CryptGostGamma.h \
    Crypt/CryptGostGammFeedBack.h \
    Crypt/CryptGostSR.h \
    Crypt/CryptStruct.h \
    Crypt/GryptGostImito.h \
    Crypt/ICrypt.h \
    Crypt/SRUtil.h \
    CryptDlg/GostCrypt.h \
    ThreadsUtils/GostSleeperThread.h \
    Exc/GenericExc.h \
    Types/BitTypes.h \
    Utils/TreeItemUtils/algorithmgamm.h \
    Utils/TreeItemUtils/algorithmsr.h \
    Utils/TreeItemUtils/algorithmtreeitem.h \
    Utils/TreeItemUtils/algoritmgammfeedback.h \
    Utils/TreeItemUtils/algorithmimito.h

FORMS    +=  CryptDlg/GostCrypt.ui
RC_FILE   =  ico.rc
RESOURCES += resources.qrc
