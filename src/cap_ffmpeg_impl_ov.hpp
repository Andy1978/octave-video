/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

/* ATTENTION:
*
* This file was generated from
*
* https://github.com/opencv/opencv/blob/4.x/modules/videoio/src/cap_ffmpeg_impl.hpp
* commit 61d48dd0f8d1cc1a115d26998705a61478f64a3c
*
* and applying the patches in cap_ffmpeg_impl_ov.patch
*/

/*
	tested against following combinations:

	$ sudo ./run.sh | grep -iP "version|name"

	PRETTY_NAME="Debian GNU/Linux 10 (buster)"
	NAME="Debian GNU/Linux"
	OCTAVE_VERSION = 4.4.1
	FFmpeg_versions = Lavu56.22.100, SwS5.3.100, Lavc58.35.100, Lavf58.20.100  <-- lowest

	PRETTY_NAME="Debian GNU/Linux 11 (bullseye)"
	NAME="Debian GNU/Linux"
	OCTAVE_VERSION = 6.2.0
	FFmpeg_versions = Lavu56.51.100, SwS5.7.100, Lavc58.91.100, Lavf58.45.100

	PRETTY_NAME="Debian GNU/Linux 12 (bookworm)"
	NAME="Debian GNU/Linux"
	OCTAVE_VERSION = 7.3.0
	FFmpeg_versions = Lavu57.28.100, SwS6.7.100, Lavc59.37.100, Lavf59.27.100

	NAME=Fedora
	VERSION="31 (Container Image)"
	OCTAVE_VERSION = 5.1.0
	FFmpeg_versions = Lavu56.31.100, SwS5.5.100, Lavc58.54.100, Lavf58.29.100

	NAME=Fedora
	VERSION="33 (Container Image)"
	OCTAVE_VERSION = 5.2.0
	FFmpeg_versions = Lavu56.51.100, SwS5.7.100, Lavc58.91.100, Lavf58.45.100

	NAME="Fedora Linux"
	VERSION="38 (Container Image)"
	OCTAVE_VERSION = 7.3.0
	FFmpeg_versions = Lavu58.2.100, SwS7.1.100, Lavc60.3.100, Lavf60.3.100

	NAME="Ubuntu"
	VERSION="20.04.6 LTS (Focal Fossa)"
	OCTAVE_VERSION = 5.2.0
	FFmpeg_versions = Lavu56.31.100, SwS5.5.100, Lavc58.54.100, Lavf58.29.100

	PRETTY_NAME="Ubuntu 22.04.2 LTS"
	NAME="Ubuntu"
	OCTAVE_VERSION = 6.4.0
	FFmpeg_versions = Lavu56.70.100, SwS5.9.100, Lavc58.134.100, Lavf58.76.100

	PRETTY_NAME="Ubuntu 23.04"
	NAME="Ubuntu"
	OCTAVE_VERSION = 7.3.0
	FFmpeg_versions = Lavu57.28.100, SwS6.7.100, Lavc59.37.100, Lavf59.27.100
*/

#include <algorithm>
#include <limits>
#include <string>
#include <mutex>
#include <octave/oct.h>

#ifndef __OPENCV_BUILD
#define CV_FOURCC(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))
#endif

#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )

#if defined _MSC_VER && _MSC_VER >= 1200
#pragma warning( disable: 4244 4510 4610 )
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#ifdef _MSC_VER
#pragma warning(disable: 4996)  // was declared deprecated
#endif

#ifndef CV_UNUSED  // Required for standalone compilation mode (OpenCV defines this in base.hpp)
#define CV_UNUSED(name) (void)name
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <cassert>
#include <libavcodec/version.h>
#include <libavformat/avformat.h>

#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/display.h>
#include <libavutil/imgutils.h>

#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#ifdef HAVE_FFMPEG_LIBAVDEVICE
#include <libavdevice/avdevice.h>
#endif

// https://github.com/FFmpeg/FFmpeg/blob/b6af56c034759b81985f8ea094e41cbd5f7fecfb/doc/APIchanges#L390-L392
#if LIBAVCODEC_BUILD >= CALC_FFMPEG_VERSION(58, 87, 100)
#include <libavcodec/bsf.h>
#endif

#include <libavutil/pixdesc.h>

// https://github.com/FFmpeg/FFmpeg/blob/b6af56c034759b81985f8ea094e41cbd5f7fecfb/doc/APIchanges#L208-L210
#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(59, 0, 100)
#  define CV_FFMPEG_FMT_CONST const
#else
// for example Debian bullseye has Lavu56.51.100, SwS5.7.100, Lavc58.91.100, Lavf58.45.100
#  define CV_FFMPEG_FMT_CONST
#endif

#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(59, 16, 100)
#  define CV_FFMPEG_PTS_FIELD pts
#else
// for example Debian bullseye has Lavu56.51.100, SwS5.7.100, Lavc58.91.100, Lavf58.45.100
#  define CV_FFMPEG_PTS_FIELD pkt_pts
#endif

#ifdef __cplusplus
}
#endif

#if defined _MSC_VER && _MSC_VER >= 1200
#pragma warning( default: 4244 4510 4610 )
#endif

#ifdef NDEBUG
#define CV_WARN(message)
#else
#define CV_WARN(message) fprintf(stderr, "warning: %s (%s:%d)\n", message, __FILE__, __LINE__)
#endif

// octave-video wrappers verbosity level
// 0 = only errors
// 1 = + warnings
// 2 = + wrapper info+debug messages
// 3 = + verbose messages
// 4 = + libav debug messages

static int verbosity_level = -1;
const char lvl_prefix[][8] = {"ERROR", "WARN", "INFO", "VERBOSE", "DEBUG"};

#define MSG(l, fmt, ...) if (verbosity_level >= l)\
  { fprintf (stderr, "%s: ", lvl_prefix[l]);\
    fprintf (stderr, fmt, __VA_ARGS__);\
    fprintf (stderr, " (%s:%d)\n", __FILE__, __LINE__);}

#define MSG_ERR(fmt, ...)     MSG(0, fmt, __VA_ARGS__)
#define MSG_WARN(fmt, ...)    MSG(1, fmt, __VA_ARGS__)
#define MSG_INFO(fmt, ...)    MSG(2, fmt, __VA_ARGS__)
#define MSG_VERBOSE(fmt, ...) MSG(3, fmt, __VA_ARGS__)
#define MSG_DEBUG(fmt, ...)   MSG(4, fmt, __VA_ARGS__)

#define CV_Assert(x) assert(x);

#define CV_8U   0
#define CV_16U  2

void set_verbosity_level (int l)
{
    if (l < 0)
      l = 0;
    else if (l > 4)
      l = 4;
    verbosity_level = l;

    // see https://ffmpeg.org/doxygen/4.4/group__lavu__log__constants.html
    switch (l)
    {
        case 0:
        case 1:
            av_log_set_level (AV_LOG_WARNING); //Something somehow does not look correct. This may or may not lead to problems
            break;
        case 2:
            av_log_set_level (AV_LOG_INFO); //Standard information.
            break;
        case 3:
            av_log_set_level (AV_LOG_VERBOSE); //Detailed information.
            break;
        case 4:
            av_log_set_level (AV_LOG_DEBUG); //Stuff which is only useful for libav* developers
            break;
    }
    MSG_DEBUG ("set new verbosity_level = %i", av_log_get_level ());
}

static int global_err;

std::string get_last_err_msg ()
{
  char err_buf[AV_ERROR_MAX_STRING_SIZE + 1];
  if (global_err)
  {
    av_strerror (global_err, err_buf, AV_ERROR_MAX_STRING_SIZE);
    return err_buf;
  }
  else
    return "No error";
}

#if defined _WIN32
    #include <windows.h>
    #if defined _MSC_VER && _MSC_VER < 1900
    struct timespec
    {
        time_t tv_sec;
        long   tv_nsec;
    };
  #endif
#elif defined __linux__ || defined __APPLE__ || defined __HAIKU__
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/time.h>
#if defined __APPLE__
    #include <sys/sysctl.h>
    #include <mach/clock.h>
    #include <mach/mach_host.h>
    #include <mach/mach_init.h>
    #include <mach/mach_traps.h>
    #include <mach/mach_port.h>
#endif
#endif

#if defined(__APPLE__)
#define AV_NOPTS_VALUE_ ((int64_t)0x8000000000000000LL)
#else
#define AV_NOPTS_VALUE_ ((int64_t)AV_NOPTS_VALUE)
#endif

#ifndef AVERROR_EOF
#define AVERROR_EOF (-MKTAG( 'E','O','F',' '))
#endif

#ifndef PKT_FLAG_KEY
#define PKT_FLAG_KEY AV_PKT_FLAG_KEY
#endif

#define LIBAVFORMAT_INTERRUPT_OPEN_DEFAULT_TIMEOUT_MS 30000
#define LIBAVFORMAT_INTERRUPT_READ_DEFAULT_TIMEOUT_MS 30000

#ifdef _WIN32
// http://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows

static
inline LARGE_INTEGER get_filetime_offset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return t;
}

static
inline void get_monotonic_time(timespec *tv)
{
    LARGE_INTEGER           t;
    FILETIME				f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized)
    {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter)
        {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        }
        else
        {
            offset = get_filetime_offset();
            frequencyToMicroseconds = 10.;
        }
    }

    if (usePerformanceCounter)
    {
        QueryPerformanceCounter(&t);
    } else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = (LONGLONG)microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_nsec = (t.QuadPart % 1000000) * 1000;
}
#else
static
inline void get_monotonic_time(timespec *time)
{
#if defined(__APPLE__) && defined(__MACH__)
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    time->tv_sec = mts.tv_sec;
    time->tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, time);
#endif
}
#endif

