/* Copyright (C) 2005 Stefan van der Walt <stefan@sun.ac.za>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef _AVHANDLER_H
#define _AVHANDLER_H

#define VIDEO_OUTBUF_SIZE 200000

// FIXME: should define -D__STDC_CONSTANT_MACROS instead of the following
#define INT64_C
#define UINT64_C
#define __STDC_CONSTANT_MACROS

#include <errno.h>
extern "C" {
#if defined (HAVE_FFMPEG_AVFORMAT_H)
#include <ffmpeg/avformat.h>
#elif defined(HAVE_LIBAVFORMAT_AVFORMAT_H)
#include <libavformat/avformat.h>
#else
#error "Missing ffmpeg headers"
#endif
}
#include <iostream>

class AVHandler {
 public:
  AVHandler() {     
    av_output = NULL;
    av_input = NULL;
    vstream = NULL;
    astream = NULL;
    frame = NULL;
    rgbframe = NULL;
    video_outbuf = NULL;
    filename = "";
    frame_nr = 0;
    
    bitrate = 400000;
    framerate = 25;
    gop_size = 10;
    width = 352;
    height = 288;
    codec_name = "msmpeg4v2";

    title = "";
    author = "";
    comment = "";

    lock_parameters = false;
  }
  
  ~AVHandler(void);

  int setup_write();

  int setup_read();

  void draw_background(unsigned char r, unsigned char g, unsigned char b);
 
  // write rgbframe to file
  int write_frame();

  // read frame nr from file into rgbframe
  int read_frame(unsigned int nr);
  
  static void print_file_formats();

  static void print_codecs();

  // The following routines can be used before `setup_write'

  void set_filename(const std::string &filename) {
    this->filename = filename;
  }

  std::string get_filename() const {
    return filename;
  }

  void set_codec(const std::string &codec) {
    codec_name = codec;
  }

  std::string get_codec() const {
    return codec_name;
  }

  std::string get_audio_codec() const {
    if (!astream) return "";

    AVCodec *codec;
    codec = avcodec_find_decoder(astream->codec->codec_id);
    if (!codec) {
	return "";
    }
    return std::string(codec->name);
  }

  unsigned int get_audio_samplerate() const {
    if (!astream) return 0;

    return astream->codec->sample_rate;
  }

  unsigned int get_audio_channels() const {
    if (!astream) return 0;

    return astream->codec->channels;
  }

  void set_bitrate(const unsigned int br) {
    bitrate = br;
  }

  unsigned int get_bitrate() const {
    return bitrate;
  }

  void set_framerate(double fr) {
    framerate = fr;
  }

  void set_gop_size(int gop) {
    gop_size = gop;
  }

  double get_framerate() const {
    return framerate;
  }

  void set_width(unsigned int width) {
    this->width = width;
  }

  unsigned int get_width() const {
    return width;
  }

  void set_height(unsigned int height) {
    this->height = height;
  }

  unsigned int get_height() const {
    return height;
  }

  unsigned int get_total_frames() const {
    if (vstream) {
      return (unsigned int) vstream->nb_frames;
    } else {
      return 0;
    }
  }

  unsigned int get_filesize() const {
    if (av_input) {
      return avio_size(av_input->pb);
    } else {
      return 0;
    }
  }

  void set_title(const std::string &t) {
    title = t;
  }

  std::string get_title() const {
    return title;
  }

  void set_author(const std::string &a) {
    author = a;
  }

  std::string get_author() const {
    return author;
  }

  void set_comment(const std::string &c) {
    comment = c;
  }

  std::string get_comment() const {
    return comment;
  }

  AVFrame *get_rgbframe() {
    return rgbframe;
  }

  static void set_log(std::ostream *log) {
    AVHandler::out = log;  }

 private:
  static std::ostream *out;
  
  AVFormatContext *av_output;
  AVFormatContext *av_input;
  AVStream *vstream;
  AVStream *astream;

  AVFrame *frame;
  AVFrame *rgbframe;
  
  uint8_t *video_outbuf;
  
  std::string filename;
  
  int frame_nr;

  unsigned int bitrate;
  double framerate;
  int gop_size;
  unsigned int width;
  unsigned int height;
  std::string codec_name;

  std::string title;
  std::string author;
  std::string comment;

  bool lock_parameters;
  
  int add_video_stream();
  
  int init_video_codecs();

  AVFrame *create_frame(PixelFormat fmt);

};

#endif
