TOP_DIR = ../../
INSTALL_DIR = $$(TOP_DIR)/install
INSTALL_BIN_DIR = $${INSTALL_DIR}/bin
INSTALL_LIB_DIR = $${INSTALL_DIR}/lib
INSTALL_INC_DIR = $${INSTALL_DIR}/include

QT += widgets
QT += xml

INCLUDEPATH *= $$PWD/src
INCLUDEPATH += $$PWD/../

include($$PWD/../sautoModel/sautoModel.pri)
include($$PWD/../sautoXml/sautoXml.pri)
include($$PWD/../sautoWidgets/sautoWidgets.pri)
include($$PWD/../sauto/sauto.pri)

TEMPLATE = app                # build an application
CONFIG  += debug_and_release  # create both debug and release targets
CONFIG  += build_all          # build both debug and release by default
CONFIG  += c++11

PROJNAME = $$basename(PWD)   # name of project
BASENAME = $$PROJNAME        # base name of output file

TEMP = $$PWD/src/$$PROJNAME/*.h     # projdir header files
for(a,TEMP) {
   exists($$a) {
      HEADERS *= $$a
   }
}

TEMP = $$PWD/src/$$PROJNAME/*.cpp   # projdir source files
for(a,TEMP) {
   exists($$a) {
      SOURCES *= $$a
   }
}

TEMP = $$PWD/src/rc/*.qrc           # projdir resource files
for(a,TEMP) {
   exists($$a) {
      RESOURCES *= $$a
   }
}

TEMPDIR  = $$PWD/tmp
DESTDIR  = $$TEMPDIR/bin
MOC_DIR  = $$TEMPDIR/moc
UI_DIR   = $$TEMPDIR/uic
RCC_DIR  = $$TEMPDIR/rcc
build_pass:CONFIG(debug, debug|release) {
  OBJECTS_DIR = $$TEMPDIR/obj/debug
} else {
  build_pass:CONFIG(release, debug|release) {
    OBJECTS_DIR = $$TEMPDIR/obj/release
  }
}

build_pass:CONFIG(debug, debug|release) {
  win32:TARGET = $$join(BASENAME,,,d)
} else {
  build_pass:CONFIG(release, debug|release) {
    win32:TARGET = $$BASENAME
  }
}

win32 {
  allclean.depends  = distclean vsclean
  vsclean.commands  = rm -f *.vcproj*          
  QMAKE_EXTRA_TARGETS += vsclean
}
unix {
  allclean.commands = rm -rf $$TEMPDIR  
}
QMAKE_EXTRA_TARGETS += allclean