static
inline timespec get_monotonic_time_diff(timespec start, timespec end)
{
    timespec temp;
    if (end.tv_nsec - start.tv_nsec < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

static
inline double get_monotonic_time_diff_ms(timespec time1, timespec time2)
{
    timespec delta = get_monotonic_time_diff(time1, time2);
    double milliseconds = delta.tv_sec * 1000 + (double)delta.tv_nsec / 1000000.0;

    return milliseconds;
}

struct Image_FFMPEG
{
    unsigned char* data;
    int step;
    int width;
    int height;
};


struct AVInterruptCallbackMetadata
{
    timespec value;
    unsigned int timeout_after_ms;
    int timeout;
};

static
inline int _opencv_ffmpeg_interrupt_callback(void *ptr)
{
    AVInterruptCallbackMetadata* metadata = (AVInterruptCallbackMetadata*)ptr;
    CV_Assert(metadata);

    if (metadata->timeout_after_ms == 0)
    {
        return 0; // timeout is disabled
    }

    timespec now;
    get_monotonic_time(&now);

    metadata->timeout = get_monotonic_time_diff_ms(metadata->value, now) > metadata->timeout_after_ms;

    return metadata->timeout ? -1 : 0;
}

static
inline void _opencv_ffmpeg_av_packet_unref(AVPacket *pkt)
{
    av_packet_unref(pkt);
};

static
inline void _opencv_ffmpeg_av_image_fill_arrays(void *frame, uint8_t *ptr, enum AVPixelFormat pix_fmt, int width, int height)
{
    av_image_fill_arrays(((AVFrame*)frame)->data, ((AVFrame*)frame)->linesize, ptr, pix_fmt, width, height, 1);
};

static
inline int _opencv_ffmpeg_av_image_get_buffer_size(enum AVPixelFormat pix_fmt, int width, int height)
{
    return av_image_get_buffer_size(pix_fmt, width, height, 1);
};

static AVRational _opencv_ffmpeg_get_sample_aspect_ratio(AVStream *stream)
{
    return av_guess_sample_aspect_ratio(NULL, stream, NULL);
}

class CvCapture_FFMPEG: public octave_base_value
{
  public:
    CvCapture_FFMPEG ();

    bool open(const char* filename);
    void close();

    //double getProperty(int) const;
    bool setProperty(int, double);
    bool grabFrame();
    bool retrieveFrame(int flag, unsigned char** data, int* step, int* width, int* height, int* cn, int* depth);

    void init();

    void    seek(int64_t frame_number);
    void    seek(double sec);
    bool    slowSeek( int framenumber );

    int64_t get_total_frames() const;
    double  get_duration_sec() const;
    double  get_fps() const;
    int64_t get_bitrate() const;

    AVRational get_sample_aspect_ratio () const
    {
         if (ok)
            return _opencv_ffmpeg_get_sample_aspect_ratio(ic->streams[video_stream]);
        else
        {
            AVRational tmp;
            tmp.num = 0;
            tmp.den = 1;
            return tmp;
        }
    }

    const char* get_video_codec_name () const
      {
        return (ok)? avcodec_get_name(video_st->codecpar->codec_id) : NULL;
      }

    double  r2d(AVRational r) const;
    int64_t dts_to_frame_number(int64_t dts);
    double  dts_to_sec(int64_t dts) const;
    void    get_rotation_angle();

    AVFormatContext * ic;
    AVCodec         * avcodec;
    AVCodecContext  * context;
    int               video_stream;
    AVStream        * video_st;
    AVFrame         * picture;
    AVFrame           rgb_picture;
    int64_t           picture_pts;

    AVPacket          packet;
    Image_FFMPEG      frame;
    struct SwsContext *img_convert_ctx;

    int64_t frame_number, first_frame_number;

    bool   rotation_auto;
    int    rotation_angle; // valid 0, 90, 180, 270
    double eps_zero;
/*
   'filename' contains the filename of the videosource,
   'filename==NULL' indicates that ffmpeg's seek support works
   for the particular file.
   'filename!=NULL' indicates that the slow fallback function is used for seeking,
   and so the filename is needed to reopen the file on backward seeking.
*/
    char              * filename;

    AVDictionary *dict;
    int open_timeout;
    int read_timeout;
    AVInterruptCallbackMetadata interrupt_metadata;

    bool setRaw();
    bool processRawPacket();
    bool rawMode;
    bool rawModeInitialized;
    bool convertRGB;
    AVPacket packet_filtered;
    AVBSFContext* bsfc;
    int use_opencl;
    int extraDataIdx;
    bool ok;
    bool is_ok () { return ok; }

  bool is_constant (void) const
  {
	return true;
  }
  bool is_defined (void) const
  {
	return true;
  }

  DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA

  void print (std::ostream & os, bool pr_as_read_syntax = false)
  {
    (void) pr_as_read_syntax;
	os << "CvCapture_FFMPEG:" << std::endl;
	if (filename)
	  os << "  filename           = " << filename << std::endl;
	os << "  get_total_frames() = " << get_total_frames() << std::endl;
	os << "  get_duration_sec() = " << get_duration_sec() << std::endl;
	os << "  get_fps()          = " << get_fps() << std::endl;
	os << "  get_bitrate()      = " << get_bitrate() << std::endl;
	os << "  width              = " << frame.width << std::endl;
	os << "  height             = " << frame.height << std::endl;
	os << "  frame_number       = " << frame_number << std::endl;
	os << "  video_codec_name   = " << get_video_codec_name () << std::endl;
	AVRational s = get_sample_aspect_ratio ();
	os << "  aspect_ration_num  = " << s.num << std::endl;
	os << "  aspect_ration_den  = " << s.den << std::endl;
  }

};

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA(CvCapture_FFMPEG, "CvCapture_FFMPEG", "CvCapture_FFMPEG");

CvCapture_FFMPEG::CvCapture_FFMPEG ()
  : octave_base_value () { init (); };

void CvCapture_FFMPEG::init()
{
    if (verbosity_level < 0)
      set_verbosity_level (1);

#ifdef HAVE_FFMPEG_LIBAVDEVICE
    //libavdevice is available, so let's register all input and output devices (e.g v4l2)
    avdevice_register_all();
#endif
    ic = 0;
    video_stream = -1;
    video_st = 0;
    picture = 0;
    picture_pts = AV_NOPTS_VALUE_;
    first_frame_number = -1;
    memset( &rgb_picture, 0, sizeof(rgb_picture) );
    memset( &frame, 0, sizeof(frame) );
    filename = 0;
    memset(&packet, 0, sizeof(packet));
    av_init_packet(&packet);
    img_convert_ctx = 0;

    avcodec = 0;
    context = 0;
    frame_number = 0;
    eps_zero = 0.000025;

    rotation_angle = 0;
    rotation_auto = true;

    dict = NULL;

    open_timeout = LIBAVFORMAT_INTERRUPT_OPEN_DEFAULT_TIMEOUT_MS;
    read_timeout = LIBAVFORMAT_INTERRUPT_READ_DEFAULT_TIMEOUT_MS;

    rawMode = false;
    rawModeInitialized = false;
    convertRGB = true;
    memset(&packet_filtered, 0, sizeof(packet_filtered));
    av_init_packet(&packet_filtered);
    bsfc = NULL;
    use_opencl = 0;
    extraDataIdx = 1;
    ok = false;
}


void CvCapture_FFMPEG::close()
{
    if( img_convert_ctx )
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = 0;
    }

    if( picture )
    {
        av_frame_free(&picture);
    }

    if( video_st )
    {
        avcodec_close( context );
        video_st = NULL;
    }

    if (context)
    {
        avcodec_free_context(&context);
    }

    if( ic )
    {
        avformat_close_input(&ic);
        ic = NULL;
    }

    av_frame_unref(&rgb_picture);

    // free last packet if exist
    if (packet.data) {
        _opencv_ffmpeg_av_packet_unref (&packet);
        packet.data = NULL;
    }

    if (dict != NULL)
       av_dict_free(&dict);

    if (packet_filtered.data)
    {
        _opencv_ffmpeg_av_packet_unref(&packet_filtered);
        packet_filtered.data = NULL;
    }

    if (bsfc)
    {
        av_bsf_free(&bsfc);
    }

    init();
}


#ifndef AVSEEK_FLAG_FRAME
#define AVSEEK_FLAG_FRAME 0
#endif
#ifndef AVSEEK_FLAG_ANY
#define AVSEEK_FLAG_ANY 1
#endif

#if defined(__OPENCV_BUILD) || defined(BUILD_PLUGIN)
typedef cv::Mutex ImplMutex;
#else
class ImplMutex
{
public:
    ImplMutex() { init(); }
    ~ImplMutex() { destroy(); }

    void init();
    void destroy();

    void lock();
    bool trylock();
    void unlock();

    struct Impl;
protected:
    Impl* impl;

private:
    ImplMutex(const ImplMutex&);
    ImplMutex& operator = (const ImplMutex& m);
};

#if defined _WIN32 || defined WINCE

struct ImplMutex::Impl
{
    void init()
    {
#if (_WIN32_WINNT >= 0x0600)
        ::InitializeCriticalSectionEx(&cs, 1000, 0);
#else
        ::InitializeCriticalSection(&cs);
#endif
        refcount = 1;
    }
    void destroy() { DeleteCriticalSection(&cs); }

    void lock() { EnterCriticalSection(&cs); }
    bool trylock() { return TryEnterCriticalSection(&cs) != 0; }
    void unlock() { LeaveCriticalSection(&cs); }

    CRITICAL_SECTION cs;
    int refcount;
};


#elif defined __APPLE__

#include <libkern/OSAtomic.h>

struct ImplMutex::Impl
{
    void init() { sl = OS_SPINLOCK_INIT; refcount = 1; }
    void destroy() { }

    void lock() { OSSpinLockLock(&sl); }
    bool trylock() { return OSSpinLockTry(&sl); }
    void unlock() { OSSpinLockUnlock(&sl); }

    OSSpinLock sl;
    int refcount;
};

#elif defined __linux__ && !defined __ANDROID__

struct ImplMutex::Impl
{
    void init() { pthread_spin_init(&sl, 0); refcount = 1; }
    void destroy() { pthread_spin_destroy(&sl); }

    void lock() { pthread_spin_lock(&sl); }
    bool trylock() { return pthread_spin_trylock(&sl) == 0; }
    void unlock() { pthread_spin_unlock(&sl); }

    pthread_spinlock_t sl;
    int refcount;
};

#else

struct ImplMutex::Impl
{
    void init() { pthread_mutex_init(&sl, 0); refcount = 1; }
    void destroy() { pthread_mutex_destroy(&sl); }

    void lock() { pthread_mutex_lock(&sl); }
    bool trylock() { return pthread_mutex_trylock(&sl) == 0; }
    void unlock() { pthread_mutex_unlock(&sl); }

    pthread_mutex_t sl;
    int refcount;
};

#endif

void ImplMutex::init()
{
    impl = new Impl();
    impl->init();
}
void ImplMutex::destroy()
{
    impl->destroy();
    delete(impl);
    impl = NULL;
}
void ImplMutex::lock() { impl->lock(); }
void ImplMutex::unlock() { impl->unlock(); }
bool ImplMutex::trylock() { return impl->trylock(); }

class AutoLock
{
public:
    explicit AutoLock(ImplMutex& m) : mutex(&m) { mutex->lock(); }
    ~AutoLock() { mutex->unlock(); }
protected:
    ImplMutex* mutex;
private:
    AutoLock(const AutoLock&); // disabled
    AutoLock& operator = (const AutoLock&); // disabled
};
#endif


static ImplMutex _mutex;

class InternalFFMpegRegister
{
public:
    static void init()
    {
		AutoLock lock(_mutex);
        static InternalFFMpegRegister instance;
    }
    InternalFFMpegRegister()
    {
        avformat_network_init();

    }
    ~InternalFFMpegRegister()
    {
        av_log_set_callback(NULL);
    }
};

static int get_number_of_cpus(void)
{
#if defined _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );

    return (int)sysinfo.dwNumberOfProcessors;
#elif defined __linux__ || defined __HAIKU__
    return (int)sysconf( _SC_NPROCESSORS_ONLN );
#elif defined __APPLE__
    int numCPU=0;
    int mib[4];
    size_t len = sizeof(numCPU);

    // set the mib for hw.ncpu
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

    // get the number of CPUs from the system
    sysctl(mib, 2, &numCPU, &len, NULL, 0);

    if( numCPU < 1 )
    {
        mib[1] = HW_NCPU;
        sysctl( mib, 2, &numCPU, &len, NULL, 0 );

        if( numCPU < 1 )
            numCPU = 1;
    }

    return (int)numCPU;
#else
    return 1;
#endif
}

inline void fill_codec_context(AVCodecContext * enc, AVDictionary * dict)
{
    if (!enc->thread_count)
    {
        int nCpus = get_number_of_cpus();
        int requestedThreads = std::min(nCpus, 16);  // [OPENCV:FFMPEG:24] Application has requested XX threads. Using a thread count greater than 16 is not recommended.
        enc->thread_count = requestedThreads;
    }

    AVDictionaryEntry* avdiscard_entry = av_dict_get(dict, "avdiscard", NULL, 0);

    if (avdiscard_entry)
    {
        if(strcmp(avdiscard_entry->value, "all") == 0)
            enc->skip_frame = AVDISCARD_ALL;
        else if (strcmp(avdiscard_entry->value, "bidir") == 0)
            enc->skip_frame = AVDISCARD_BIDIR;
        else if (strcmp(avdiscard_entry->value, "default") == 0)
            enc->skip_frame = AVDISCARD_DEFAULT;
        else if (strcmp(avdiscard_entry->value, "none") == 0)
            enc->skip_frame = AVDISCARD_NONE;
        else if (strcmp(avdiscard_entry->value, "nonintra") == 0)
            enc->skip_frame = AVDISCARD_NONINTRA;
        else if (strcmp(avdiscard_entry->value, "nonkey") == 0)
            enc->skip_frame = AVDISCARD_NONKEY;
        else if (strcmp(avdiscard_entry->value, "nonref") == 0)
            enc->skip_frame = AVDISCARD_NONREF;
    }
}

bool CvCapture_FFMPEG::open(const char* _filename)
{
    InternalFFMpegRegister::init();

	AutoLock lock(_mutex);

    unsigned i;
    int nThreads = 0;

    close();

    convertRGB = true;

    interrupt_metadata.timeout_after_ms = LIBAVFORMAT_INTERRUPT_OPEN_DEFAULT_TIMEOUT_MS;
    get_monotonic_time(&interrupt_metadata.value);

    ic = avformat_alloc_context();
    ic->interrupt_callback.callback = _opencv_ffmpeg_interrupt_callback;
    ic->interrupt_callback.opaque = &interrupt_metadata;

    av_dict_set(&dict, "rtsp_transport", "tcp", 0);
    CV_FFMPEG_FMT_CONST AVInputFormat* input_format = NULL;
    AVDictionaryEntry* entry = av_dict_get(dict, "input_format", NULL, 0);
    if (entry != 0)
    {
      input_format = av_find_input_format(entry->value);
    }

    global_err = avformat_open_input(&ic, _filename, input_format, &dict);

    if (global_err < 0)
    {
        CV_WARN("Error opening file");
        CV_WARN(_filename);
        goto exit_func;
    }
    global_err = avformat_find_stream_info(ic, NULL);
    if (global_err < 0)
    {
        MSG_WARN("Unable to read codec parameters from stream (%s)", get_last_err_msg().c_str());
        goto exit_func;
    }
    for(i = 0; i < ic->nb_streams; i++)
    {
        AVCodecParameters* par = ic->streams[i]->codecpar;
        AVCodecID codec_id = par->codec_id;
        AVMediaType codec_type = par->codec_type;

        if( AVMEDIA_TYPE_VIDEO == codec_type && video_stream < 0)
        {
            // backup encoder' width/height
            int enc_width = par->width;
            int enc_height = par->height;

            MSG_INFO("FFMPEG: stream[%i] is video stream with codecID=%i, width=%i, height=%i", i, (int)codec_id, enc_width, enc_height);

            // find and open decoder, try HW acceleration types specified in 'hw_acceleration' list (in order)
            const AVCodec *codec = NULL;
            global_err = -1;
            do {
                {
                    AVDictionaryEntry* video_codec_param = av_dict_get(dict, "video_codec", NULL, 0);
                    if (video_codec_param == NULL)
                    {
                        codec = avcodec_find_decoder(codec_id);
                        if (!codec)
                        {
                            MSG_ERR("Could not find decoder for codec '%s'(id = %i)", avcodec_get_name (codec_id), (int)codec_id);
                        }
                    }
                    else
                    {
                        MSG_INFO("FFMPEG: Using video_codec='%s'", video_codec_param->value);
                        codec = avcodec_find_decoder_by_name(video_codec_param->value);
                        if (!codec)
                        {
                            MSG_ERR("Could not find decoder '%s'", video_codec_param->value);
                        }
                    }
                    if (codec)
                    {
                        context = avcodec_alloc_context3(codec);
                        CV_Assert(context);
                    }
                }
                if (!codec)
                {
                    avcodec_free_context(&context);
                    continue;
                }
                context->thread_count = nThreads;
                fill_codec_context(context, dict);
                avcodec_parameters_to_context(context, par);
                global_err = avcodec_open2(context, codec, NULL);
                if (global_err >= 0) {
                    break;
                } else {
                    MSG_ERR("Could not open codec '%s', error: '%s'", codec->name, get_last_err_msg ().c_str());
                }
            } while (0);
            if (global_err < 0) {
                MSG_ERR("VIDEOIO/FFMPEG: Failed to initialize VideoCapture, error: '%s'", get_last_err_msg().c_str());
                goto exit_func;
            }

            // checking width/height (since decoder can sometimes alter it, eg. vp6f)
            if (enc_width && (context->width != enc_width))
                context->width = enc_width;
            if (enc_height && (context->height != enc_height))
                context->height = enc_height;

            video_stream = i;
            video_st = ic->streams[i];
            picture = av_frame_alloc();
            frame.width = context->width;
            frame.height = context->height;
            frame.step = 0;
            frame.data = NULL;
            get_rotation_angle();
            break;
        }
    }

    if (video_stream >= 0)
        ok = true;

exit_func:

    // deactivate interrupt callback
    interrupt_metadata.timeout_after_ms = 0;

    if( !ok )
      close();

    return ok;
}


bool CvCapture_FFMPEG::setRaw()
{
    if (!rawMode)
    {
        if (frame_number != 0)
        {
            CV_WARN("Incorrect usage: do not grab frames before .set(CAP_PROP_FORMAT, -1)");
        }
        // binary stream filter creation is moved into processRawPacket()
        rawMode = true;
    }
    return true;
}

static inline bool h26xContainer(const char* formatLongName) {
    return !strcmp(formatLongName, "QuickTime / MOV") || !strcmp(formatLongName, "FLV (Flash Video)") || !strcmp(formatLongName, "Matroska / WebM");
}

bool CvCapture_FFMPEG::processRawPacket()
{
    if (packet.data == NULL)  // EOF
        return false;
    if (!rawModeInitialized)
    {
        rawModeInitialized = true;
        AVCodecID eVideoCodec = ic->streams[video_stream]->codecpar->codec_id;
        const char* filterName = NULL;
        if (   eVideoCodec == AV_CODEC_ID_H264
            || eVideoCodec == AV_CODEC_ID_H265)
        {
            if(h26xContainer(ic->iformat->long_name))
                filterName = eVideoCodec == AV_CODEC_ID_H264 ? "h264_mp4toannexb" : "hevc_mp4toannexb";
        }
        if (filterName)
        {
            const AVBitStreamFilter * bsf = av_bsf_get_by_name(filterName);
            if (!bsf)
            {
                //fixme CV_WARN("Bitstream filter is not available: %s", filterName);
                return false;
            }
            int err = av_bsf_alloc(bsf, &bsfc);
            if (err < 0)
            {
                CV_WARN("Error allocating context for bitstream buffer");
                return false;
            }
            avcodec_parameters_copy(bsfc->par_in, ic->streams[video_stream]->codecpar);
            err = av_bsf_init(bsfc);
            if (err < 0)
            {
                CV_WARN("Error initializing bitstream buffer");
                return false;
            }
        }
    }
    if (bsfc)
    {
        if (packet_filtered.data)
        {
            _opencv_ffmpeg_av_packet_unref(&packet_filtered);
        }

        int err = av_bsf_send_packet(bsfc, &packet);
        if (err < 0)
        {
            CV_WARN("Packet submission for filtering failed");
            return false;
        }
        err = av_bsf_receive_packet(bsfc, &packet_filtered);
        if (err < 0)
        {
            CV_WARN("Filtered packet retrieve failed");
            return false;
        }
        return packet_filtered.data != NULL;
    }
    return packet.data != NULL;
}

bool CvCapture_FFMPEG::grabFrame()
{
    bool valid = false;

    static const size_t max_read_attempts = 4096; // cv::utils::getConfigurationParameterSizeT("OPENCV_FFMPEG_READ_ATTEMPTS", 4096);
    static const size_t max_decode_attempts = 64; //cv::utils::getConfigurationParameterSizeT("OPENCV_FFMPEG_DECODE_ATTEMPTS", 64);
    size_t cur_read_attempts = 0;
    size_t cur_decode_attempts = 0;

    if( !ic || !video_st || !context )  return false;

    if( ic->streams[video_stream]->nb_frames > 0 &&
        frame_number > ic->streams[video_stream]->nb_frames )
        return false;

    picture_pts = AV_NOPTS_VALUE_;

    // activate interrupt callback
    interrupt_metadata.timeout = 0;
    get_monotonic_time(&interrupt_metadata.value);
    interrupt_metadata.timeout_after_ms = read_timeout;

    // check if we can receive frame from previously decoded packet
    valid = avcodec_receive_frame(context, picture) >= 0;

    // get the next frame
    while (!valid)
    {

        _opencv_ffmpeg_av_packet_unref (&packet);

        if (interrupt_metadata.timeout)
        {
            valid = false;
            break;
        }

        int ret = av_read_frame(ic, &packet);

        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret == AVERROR_EOF)
        {
            if (rawMode)
                break;

            // flush cached frames from video decoder
            packet.data = NULL;
            packet.size = 0;
            packet.stream_index = video_stream;
        }

        if( packet.stream_index != video_stream )
        {
            _opencv_ffmpeg_av_packet_unref (&packet);
            if (++cur_read_attempts > max_read_attempts)
            {
                MSG_WARN("packet read max attempts (%li) exceeded", max_read_attempts);
                break;
            }
            continue;
        }

        if (rawMode)
        {
            valid = processRawPacket();
            break;
        }

        // Decode video frame
        if (avcodec_send_packet(context, &packet) < 0) {
            break;
        }
        ret = avcodec_receive_frame(context, picture);

        if (ret >= 0) {
            valid = true;
        } else if (ret == AVERROR(EAGAIN)) {
            continue;
        }
        else
        {
            if (++cur_decode_attempts > max_decode_attempts)
            {
                fprintf(stderr,
                    "frame decode max attempts exceeded, try to increase attempt "
                    "limit by setting environment variable OPENCV_FFMPEG_DECODE_ATTEMPTS "
                    "(current value is %zu)\n", max_decode_attempts);
                break;
            }
        }
    }

    if (valid) {
        if( picture_pts == AV_NOPTS_VALUE_ )
            picture_pts = picture->CV_FFMPEG_PTS_FIELD != AV_NOPTS_VALUE_ && picture->CV_FFMPEG_PTS_FIELD != 0 ? picture->CV_FFMPEG_PTS_FIELD : picture->pkt_dts;
        frame_number++;
    }

    if (!rawMode && valid && first_frame_number < 0)
        first_frame_number = dts_to_frame_number(picture_pts);

    // deactivate interrupt callback
    interrupt_metadata.timeout_after_ms = 0;

    // return if we have a new frame or not
    return valid;
}

