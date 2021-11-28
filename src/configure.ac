#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([Octave-Forge video package], [2.0.2])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_LANG(C++)

# Define macros needed for libav
#AC_DEFINE(__STDC_CONSTANT_MACROS, [], [workaround for C++ programs to use C99 macros])

AC_CHECK_PROG([MKOCTFILE], [mkoctfile], [yes], [no])
if [test $MKOCTFILE = "no"]; then
  AC_MSG_ERROR([mkoctfile required to install $PACKAGE_NAME])
fi

# Checks for libraries.

PKG_CHECK_MODULES([FFMPEG], [libswscale, libavformat, libavcodec, libavutil], [],
  [AC_MSG_ERROR([FFmpeg libswscale, libavformat, libavcodec or libavutil not found])])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
#AC_TYPE_UINT64_T
#AC_TYPE_UINT8_T

AC_SUBST(CPPFLAGS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

$PACKAGE_NAME is now configured with

   FFMPEG LIBS:       $FFMPEG_LIBS
   FFMPEG CFLAGS:     $FFMPEG_CFLAGS
   DEFS:              $DEFS
])
