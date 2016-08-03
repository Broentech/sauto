
INCLUDEPATH *= $$PWD/src
INCLUDEPATH += $$PWD/../

PROJNAME = $$basename(PWD)   
BASENAME = $$PROJNAME        

TEMP = $$PWD/src/$$PROJNAME/*.h     
for(a,TEMP) {
   exists($$a) {
      HEADERS *= $$a
   }
}
TEMP = $$PWD/src/$$PROJNAME/*.cpp   
for(a,TEMP) {
   exists($$a) {
      SOURCES *= $$a
   }
}

TEMPDIR = $$PWD/tmp
LIBDIR  = lib

equals(TEMPLATE, "app") | equals(TEMPLATE, "vcapp") {
  build_pass:CONFIG(debug, debug|release) {
    win32:LIBNAME = $$join(BASENAME,,,d.lib)
  } else {
    build_pass:CONFIG(release, debug|release) {
      win32:LIBNAME = $$join(BASENAME,,,.lib)
    }
  }
  LIBS *= -L$$TEMPDIR/$$LIBDIR               # library directory
  LIBS *= $$LIBNAME                          # library name
  DEPENDPATH *= $$PWD/src
  LIBNAME = ""  # just to be safe
}