bool CvCapture_FFMPEG::retrieveFrame(int flag, unsigned char** data, int* step, int* width, int* height, int* cn, int* depth)
{
    if (!video_st || !context)
        return false;

    if (rawMode || flag == extraDataIdx)
    {
        bool ret = true;
        if (flag == 0) {
            AVPacket& p = bsfc ? packet_filtered : packet;
            *data = p.data;
            *step = p.size;
            ret = p.data != NULL;
        }
        else if (flag == extraDataIdx) {
            *data = ic->streams[video_stream]->codecpar->extradata;
            *step = ic->streams[video_stream]->codecpar->extradata_size;
        }
        *width = *step;
        *height = 1;
        *cn = 1;
        *depth = CV_8U;
        return  ret;
    }

    AVFrame* sw_picture = picture;

    if (!sw_picture || !sw_picture->data[0])
        return false;

    MSG_VERBOSE("Input picture format: '%s'", av_get_pix_fmt_name((AVPixelFormat)sw_picture->format));
    const AVPixelFormat result_format = convertRGB ? AV_PIX_FMT_BGR24 : (AVPixelFormat)sw_picture->format;
    switch (result_format)
    {
    case AV_PIX_FMT_BGR24: *depth = CV_8U; *cn = 3; break;
    case AV_PIX_FMT_GRAY8: *depth = CV_8U; *cn = 1; break;
    case AV_PIX_FMT_GRAY16LE: *depth = CV_16U; *cn = 1; break;
    default:
        MSG_WARN("Unknown/unsupported picture format: %s, will be treated as 8UC1.", av_get_pix_fmt_name(result_format));
        *depth = CV_8U;
        *cn = 1;
        break; // TODO: return false?
    }

    if( img_convert_ctx == NULL ||
        frame.width != video_st->codecpar->width ||
        frame.height != video_st->codecpar->height ||
        frame.data == NULL )
    {
        // Some sws_scale optimizations have some assumptions about alignment of data/step/width/height
        // Also we use coded_width/height to workaround problem with legacy ffmpeg versions (like n0.8)
        int buffer_width = context->coded_width, buffer_height = context->coded_height;

        img_convert_ctx = sws_getCachedContext(
                img_convert_ctx,
                buffer_width, buffer_height,
                (AVPixelFormat)sw_picture->format,
                buffer_width, buffer_height,
                result_format,
                SWS_BICUBIC,
                NULL, NULL, NULL
                );

        if (img_convert_ctx == NULL)
            return false;//CV_Error(0, "Cannot initialize the conversion context!");

        av_frame_unref(&rgb_picture);
        rgb_picture.format = result_format;
        rgb_picture.width = buffer_width;
        rgb_picture.height = buffer_height;
        if (0 != av_frame_get_buffer(&rgb_picture, 32))
        {
            CV_WARN("OutOfMemory");
            return false;
        }
        frame.width = video_st->codecpar->width;
        frame.height = video_st->codecpar->height;
        frame.data = rgb_picture.data[0];
        frame.step = rgb_picture.linesize[0];
    }

    sws_scale(
            img_convert_ctx,
            sw_picture->data,
            sw_picture->linesize,
            0, sw_picture->height,
            rgb_picture.data,
            rgb_picture.linesize
            );

    *data = frame.data;
    *step = frame.step;
    *width = frame.width;
    *height = frame.height;

    return true;
}
/*
static inline double getCodecTag(const AVCodecID codec_id) {
    const struct AVCodecTag* fallback_tags[] = {
        avformat_get_riff_video_tags(),
        avformat_get_mov_video_tags(),
        codec_bmp_tags, // fallback for avformat < 54.1
        NULL };
    return av_codec_get_tag(fallback_tags, codec_id);
}
*/
/*
static inline double getCodecIdFourcc(const AVCodecID codec_id)
{
    if (codec_id == AV_CODEC_ID_NONE) return -1;
    const char* codec_fourcc = _opencv_avcodec_get_name(codec_id);
    if (!codec_fourcc || strcmp(codec_fourcc, "unknown_codec") == 0 || strlen(codec_fourcc) != 4)
        return getCodecTag(codec_id);
    return (double)CV_FOURCC(codec_fourcc[0], codec_fourcc[1], codec_fourcc[2], codec_fourcc[3]);
}
*/

