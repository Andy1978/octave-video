/*
 * Playground to test implementation in cap_ffmpeg_impl_ov.hpp
 * (Not using (or bypassing) functions in cap_ffmpeg_wrapper.cc)
 */

#include <iostream>
#include <octave/oct.h>

#include "cap_ffmpeg_impl_ov.hpp"

/*
static unsigned get_codecs_sorted(const AVCodecDescriptor ***rcodecs)
{
    const AVCodecDescriptor *desc = NULL;
    const AVCodecDescriptor **codecs;
    unsigned nb_codecs = 0, i = 0;

    while ((desc = avcodec_descriptor_next(desc)))
        nb_codecs++;
    if (!(codecs = (const AVCodecDescriptor **) av_calloc(nb_codecs, sizeof(*codecs))))
        fprintf (stderr, "out of memory\n");
    desc = NULL;
    while ((desc = avcodec_descriptor_next(desc)))
        codecs[i++] = desc;
    assert(i == nb_codecs);
    //qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
    *rcodecs = codecs;
    return nb_codecs;
}
*/
int main (void)
{
/*
    const AVCodecDescriptor **codecs;
    unsigned i, nb_codecs = get_codecs_sorted(&codecs);
*/

  set_verbosity_level (3);
  //std::string filename = "foo.mkv";
  std::string filename = "foo.mp4";

  CvVideoWriter_FFMPEG *w = new CvVideoWriter_FFMPEG ();

  double fps   = 30;
  int width    = 250;
  int height   = 150;
  int cn       = 3;
  bool isColor = 1;

/*
  const AVOutputFormat* fmt = av_guess_format	(NULL, filename.c_str (), NULL);

	printf ("name        = '%s'\n", fmt->name);
	printf ("long_name   = '%s'\n", fmt->long_name);
	printf ("mime_type   = '%s'\n", fmt->mime_type);
	printf ("extensions  = '%s'\n", fmt->extensions);
	printf ("audio_codec = %i = '%s'\n", fmt->audio_codec, avcodec_get_name (fmt->audio_codec));
	printf ("video_codec = %i = '%s'\n", fmt->video_codec, avcodec_get_name (fmt->video_codec));
	printf ("codec_tag   = %p\n", fmt->codec_tag);
*/

  //unsigned int tag = av_codec_get_tag (fmt->codec_tag, fmt->video_codec);

  //unsigned int tag = av_codec_get_tag (fmt->codec_tag, AV_CODEC_ID_LJPEG);
  //printf("DEBUG: tag = %i = '%c%c%c%c'\n", tag, CV_TAG_TO_PRINTABLE_CHAR4(tag));

  // override
  //tag = MKTAG ('w', 'e', 'b', 'p');
  //tag = MKTAG ('0', '1', '2', 'v');

  int tag = -1; // use default from output format

  //tag = MKTAG ('m', 'r', 'l', 'e');
  //tag = MKTAG ('w', 'm', 'v', '2');
  //tag = MKTAG ('V', 'P', '9', '0');

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

#if 0
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
#endif

  return 0;
}
