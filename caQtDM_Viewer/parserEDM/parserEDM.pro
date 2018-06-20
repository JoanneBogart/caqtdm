#   Has to do with version control if in caqtdm git repo
#   include(../qtdefs.pri)

# Following (maybe) affects build parameters picked up from caQtDM.pri
CONFIG += caQtDM_xdl2ui 
CONFIG += debug


# Leave the following line in for now although most of it is irrelevant.  
# Will need to change when we integrate with PyDM or become standalone repo
include(../../caQtDM.pri)

# Not sure why test isn't working properly
# contains(QT_VER_MAJ, 5) {
  QT       += widgets
  DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
# }
!win32 {
  QMAKE_CXXFLAGS += "-Wno-write-strings"
}

TEMPLATE = app
INCLUDEPATH += .
MOC_DIR = moc

# Input
HEADERS += XmlWriter.h  \
    myParserEDM.h \
    QtProperties.h \
    dmsearchfile.h \
    tag_pkg.h \
    utility.h \
    expString.h \
    parserClass.h \
    tag_pkg.str \
    environment.str
SOURCES += myParserEDM.cpp XmlWriter.cpp \
    dmsearchfile.cpp \
    tag_pkg.cc \
    utility.cc \
    expString.cc \
    parserClass.cc

TARGET = edl2ui

OTHER_FILES += \
    stylesheet.qss