#if 0
double CvCapture_FFMPEG::getProperty( int property_id ) const
{
    if( !video_st || !context ) return 0;

    switch( property_id )
    {
    case CAP_PROP_POS_MSEC:
        if (picture_pts == AV_NOPTS_VALUE_)
        {
            return 0;
        }
        return (dts_to_sec(picture_pts) * 1000);
    case CAP_PROP_POS_FRAMES:
        return (double)frame_number;
    case CAP_PROP_POS_AVI_RATIO:
        return r2d(ic->streams[video_stream]->time_base);
    case CAP_PROP_FRAME_COUNT:
        return (double)get_total_frames();
    case CAP_PROP_FRAME_WIDTH:
        return (double)((rotation_auto && ((rotation_angle%180) != 0)) ? frame.height : frame.width);
    case CAP_PROP_FRAME_HEIGHT:
        return (double)((rotation_auto && ((rotation_angle%180) != 0)) ? frame.width : frame.height);
    case CAP_PROP_FRAME_TYPE:
        return (double)av_get_picture_type_char(picture->pict_type);
    case CAP_PROP_FPS:
        return get_fps();
    case CAP_PROP_FOURCC: {
        const double fourcc = getCodecIdFourcc(video_st->codecpar->codec_id);
        if (fourcc != -1) return fourcc;
        const double codec_tag = (double)video_st->codecpar->codec_tag;
        if (codec_tag) return codec_tag;
        else return -1;
    }
    case CAP_PROP_SAR_NUM:
        return _opencv_ffmpeg_get_sample_aspect_ratio(ic->streams[video_stream]).num;
    case CAP_PROP_SAR_DEN:
        return _opencv_ffmpeg_get_sample_aspect_ratio(ic->streams[video_stream]).den;
    case CAP_PROP_CODEC_PIXEL_FORMAT:
    {
        AVPixelFormat pix_fmt = (AVPixelFormat)video_st->codecpar->format;
        unsigned int fourcc_tag = avcodec_pix_fmt_to_codec_tag(pix_fmt);
        return (fourcc_tag == 0) ? (double)-1 : (double)fourcc_tag;
    }
    case CAP_PROP_FORMAT:
        if (rawMode)
            return -1;
        break;
    case CAP_PROP_CONVERT_RGB:
        return convertRGB;
    case CAP_PROP_LRF_HAS_KEY_FRAME: {
        const AVPacket& p = bsfc ? packet_filtered : packet;
        return ((p.flags & AV_PKT_FLAG_KEY) != 0) ? 1 : 0;
    }
    case CAP_PROP_CODEC_EXTRADATA_INDEX:
            return extraDataIdx;
    case CAP_PROP_BITRATE:
        return static_cast<double>(get_bitrate());
    case CAP_PROP_ORIENTATION_META:
        return static_cast<double>(rotation_angle);
    case CAP_PROP_ORIENTATION_AUTO:
        return static_cast<double>(rotation_auto);
    case CAP_PROP_STREAM_OPEN_TIME_USEC:
        //ic->start_time_realtime is in microseconds
        return ((double)ic->start_time_realtime);
    case CAP_PROP_N_THREADS:
        return static_cast<double>(context->thread_count);
    default:
        break;
    }

    return 0;
}
#endif

