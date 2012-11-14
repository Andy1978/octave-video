dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([Octave-Forge video package], [1.1.0+])

AC_PROG_CXX
AC_LANG(C++)

AC_CHECK_PROG([HAVE_MKOCTFILE], [mkoctfile], [yes], [no])
if [test $HAVE_MKOCTFILE = "no"]; then
  AC_MSG_ERROR([mkoctfile required to install $PACKAGE_NAME])
fi

dnl Check for ffmpeg
AC_CHECK_PROG([FFMPEG_CONFIG], [pkg-config], [pkg-config])
if test -z $FFMPEG_CONFIG ; then
  AC_MSG_WARN([`pkg-config' not found -- using default configuration])
  FFMPEG_FLAGS="-lavcodec -lavutil -lm"
  echo $FFMPEG_FLAGS
else
  FFMPEG_FLAGS=`pkg-config libavformat libavcodec libavutil libswscale --libs --cflags`
  echo $FFMPEG_FLAGS
fi

AC_CHECK_LIB([avformat], [av_write_frame], [],
             [AC_MSG_ERROR([$PACKAGE_NAME requires libavformat from FFmpeg])],
             [$FFMPEG_FLAGS])
AC_CHECK_LIB([avcodec], [av_malloc], [],
             [AC_MSG_ERROR([$PACKAGE_NAME requires libavcodec from FFmpeg])],
             [$FFMPEG_FLAGS])
AC_CHECK_LIB([swscale], [sws_scale], [],
             [AC_MSG_ERROR([$PACKAGE_NAME requires libswscale from FFmpeg])],
             [$FFMPEG_FLAGS])

OLD_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $FFMPEG_FLAGS"
FFMPEG_DEFS=
AC_SUBST(FFMPEG_DEFS)
AC_CHECK_HEADERS(ffmpeg/avformat.h,
  FFMPEG_DEFS="$FFMPEG_DEFS -DHAVE_FFMPEG_AVFORMAT_H", [
  AC_CHECK_HEADERS(libavformat/avformat.h, 
    FFMPEG_DEFS="$FFMPEG_DEFS -DHAVE_LIBAVFORMAT_AVFORMAT_H",
    AC_MSG_ERROR([FFMpeg header files not found]))])
AC_CHECK_HEADERS(ffmpeg/avcodec.h, 
  FFMPEG_DEFS="$FFMPEG_DEFS -DHAVE_FFMPEG_AVCODEC_H", [
  AC_CHECK_HEADERS(libavcodec/avcodec.h, 
    FFMPEG_DEFS="$FFMPEG_DEFS -DHAVE_LIBAVCODEC_AVCODEC_H",
    AC_MSG_ERROR([FFMpeg header files not found]))])
AC_CHECK_DECL(AVSEEK_FLAG_BACKWARD, [],
              AC_MSG_ERROR([need a newer version of FFMpeg]), [
#if defined(HAVE_FFMPEG_AVFORMAT_H)
#include <ffmpeg/avformat.h>
#elif defined(HAVE_LIBAVFORMAT_AVFORMAT_H)
#include <libavformat/avformat.h>
#endif])
CPPFLAGS=$OLD_CPPFLAGS

AC_CONFIG_FILES([Makefile])
AC_SUBST([FFMPEG_CONFIG])
AC_OUTPUT
