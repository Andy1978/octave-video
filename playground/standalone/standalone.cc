/*
 * Playground to test implementation in cap_ffmpeg_impl_ov.hpp
 * (Not using functions in cap_ffmpeg_wrapper.cc)
 */

#include <iostream>
#include <octave/oct.h>

#include "cap_ffmpeg_impl_ov.hpp"

int main (void)
{
  set_verbosity_level (1); // errors, warnings, info and verbose debug messages (but not maximum = 4)
  std::string filename = "foo.mp4";

  CvVideoWriter_FFMPEG *w = new CvVideoWriter_FFMPEG ();

  double fps   = 30;
  int width    = 250;
  int height   = 150;
  int cn       = 3;
  bool isColor = 1;

  const AVOutputFormat* fmt = av_guess_format	(NULL, filename.c_str (), NULL);
  unsigned int tag = av_codec_get_tag (fmt->codec_tag, fmt->video_codec);
  printf("DEBUG: tag = '%c%c%c%c'\n", CV_TAG_TO_PRINTABLE_CHAR4(tag));

  bool valid = w->open (filename.c_str (), tag, fps, width, height, isColor);
  if (!valid)
    std::cerr << "Opening '" << filename << "' for writing failed" << std::endl;

  uint8NDArray f = uint8NDArray (dim_vector (height, width, cn));

  int step = width * cn;
  int origin = 0;

  printf ("DEBUG: width=%i, height=%i, cn=%i, step=%i\n", width, height, cn, step);

  Array<octave_idx_type> perm (dim_vector (3, 1));
  perm(0) = 2;
  perm(1) = 1;
  perm(2) = 0;
  f = f.permute (perm);

  unsigned char *t = reinterpret_cast<unsigned char*>(f.fortran_vec());
  for (int x = 0; x < 300; ++x)
  {
    for (int i = 0; i < width*height*cn; ++i)
      t[i] = i + 5*x;

    bool ret = w->writeFrame (t, step, width, height, cn, origin);
    if (! ret)
      std::cerr << "CvVideoWriter_FFMPEG::writeFrame failed" << std::endl;
  }

  w->close();

  CvCapture_FFMPEG *r = new CvCapture_FFMPEG ();
  valid = r->open (filename.c_str ());
  if (!valid)
    std::cerr << "Opening '" << filename << "' for reading failed" << std::endl;

  int frames_read = 0;
  while (r->grabFrame ())
  {
      unsigned char* data;
      int depth;        // 0 = CV_8U, 1 glaube ich signed, 2 = CV_16U

      bool ret = r->retrieveFrame (0, &data, &step, &width, &height, &cn, &depth);
      printf ("ret = %i, width = %i, height = %i, step = %i, cn = %i, depth = %i\n", ret, width, height, step, cn, depth);

      assert (cn == 3);
      frames_read++;
  }

  printf ("frames_read = %i\n", frames_read);
  r->close ();

  return 0;
}