double CvCapture_FFMPEG::r2d(AVRational r) const
{
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

double CvCapture_FFMPEG::get_duration_sec() const
{
    if (! ok)
        return -1;

    double sec = (double)ic->duration / (double)AV_TIME_BASE;
    //printf ("sec = %f\n", sec);

    if (sec < eps_zero)
    {
        sec = (double)ic->streams[video_stream]->duration * r2d(ic->streams[video_stream]->time_base);
    }

    return sec;
}

int64_t CvCapture_FFMPEG::get_bitrate() const
{
    if (! ok)
        return -1;

    return ic->bit_rate / 1000;
}

double CvCapture_FFMPEG::get_fps() const
{
    if (! ok)
        return -1;

    double fps = r2d(av_guess_frame_rate(ic, ic->streams[video_stream], NULL));
    return fps;
}

int64_t CvCapture_FFMPEG::get_total_frames() const
{
    if (! ok)
        return -1;

    int64_t nbf = ic->streams[video_stream]->nb_frames;

    if (nbf == 0)
    {
        double t = get_duration_sec ();
        double fps = get_fps();
        MSG_WARN ("'nb_frames' returned %li -> calculate total_frames from duration = %.2fs and fps = %.2f/s", nbf, t, fps);
        nbf = (int64_t)floor(t * fps + 0.5);
    }
    return nbf;
}

int64_t CvCapture_FFMPEG::dts_to_frame_number(int64_t dts)
{
    double sec = dts_to_sec(dts);
    return (int64_t)(get_fps() * sec + 0.5);
}

double CvCapture_FFMPEG::dts_to_sec(int64_t dts) const
{
    return (double)(dts - ic->streams[video_stream]->start_time) *
        r2d(ic->streams[video_stream]->time_base);
}

void CvCapture_FFMPEG::get_rotation_angle()
{
    rotation_angle = 0;
    const uint8_t *data = 0;
    data = av_stream_get_side_data(video_st, AV_PKT_DATA_DISPLAYMATRIX, NULL);
    if (data)
    {
        rotation_angle = -round(av_display_rotation_get((const int32_t*)data));
        if (rotation_angle < 0)
            rotation_angle += 360;
    }
}

void CvCapture_FFMPEG::seek(int64_t _frame_number)
{
    CV_Assert(context);
    _frame_number = std::min(_frame_number, get_total_frames());
    int delta = 16;

    // if we have not grabbed a single frame before first seek, let's read the first frame
    // and get some valuable information during the process
    if( first_frame_number < 0 && get_total_frames() > 1 )
        grabFrame();

    for(;;)
    {
        int64_t _frame_number_temp = std::max(_frame_number-delta, (int64_t)0);
        double sec = (double)_frame_number_temp / get_fps();
        int64_t time_stamp = ic->streams[video_stream]->start_time;
        double  time_base  = r2d(ic->streams[video_stream]->time_base);
        time_stamp += (int64_t)(sec / time_base + 0.5);
        if (get_total_frames() > 1) av_seek_frame(ic, video_stream, time_stamp, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(context);
        if( _frame_number > 0 )
        {
            grabFrame();

            if( _frame_number > 1 )
            {
                frame_number = dts_to_frame_number(picture_pts) - first_frame_number;
                //printf("_frame_number = %d, frame_number = %d, delta = %d\n",
                //       (int)_frame_number, (int)frame_number, delta);

                if( frame_number < 0 || frame_number > _frame_number-1 )
                {
                    if( _frame_number_temp == 0 || delta >= INT_MAX/4 )
                        break;
                    delta = delta < 16 ? delta*2 : delta*3/2;
                    continue;
                }
                while( frame_number < _frame_number-1 )
                {
                    if(!grabFrame())
                        break;
                }
                frame_number++;
                break;
            }
            else
            {
                frame_number = 1;
                break;
            }
        }
        else
        {
            frame_number = 0;
            break;
        }
    }
}

void CvCapture_FFMPEG::seek(double sec)
{
    seek((int64_t)(sec * get_fps() + 0.5));
}

/*
bool CvCapture_FFMPEG::setProperty( int property_id, double value )
{
    if( !video_st ) return false;
#if 0
    switch( property_id )
    {
    case CAP_PROP_POS_MSEC:
    case CAP_PROP_POS_FRAMES:
    case CAP_PROP_POS_AVI_RATIO:
        {
            switch( property_id )
            {
            case CAP_PROP_POS_FRAMES:
                seek((int64_t)value);
                break;

            case CAP_PROP_POS_MSEC:
                seek(value/1000.0);
                break;

            case CAP_PROP_POS_AVI_RATIO:
                seek((int64_t)(value*ic->duration));
                break;
            }

            picture_pts=(int64_t)value;
        }
        break;
    case CAP_PROP_FORMAT:
        if (value == -1)
            return setRaw();
        return false;
    case CAP_PROP_CONVERT_RGB:
        convertRGB = (value != 0);
        return true;
    case CAP_PROP_ORIENTATION_AUTO:
        rotation_auto = value != 0 ? true : false;
        return true;
    default:
        return false;
    }
#endif
    return true;
}
*/
///////////////// FFMPEG CvVideoWriter implementation //////////////////////////

class CvVideoWriter_FFMPEG: public octave_base_value
{
  public:
    CvVideoWriter_FFMPEG ();

    bool open( const char* filename, int fourcc,
               double fps, int width, int height, bool isColor );
    void close();
    bool writeFrame( const unsigned char* data, int step, int width, int height, int cn, int origin );
    double getProperty(int propId) const;

    void init();

    CV_FFMPEG_FMT_CONST AVOutputFormat  * fmt;
    AVFormatContext * oc;
    uint8_t         * outbuf;
    uint32_t          outbuf_size;
    FILE            * outfile;
    AVFrame         * picture;
    AVFrame         * input_picture;
    uint8_t         * picbuf;
    AVStream        * video_st;
    AVCodecContext  * context;
    AVPixelFormat     input_pix_fmt;
    unsigned char   * aligned_input;
    size_t            aligned_input_size;
    int               frame_width, frame_height;
    int               frame_idx;
    struct SwsContext *img_convert_ctx;
    int               hw_device;
    int               use_opencl;

    bool              ok;
    bool is_ok () { return ok; }

    const char* get_video_codec_name () const
    {
      return avcodec_get_name(video_st->codecpar->codec_id);
    }

    bool is_constant (void) const
    {
      return true;
    }
    bool is_defined (void) const
    {
      return true;
    }

    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA

    // called from Octave, see example in demo_low_level_write_frame.m
    void print (std::ostream & os, bool pr_as_read_syntax = false)
    {
      (void) pr_as_read_syntax;
      os << "CvVideoWriter_FFMPEG:" << std::endl;
      os << "  ok                      = " << ok << std::endl;
      os << "  frame_width             = " << frame_width << std::endl;
      os << "  frame_height            = " << frame_height << std::endl;
      os << "  frame_idx               = " << frame_idx << std::endl;
      os << "  get_video_codec_name () = " << get_video_codec_name () << std::endl;
    }
};

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA(CvVideoWriter_FFMPEG, "CvVideoWriter_FFMPEG", "CvVideoWriter_FFMPEG");

CvVideoWriter_FFMPEG::CvVideoWriter_FFMPEG ()
  : octave_base_value () { init (); };

static const char * icvFFMPEGErrStr(int err)
{
    switch(err) {
    case AVERROR_BSF_NOT_FOUND:
        return "Bitstream filter not found";
    case AVERROR_DECODER_NOT_FOUND:
        return "Decoder not found";
    case AVERROR_DEMUXER_NOT_FOUND:
        return "Demuxer not found";
    case AVERROR_ENCODER_NOT_FOUND:
        return "Encoder not found";
    case AVERROR_EOF:
        return "End of file";
    case AVERROR_EXIT:
        return "Immediate exit was requested; the called function should not be restarted";
    case AVERROR_FILTER_NOT_FOUND:
        return "Filter not found";
    case AVERROR_INVALIDDATA:
        return "Invalid data found when processing input";
    case AVERROR_MUXER_NOT_FOUND:
        return "Muxer not found";
    case AVERROR_OPTION_NOT_FOUND:
        return "Option not found";
    case AVERROR_PATCHWELCOME:
        return "Not yet implemented in FFmpeg, patches welcome";
    case AVERROR_PROTOCOL_NOT_FOUND:
        return "Protocol not found";
    case AVERROR_STREAM_NOT_FOUND:
        return "Stream not found";
    default:
        break;
    }

    return "Unspecified error";
}

/* function internal to FFMPEG (libavformat/riff.c) to lookup codec id by fourcc tag*/
/*
extern "C" {
    enum AVCodecID codec_get_bmp_id(unsigned int tag);
}
*/

void CvVideoWriter_FFMPEG::init()
{
    if (verbosity_level < 0)
      set_verbosity_level (1);

    fmt = 0;
    oc = 0;
    outbuf = 0;
    outbuf_size = 0;
    outfile = 0;
    picture = 0;
    input_picture = 0;
    picbuf = 0;
    video_st = 0;
    context = 0;
    input_pix_fmt = AV_PIX_FMT_NONE;
    aligned_input = NULL;
    aligned_input_size = 0;
    img_convert_ctx = 0;
    frame_width = frame_height = 0;
    frame_idx = 0;
    hw_device = -1;
    use_opencl = 0;
    ok = false;
}

/**
 * the following function is a modified version of code
 * found in ffmpeg-0.4.9-pre1/output_example.c
 */
static AVFrame * icv_alloc_picture_FFMPEG(int pix_fmt, int width, int height, bool alloc)
{
    AVFrame *picture = av_frame_alloc();
    if (!picture)
      return NULL;

    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;

    int size = _opencv_ffmpeg_av_image_get_buffer_size( (AVPixelFormat) pix_fmt, width, height);
    if(alloc)
    {
        uint8_t *picture_buf = (uint8_t *) malloc(size);
        if (!picture_buf)
        {
            av_free(picture);
            return NULL;
        }
        _opencv_ffmpeg_av_image_fill_arrays(picture, picture_buf,
                       (AVPixelFormat) pix_fmt, width, height);
    }
    return picture;
}

/* configure video stream */
static AVCodecContext * icv_configure_video_stream_FFMPEG(AVFormatContext *oc,
                                                   AVStream *st,
                                                   const AVCodec* codec,
                                                   int w, int h, int bitrate,
                                                   double fps, AVPixelFormat pixel_format, int fourcc)
{
    AVCodecContext *c = avcodec_alloc_context3(codec);
    CV_Assert(c);

    int frame_rate, frame_rate_base;

    CV_Assert (codec);
    c->codec_id = codec->id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;
    c->codec_tag = fourcc;

    /* put sample parameters */
    int64_t lbit_rate = (int64_t)bitrate;
    lbit_rate += (bitrate / 2);
    lbit_rate = std::min(lbit_rate, (int64_t)INT_MAX);
    c->bit_rate = lbit_rate;

    // took advice from
    // http://ffmpeg-users.933282.n4.nabble.com/warning-clipping-1-dct-coefficients-to-127-127-td934297.html
    c->qmin = 3;

    /* resolution must be a multiple of two */
    c->width = w;
    c->height = h;

    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    frame_rate=(int)(fps+0.5);
    frame_rate_base=1;
    while (fabs(((double)frame_rate/frame_rate_base) - fps) > 0.001){
        frame_rate_base*=10;
        frame_rate=(int)(fps*frame_rate_base + 0.5);
    }
    c->time_base.den = frame_rate;
    c->time_base.num = frame_rate_base;
    /* adjust time base for supported framerates */
    if(codec->supported_framerates){
        const AVRational *p= codec->supported_framerates;
        AVRational req = {frame_rate, frame_rate_base};
        const AVRational *best=NULL;
        AVRational best_error= {INT_MAX, 1};
        for(; p->den!=0; p++){
            AVRational error= av_sub_q(req, *p);
            if(error.num <0) error.num *= -1;
            if(av_cmp_q(error, best_error) < 0){
                best_error= error;
                best= p;
            }
        }
        if (best == NULL)
        {
            avcodec_free_context(&c);
            return NULL;
        }
        c->time_base.den= best->num;
        c->time_base.num= best->den;
    }

    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = pixel_format;
    if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
        c->max_b_frames = 2;
    }
    if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO || c->codec_id == AV_CODEC_ID_MSMPEG4V3){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesn't happen with normal video, it just happens here as the
           motion of the chroma plane doesn't match the luma plane */
        /* avoid FFMPEG warning 'clipping 1 dct coefficients...' */
        c->mb_decision=2;
    }

    /* Some settings for libx264 encoding, restore dummy values for gop_size
     and qmin since they will be set to reasonable defaults by the libx264
     preset system. Also, use a crf encode with the default quality rating,
     this seems easier than finding an appropriate default bitrate. */
    if (c->codec_id == AV_CODEC_ID_H264) {
      c->gop_size = -1;
      c->qmin = -1;
      c->bit_rate = 0;
      if (c->priv_data)
          av_opt_set(c->priv_data,"crf","23", 0);
    }

    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
      c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    st->avg_frame_rate = av_make_q(frame_rate, frame_rate_base);
    st->time_base = c->time_base;

    return c;
}

