#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([Octave-Forge video package], [1.1.0+])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_LANG(C++)

# Define macros needed for libav
AC_DEFINE(__STDC_CONSTANT_MACROS, [], [workaround for C++ programs to use C99 macros])
AC_DEFINE(VIDEO_OUTBUF_SIZE, [200000], [size of video buffer])

AC_CHECK_PROG([HAVE_MKOCTFILE], [mkoctfile], [yes], [no])
if [test $HAVE_MKOCTFILE = "no"]; then
  AC_MSG_ERROR([mkoctfile required to install $PACKAGE_NAME])
fi

# Checks for libraries.

AC_CHECK_LIB([avformat], [av_write_frame], [],
             [AC_MSG_ERROR([$PACKAGE_NAME requires libavformat from FFmpeg])])
AC_CHECK_LIB([avcodec], [av_malloc], [],
             [AC_MSG_ERROR([$PACKAGE_NAME requires libavcodec from FFmpeg])])
AC_CHECK_LIB([swscale], [sws_scale], [],
             [AC_MSG_ERROR([$PACKAGE_NAME requires libswscale from FFmpeg])])
             
# Checks for header files.

AC_CHECK_HEADERS([ffmpeg/avformat.h], [], [AC_CHECK_HEADERS([libavformat/avformat.h], [],
  AC_MSG_ERROR([FFMpeg header avformat.h not found]))])

AC_CHECK_HEADERS([ffmpeg/avcodec.h], [], [AC_CHECK_HEADERS([libavcodec/avcodec.h], [],
  AC_MSG_ERROR([FFMpeg header avcodec.h not found]))])

AC_CHECK_DECL(AVSEEK_FLAG_BACKWARD, [],
              AC_MSG_ERROR([need a newer version of FFMpeg]), [
#if defined(HAVE_FFMPEG_AVFORMAT_H)
#include <ffmpeg/avformat.h>
#elif defined(HAVE_LIBAVFORMAT_AVFORMAT_H)
#include <libavformat/avformat.h>
#endif])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

$PACKAGE_NAME is now configured with

   FFMPEG LIBS:       $LIBS
   DEFS:              $DEFS
])
