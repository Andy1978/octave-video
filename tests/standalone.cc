/*
 * Playground to test implementation in cap_ffmpeg_impl_ov.hpp
 */

#include <iostream>
#include <octave/oct.h>

#include "cap_ffmpeg_impl_ov.hpp"


int
main (void)
{
  set_verbosity_level (3);
  
  std::string filename = "foo.mov";

  // codec tag, in OpenCV "fourcc" is used interchangeably
  // empty fourcc selects default codec_id for guessed container
  unsigned int tag;
  //std::string fourcc = "MPEG";
  //tag = MKTAG(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);

  const AVOutputFormat* fmt = av_guess_format	(NULL, filename.c_str (), NULL);
  if (! fmt)
		error ("Can't guess container format from filename '%s'", filename.c_str ());
  
  if (0 && fmt && fmt->codec_tag)
    {
      const AVCodecTag * ptags = fmt->codec_tag[0];
      while (ptags->id != AV_CODEC_ID_NONE)
        {
          unsigned int tag = ptags->tag;
          printf("fourcc tag 0x%08x/'%c%c%c%c' codec_id %04X\n", tag, CV_TAG_TO_PRINTABLE_CHAR4(tag), ptags->id);
          ptags++;
        }
    }

  // guess codec
  //AVCodecID x = av_guess_codec (fmt, NULL, filename.c_str ()

  tag = av_codec_get_tag (fmt->codec_tag, fmt->video_codec);

  printf("fourcc tag 0x%08x/'%c%c%c%c'\n", tag, CV_TAG_TO_PRINTABLE_CHAR4(tag));

  CvVideoWriter_FFMPEG *h = new CvVideoWriter_FFMPEG ();

  double fps   = 30;
  int width    = 25;
  int height   = 15;
  bool isColor = 1;
  
  bool valid = h->open (filename.c_str (), tag, fps, width, height, isColor);
  if (!valid)
    {
      // FIXME: CvVideoWriter_FFMPEG::open just returns false without explanation why
      error ("Opening '%s' for writing failed", filename.c_str ());
    }
    
  return 0;
}