static const int OPENCV_NO_FRAMES_WRITTEN_CODE = 1000;

static int icv_av_write_frame_FFMPEG( AVFormatContext * oc, AVStream * video_st, AVCodecContext * c,
                                      uint8_t *, uint32_t,
                                      AVFrame * picture, int frame_idx)
{
    int ret;

	/* encode the image */
	if (picture == NULL && frame_idx == 0)
    {
		ret = OPENCV_NO_FRAMES_WRITTEN_CODE;
        MSG_ERR ("picture == NULL && frame_idx == %i", frame_idx);
        return ret;
    }
	else
	{
		ret = avcodec_send_frame(c, picture);
		if (ret < 0)
        {
          global_err = ret;
          MSG_ERR ("Error '%s' sending frame to encoder (avcodec_send_frame)", get_last_err_msg().c_str());
        }
	}

	while (ret >= 0)
	{
		AVPacket* pkt = av_packet_alloc();
		pkt->stream_index = video_st->index;
		ret = avcodec_receive_packet(c, pkt);

		//fprintf (stderr, "USE_AV_SEND_FRAME_API avcodec_receive_packet returned %i\n", ret);

		if (!ret)
		{
			av_packet_rescale_ts(pkt, c->time_base, video_st->time_base);
			ret = av_write_frame(oc, pkt);
			av_packet_free(&pkt);
			continue;
		}

		// FIXME von Andy: mir scheint bei der USE_AV_SEND_FRAME_API
		// ist das okay, wenn ein EAGAIN zurück kommt
		if (ret == AVERROR(EAGAIN))
		{
			//fprintf (stderr, "von Andy, Rückgabe von avcodec_receive_packet umbiegen...\n");
			ret = 0;
		}

		av_packet_free(&pkt);
		break;
	}
    return ret;
}

/// write a frame with FFMPEG
bool CvVideoWriter_FFMPEG::writeFrame( const unsigned char* data, int step, int width, int height, int cn, int origin )
{
    // check parameters
    if (input_pix_fmt == AV_PIX_FMT_BGR24) {
        if (cn != 3) {
            MSG_ERR("Only cn == 3 is allowed for AV_PIX_FMT_BGR24 (but is %i)", cn);
            return false;
        }
    }
    else if (input_pix_fmt == AV_PIX_FMT_GRAY8 || input_pix_fmt == AV_PIX_FMT_GRAY16LE) {
        if (cn != 1) {
            MSG_ERR("Only cn == 1 is allowed for AV_PIX_FMT_GRAY8 or AV_PIX_FMT_GRAY16LE (but is %i)", cn);
            return false;
        }
    }
    else {
        MSG_ERR("Input data does not match selected pixel format: %s, number of channels: %i",
                av_get_pix_fmt_name(input_pix_fmt), cn);
        CV_Assert(false);
    }

    // hint: in ::open, width and height was truncated to even number
    if(    (width & -2) != frame_width
        || (height & -2) != frame_height
        || !data )
    {
        MSG_ERR("width (%i) or height (%i) doesn't match truncated values (frame_width = %i, frame_height = %i) previously given to ::open", width, height, frame_width, frame_height);
        return false;
    }
    width = frame_width;
    height = frame_height;

    // FFmpeg contains SIMD optimizations which can sometimes read data past
    // the supplied input buffer.
    // Related info: https://trac.ffmpeg.org/ticket/6763
    // 1. To ensure that doesn't happen, we pad the step to a multiple of 32
    // (that's the minimal alignment for which Valgrind doesn't raise any warnings).
    // 2. (dataend - SIMD_SIZE) and (dataend + SIMD_SIZE) is from the same 4k page
    const int CV_STEP_ALIGNMENT = 32;
    const size_t CV_SIMD_SIZE = 32;
    const size_t CV_PAGE_MASK = ~(size_t)(4096 - 1);
    const unsigned char* dataend = data + ((size_t)height * step);
    if (step % CV_STEP_ALIGNMENT != 0 ||
        (((size_t)dataend - CV_SIMD_SIZE) & CV_PAGE_MASK) != (((size_t)dataend + CV_SIMD_SIZE) & CV_PAGE_MASK))
    {
        int aligned_step = (step + CV_STEP_ALIGNMENT - 1) & ~(CV_STEP_ALIGNMENT - 1);

        size_t new_size = (aligned_step * height + CV_SIMD_SIZE);

        if (!aligned_input || aligned_input_size < new_size)
        {
            if (aligned_input)
                av_freep(&aligned_input);
            aligned_input_size = new_size;
            aligned_input = (unsigned char*)av_mallocz(aligned_input_size);
        }

        if (origin == 1)
            for( int y = 0; y < height; y++ )
                memcpy(aligned_input + y*aligned_step, data + (height-1-y)*step, step);
        else
            for( int y = 0; y < height; y++ )
                memcpy(aligned_input + y*aligned_step, data + y*step, step);

        data = aligned_input;
        step = aligned_step;
    }

    AVPixelFormat sw_pix_fmt = context->pix_fmt;
    if ( sw_pix_fmt != input_pix_fmt ) {
        CV_Assert( input_picture );
        // let input_picture point to the raw data buffer of 'image'
        _opencv_ffmpeg_av_image_fill_arrays(input_picture, (uint8_t *) data,
                       (AVPixelFormat)input_pix_fmt, width, height);
        input_picture->linesize[0] = step;

        if( !img_convert_ctx )
        {
            img_convert_ctx = sws_getContext(width,
                                             height,
                                             (AVPixelFormat)input_pix_fmt,
                                             context->width,
                                             context->height,
                                             sw_pix_fmt,
                                             SWS_BICUBIC,
                                             NULL, NULL, NULL);
            if( !img_convert_ctx )
                return false;
        }

        if ( sws_scale(img_convert_ctx, input_picture->data,
                       input_picture->linesize, 0,
                       height,
                       picture->data, picture->linesize) < 0 )
            return false;
    }
    else{
        _opencv_ffmpeg_av_image_fill_arrays(picture, (uint8_t *) data,
                       (AVPixelFormat)input_pix_fmt, width, height);
        picture->linesize[0] = step;
    }

    bool ret;
    {
        picture->pts = frame_idx;
        int ret_write = icv_av_write_frame_FFMPEG(oc, video_st, context, outbuf, outbuf_size, picture, frame_idx);
        ret = ret_write >= 0 ? true : false;
    }

    frame_idx++;

    return ret;
}

/// close video output stream and free associated memory
void CvVideoWriter_FFMPEG::close()
{
    /* no more frame to compress. The codec has a latency of a few
       frames if using B frames, so we get the last frames by
       passing the same picture again */
    // TODO -- do we need to account for latency here?

    /* write the trailer, if any */
    if (picture && ok && oc)
    {
		for(;;)
		{
			int ret = icv_av_write_frame_FFMPEG( oc, video_st, context, outbuf, outbuf_size, NULL, frame_idx);
			if (ret == OPENCV_NO_FRAMES_WRITTEN_CODE || ret < 0 )
				break;
		}
        av_write_trailer(oc);
    }

    if( img_convert_ctx )
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = 0;
    }

    // free pictures
    if (picture && context && context->pix_fmt != input_pix_fmt)
    {
        if(picture->data[0])
            free(picture->data[0]);
        picture->data[0] = 0;
    }
    av_free(picture);

    if (input_picture)
        av_free(input_picture);

    avcodec_free_context(&context);

    av_free(outbuf);

    if (oc)
    {
        if (!(fmt->flags & AVFMT_NOFILE))
        {
            /* close the output file */
            avio_close(oc->pb);
        }

        /* free the stream */
        avformat_free_context(oc);
    }

    av_freep(&aligned_input);

    init();
}

#define CV_PRINTABLE_CHAR(ch) ((ch) < 32 ? '?' : (ch))
#define CV_TAG_TO_PRINTABLE_CHAR4(tag) CV_PRINTABLE_CHAR((tag) & 255), CV_PRINTABLE_CHAR(((tag) >> 8) & 255), CV_PRINTABLE_CHAR(((tag) >> 16) & 255), CV_PRINTABLE_CHAR(((tag) >> 24) & 255)

/// Create a video writer object that uses FFMPEG
bool CvVideoWriter_FFMPEG::open( const char * filename, int fourcc,
                                 double fps, int width, int height, bool is_color)
{
    InternalFFMpegRegister::init();

	AutoLock lock(_mutex);

    AVCodecID codec_id = AV_CODEC_ID_NONE;

    AVPixelFormat codec_pix_fmt;
    double bitrate_scale = 1;

    close();

	/* original code with depth in VideoParameter:
	   const int depth = params.get(VIDEOWRITER_PROP_DEPTH, CV_8U);
       const bool is_supported = depth == CV_8U || (depth == CV_16U && !is_color);
	   if (!is_supported)
		{
			MSG_ERR(Unsupported depth/isColor combination is selected, only CV_8UC1/CV_8UC3/CV_16UC1 are supported.");
			return false;
		}
	*/

    const int depth = CV_8U; //FIXME: interesting for octave-video wrapper?

    // check arguments
    if( !filename )
        return false;
    if(fps <= 0)
        return false;

    // we allow frames of odd width or height, but in this case we truncate
    // the rightmost column/the bottom row. Probably, this should be handled more elegantly,
    // but some internal functions inside FFMPEG swscale require even width/height.
    width &= -2;
    height &= -2;
    if( width <= 0 || height <= 0 )
        return false;

    fmt = av_guess_format(NULL, filename, NULL);

    if (fmt)
    {
        MSG_INFO ("Guessed format '%s' from filename '%s'", fmt->long_name, filename);
    }
    else
    {
        MSG_ERR ("Can't guess output format from filename '%s'", filename);
        return false;
    }

    /* determine optimal pixel format */
    if (is_color)
    {
        switch (depth)
        {
        case CV_8U: input_pix_fmt = AV_PIX_FMT_BGR24; break;
        default:
            MSG_ERR("Unsupported input depth %i for color image", depth);
            return false;
        }
    }
    else
    {
        switch (depth)
        {
        case CV_8U: input_pix_fmt = AV_PIX_FMT_GRAY8; break;
        case CV_16U: input_pix_fmt = AV_PIX_FMT_GRAY16LE; break;
        default:
            MSG_ERR("Unsupported input depth %i for grayscale image", depth);
            return false;
        }
    }
    MSG_INFO("Selected pixel format: '%s'", av_get_pix_fmt_name(input_pix_fmt));

    /* Lookup codec_id for given fourcc */
    if (fourcc < 0)
    {
        // use default from output format
        codec_id = fmt->video_codec;
        fourcc = av_codec_get_tag (fmt->codec_tag, codec_id);
    }
    else
    {
        codec_id = av_codec_get_id (fmt->codec_tag, fourcc);
        if (codec_id == AV_CODEC_ID_NONE)
        {
            MSG_INFO ("av_codec_get_id for tag '%c%c%c%c' failed, now try RIFF and MOV tags...", CV_TAG_TO_PRINTABLE_CHAR4(fourcc));

            const struct AVCodecTag * fallback_tags[] = {
                    avformat_get_riff_video_tags(),
                    avformat_get_mov_video_tags(),
                    //codec_bmp_tags, // fallback for avformat < 54.1
                    NULL };

            codec_id = av_codec_get_id(fallback_tags, fourcc);
            if (codec_id == AV_CODEC_ID_NONE)
            {
                MSG_INFO ("av_codec_get_id in RIFF+MOV tags for '%c%c%c%c' failed, now try to search descriptor name...", CV_TAG_TO_PRINTABLE_CHAR4(fourcc));
                char *p = (char *) &fourcc;
                char name[] = {(char)tolower(p[0]), (char)tolower(p[1]), (char)tolower(p[2]), (char)tolower(p[3]), 0};
                const AVCodecDescriptor *desc = avcodec_descriptor_get_by_name(name);
                if (desc)
                    codec_id = desc->id;

                if (codec_id == AV_CODEC_ID_NONE)
                {
                    MSG_ERR ("Giving up to find codec for tag 0x%08x/'%c%c%c%c' for output format '%s / %s -> ABORT",
                            fourcc, CV_TAG_TO_PRINTABLE_CHAR4(fourcc),
                            fmt->name, fmt->long_name);
                    return false;
                }
            }
        }
    }
    MSG_INFO ("Using codec '%s' for encoding, FOURCC = '%c%c%c%c'", avcodec_get_name (codec_id), CV_TAG_TO_PRINTABLE_CHAR4(fourcc));

    // alloc memory for context
    oc = avformat_alloc_context();
    CV_Assert(oc);

    /* set file name */
    oc->oformat = fmt;
    size_t name_len = strlen(filename);
    oc->url = (char*)av_malloc(name_len + 1);
    CV_Assert(oc->url);
    memcpy((void*)oc->url, filename, name_len + 1);
    oc->url[name_len] = '\0';

    /* set some options */
    oc->max_delay = (int)(0.7*AV_TIME_BASE);  /* This reduces buffer underrun warnings with MPEG */

    // set a few optimal pixel formats for lossless codecs of interest..
    switch (codec_id) {
    case AV_CODEC_ID_JPEGLS:
        // BGR24 or GRAY8 depending on is_color...
        // supported: bgr24 rgb24 gray gray16le
        // as of version 3.4.1
        codec_pix_fmt = input_pix_fmt;
        break;
    case AV_CODEC_ID_HUFFYUV:
        // supported: yuv422p rgb24 bgra
        // as of version 3.4.1
        switch(input_pix_fmt)
        {
            case AV_PIX_FMT_RGB24:
            case AV_PIX_FMT_BGRA:
                codec_pix_fmt = input_pix_fmt;
                break;
            case AV_PIX_FMT_BGR24:
                codec_pix_fmt = AV_PIX_FMT_RGB24;
                break;
            default:
                codec_pix_fmt = AV_PIX_FMT_YUV422P;
                break;
        }
        break;
    case AV_CODEC_ID_PNG:
        // supported: rgb24 rgba rgb48be rgba64be pal8 gray ya8 gray16be ya16be monob
        // as of version 3.4.1
        switch(input_pix_fmt)
        {
            case AV_PIX_FMT_GRAY8:
            case AV_PIX_FMT_GRAY16BE:
            case AV_PIX_FMT_RGB24:
            case AV_PIX_FMT_BGRA:
                codec_pix_fmt = input_pix_fmt;
                break;
            case AV_PIX_FMT_GRAY16LE:
                codec_pix_fmt = AV_PIX_FMT_GRAY16BE;
                break;
            case AV_PIX_FMT_BGR24:
                codec_pix_fmt = AV_PIX_FMT_RGB24;
                break;
            default:
                codec_pix_fmt = AV_PIX_FMT_YUV422P;
                break;
        }
        break;
    case AV_CODEC_ID_FFV1:
        // supported: MANY
        // as of version 3.4.1
        switch(input_pix_fmt)
        {
            case AV_PIX_FMT_GRAY8:
            case AV_PIX_FMT_GRAY16LE:
#ifdef AV_PIX_FMT_BGR0
            case AV_PIX_FMT_BGR0:
#endif
            case AV_PIX_FMT_BGRA:
                codec_pix_fmt = input_pix_fmt;
                break;
            case AV_PIX_FMT_GRAY16BE:
                codec_pix_fmt = AV_PIX_FMT_GRAY16LE;
                break;
            case AV_PIX_FMT_BGR24:
            case AV_PIX_FMT_RGB24:
#ifdef AV_PIX_FMT_BGR0
                codec_pix_fmt = AV_PIX_FMT_BGR0;
#else
                codec_pix_fmt = AV_PIX_FMT_BGRA;
#endif
                break;
            default:
                codec_pix_fmt = AV_PIX_FMT_YUV422P;
                break;
        }
        break;
    case AV_CODEC_ID_MJPEG:
    case AV_CODEC_ID_LJPEG:
        codec_pix_fmt = AV_PIX_FMT_YUVJ420P;
        bitrate_scale = 3;
        break;
    case AV_CODEC_ID_RAWVIDEO:
        // RGBA is the only RGB fourcc supported by AVI and MKV format
        if(fourcc == CV_FOURCC('R','G','B','A'))
        {
            codec_pix_fmt = AV_PIX_FMT_RGBA;
        }
        else
        {
            switch(input_pix_fmt)
            {
                case AV_PIX_FMT_GRAY8:
                case AV_PIX_FMT_GRAY16LE:
                case AV_PIX_FMT_GRAY16BE:
                    codec_pix_fmt = input_pix_fmt;
                    break;
                default:
                    codec_pix_fmt = AV_PIX_FMT_YUV420P;
                    break;
            }
        }
        break;
    default:
        // good for lossy formats, MPEG, etc.
        codec_pix_fmt = AV_PIX_FMT_YUV420P;
        break;
    }

    double bitrate = std::min(bitrate_scale*fps*width*height, (double)INT_MAX/2);

    if (codec_id == AV_CODEC_ID_NONE) {
        codec_id = av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_VIDEO);
    }

    // Add video stream to output file
    video_st = avformat_new_stream(oc, 0);
    if (!video_st) {
        CV_WARN("Could not allocate stream");
        return false;
    }

    AVDictionary *dict = NULL;

    // find and open encoder, try HW acceleration types specified in 'hw_acceleration' list (in order)
    int err = -1;
    const AVCodec* codec = NULL;
    do {
        codec = avcodec_find_encoder(codec_id);
        if (!codec)
        {
            MSG_ERR("Could not find encoder for codec '%s'(id = %i)", avcodec_get_name (codec_id), (int)codec_id);
            continue;
        }

        AVPixelFormat format = codec_pix_fmt;

        avcodec_free_context(&context);
        context = icv_configure_video_stream_FFMPEG(oc, video_st, codec,
                                              width, height, (int) (bitrate + 0.5),
                                              fps, format, fourcc);
        if (!context)
        {
            continue;
        }

        //av_dump_format(oc, 0, filename, 1);

        int64_t lbit_rate = (int64_t) context->bit_rate;
        lbit_rate += (int64_t)(bitrate / 2);
        lbit_rate = std::min(lbit_rate, (int64_t) INT_MAX);
        context->bit_rate_tolerance = (int) lbit_rate;
        context->bit_rate = (int) lbit_rate;

        /* open the codec */
        err = avcodec_open2(context, codec, NULL);
        if (err >= 0) {
            break;
        } else {
            MSG_ERR("Could not open codec '%s', error: %s (%i)", codec->name, icvFFMPEGErrStr(err), err);
        }
    } while (0);

    if (dict != NULL)
        av_dict_free(&dict);

    if (err < 0) {
        MSG_ERR("VIDEOIO/FFMPEG: Failed to initialize VideoWriter, err = %i", err);
        return false;
    }

    // Copy all to codecpar...
    // !!! https://stackoverflow.com/questions/15897849/c-ffmpeg-not-writing-avcc-box-information
    avcodec_parameters_from_context(video_st->codecpar, context);

	/* allocate output buffer */
	/* assume we will never get codec output with more than 4 bytes per pixel... */
	outbuf_size = width*height*4;
	outbuf = (uint8_t *) av_malloc(outbuf_size);

    bool need_color_convert;
    AVPixelFormat sw_pix_fmt = context->pix_fmt;

    need_color_convert = (sw_pix_fmt != input_pix_fmt);

    /* allocate the encoded raw picture */
    picture = icv_alloc_picture_FFMPEG(sw_pix_fmt, context->width, context->height, need_color_convert);
    if (!picture) {
        return false;
    }

    /* if the output format is not our input format, then a temporary
   picture of the input format is needed too. It is then converted
   to the required output format */
    input_picture = NULL;
    if ( need_color_convert ) {
        input_picture = icv_alloc_picture_FFMPEG(input_pix_fmt, context->width, context->height, false);
        if (!input_picture) {
            return false;
        }
    }

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE))
    {
        if (avio_open(&oc->pb, filename, AVIO_FLAG_WRITE) < 0)
        {
            return false;
        }
    }

    /* write the stream header, if any */
    global_err = avformat_write_header(oc, NULL);

    //printf ("global_err = %i = %s\n", global_err, get_last_err_msg ().c_str());
    //printf ("DEBUG codec_id = %i, fourcc = %i = '%c%c%c%c'\n", codec_id, fourcc, CV_TAG_TO_PRINTABLE_CHAR4(fourcc));

    if(global_err < 0)
    {
        MSG_ERR ("avformat_write_header failed with '%s'", get_last_err_msg ().c_str());
        close();
        remove(filename);
        return false;
    }
    frame_width = width;
    frame_height = height;
    frame_idx = 0;
    ok = true;

    return true;
}

/*
static
CvCapture_FFMPEG* cvCreateFileCaptureWithParams_FFMPEG(const char* filename, const VideoCaptureParameters& params)
{
    // FIXIT: remove unsafe malloc() approach
    CvCapture_FFMPEG* capture = (CvCapture_FFMPEG*)malloc(sizeof(*capture));
    if (!capture)
        return 0;
    capture->init();
    if (capture->open(filename, params))
        return capture;

    capture->close();
    free(capture);
    return 0;
}

void cvReleaseCapture_FFMPEG(CvCapture_FFMPEG** capture)
{
    if( capture && *capture )
    {
        (*capture)->close();
        free(*capture);
        *capture = 0;
    }
}
*/
/*
int cvSetCaptureProperty_FFMPEG(CvCapture_FFMPEG* capture, int prop_id, double value)
{
    return capture->setProperty(prop_id, value);
}
*/
//double cvGetCaptureProperty_FFMPEG(CvCapture_FFMPEG* capture, int prop_id)
//{
//    return capture->getProperty(prop_id);
//}
/*
int cvGrabFrame_FFMPEG(CvCapture_FFMPEG* capture)
{
    return capture->grabFrame();
}
*/
/* Original, was hat man sich da gedacht?
int cvRetrieveFrame_FFMPEG(CvCapture_FFMPEG* capture, unsigned char** data, int* step, int* width, int* height, int* cn)
{
    int depth = CV_8U;
    return cvRetrieveFrame2_FFMPEG(capture, data, step, width, height, cn, &depth);
}

int cvRetrieveFrame2_FFMPEG(CvCapture_FFMPEG* capture, unsigned char** data, int* step, int* width, int* height, int* cn, int* depth)
{
    return capture->retrieveFrame(0, data, step, width, height, cn, depth);
}
*/

/*
int cvRetrieveFrame_FFMPEG(CvCapture_FFMPEG* capture, unsigned char** data, int* step, int* width, int* height, int* cn, int* depth)
{
    return capture->retrieveFrame(0, data, step, width, height, cn, depth);
}
*/
/*
CvVideoWriter_FFMPEG* cvCreateVideoWriter_FFMPEG( const char* filename, int fourcc, double fps,
                                                  int width, int height, int isColor )
{
    CvVideoWriter_FFMPEG* writer = (CvVideoWriter_FFMPEG*)malloc(sizeof(*writer));
    if (!writer)
        return 0;
    writer->init();
    if( writer->open( filename, fourcc, fps, width, height, isColor != 0 ))
        return writer;
    writer->close();
    free(writer);
    return 0;
}

void cvReleaseVideoWriter_FFMPEG( CvVideoWriter_FFMPEG** writer )
{
    if( writer && *writer )
    {
        (*writer)->close();
        free(*writer);
        *writer = 0;
    }
}

int cvWriteFrame_FFMPEG( CvVideoWriter_FFMPEG* writer,
                         const unsigned char* data, int step,
                         int width, int height, int cn, int origin)
{
    return writer->writeFrame(data, step, width, height, cn, origin);
}
*/
