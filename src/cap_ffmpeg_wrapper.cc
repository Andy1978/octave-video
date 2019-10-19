#include "cap_ffmpeg_impl_ov.hpp"

/******************    CvCapture_FFMPEG     **************************/

CvCapture_FFMPEG* get_cap_from_ov (octave_value ov)
{
  if (!capture_type_loaded)
    {
      CvCapture_FFMPEG::register_type();
      capture_type_loaded = true;
    }

  if (ov.type_id() != CvCapture_FFMPEG::static_type_id())
    {
      error("get_handler_from_ov: Not a valid CvCapture_FFMPEG");
      return 0;
    }

  CvCapture_FFMPEG* p = 0;
  const octave_base_value& rep = ov.get_rep();
  p = &((CvCapture_FFMPEG &)rep);
  return p;
}

// PKG_ADD: autoload ("__cap_open__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__cap_open__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__cap_open__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{h} =} __cap_open__ (@var{filename})\n\
Creates an instance of CvCapture_FFMPEG.\n\
@seealso{getsnapshot}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  if (nargin != 1)
    {
      print_usage();
      return retval;
    }

  //if (!type_loaded)
  //  {
  //    CvCapture_FFMPEG::register_type();
  //    type_loaded = true;
  //  }
  std::string filename = args(0).string_value ();
  if (! error_state)
    {
      CvCapture_FFMPEG *h = new CvCapture_FFMPEG ();
      h->open (filename.c_str ());
      retval.append (octave_value (h));

      // FIXME: perhaps return these properties as struct?
      printf ("get_total_frames () = %i\n", h->get_total_frames ());
      printf ("get_duration_sec () = %f\n", h->get_duration_sec ());
      printf ("get_fps ()          = %f\n", h->get_fps ());
      printf ("get_bitrate ()      = %i\n", h->get_bitrate ());
    }
  return retval;
}

// PKG_ADD: autoload ("__cap_grab_frame__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__cap_grab_frame__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__cap_grab_frame__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{f} =} __cap_grab_frame__ (@var{h}, [@var{preview}])\n\
\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  //~ if (nargin < 1 || nargin>2)
    //~ {
      //~ print_usage ();
      //~ return retval;
    //~ }

  CvCapture_FFMPEG* p = get_cap_from_ov (args(0));
  if (p)
    return (octave_value (p->grabFrame ()));

  return retval;
}

// PKG_ADD: autoload ("__cap_retrieve_frame__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__cap_retrieve_frame__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__cap_retrieve_frame__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{f} =} __cap_retrieve_frame__ (@var{h}, [@var{preview}])\n\
\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  //~ if (nargin < 1 || nargin>2)
    //~ {
      //~ print_usage ();
      //~ return retval;
    //~ }

  CvCapture_FFMPEG* p = get_cap_from_ov (args(0));
  if (p)
    {
      unsigned char* data;
      int width = 0;
      int height = 0;
      int step;         // AVFrame::linesize, size in bytes of each picture line
      int cn;           // number of colors and should always be 3 here

      bool ret = p->retrieveFrame (0, &data, &step, &width, &height, &cn);

      //printf ("ret = %i, width = %i, height = %i, step = %i, cn = %i\n", ret, width, height, step, cn);

      assert (cn == 3);

      // step may be bigger because of padding
      assert (step >= width * cn);

      if (ret)
        {
#if 0
          // Attention: step and cn not handled yet
          dim_vector dv (3, step/cn, height);
          uint8NDArray img (dv);

          unsigned char *p = reinterpret_cast<unsigned char*>(img.fortran_vec());
          memcpy(p, data, img.numel ());

          Array<octave_idx_type> perm (dim_vector (3, 1));
          perm(0) = 2;
          perm(1) = 1;
          perm(2) = 0;

          // FIXME: howto handle padding? Extract submatrix with "extract"?
          retval(0) = octave_value(img.permute (perm));
#else

          dim_vector dv (height, width, cn);
          uint8NDArray img (dv);

          for (unsigned int x = 0; x < width; ++x)
            for (unsigned int y = 0; y < height; ++y)
              for (unsigned int c = 0; c < cn; ++c)
                img (y, x, c) = data[x * cn + y * step + c];


          retval(0) = octave_value(img);

#endif
        }

    }
  return retval;
}

/*************    CvVideoWriter_FFMPEG     ****************/

CvVideoWriter_FFMPEG* get_writer_from_ov (octave_value ov)
{
  if (!writer_type_loaded)
    {
      CvVideoWriter_FFMPEG::register_type();
      writer_type_loaded = true;
    }

  if (ov.type_id() != CvVideoWriter_FFMPEG::static_type_id())
    {
      error("get_handler_from_ov: Not a valid CvVideoWriter_FFMPEG");
      return 0;
    }

  CvVideoWriter_FFMPEG* p = 0;
  const octave_base_value& rep = ov.get_rep();
  p = &((CvVideoWriter_FFMPEG &)rep);
  return p;
}

// PKG_ADD: autoload ("__writer_open__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__writer_open__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__writer_open__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{h} =} __writer_open__ (@var{filename}, @var{fourcc}, @var{fps}, @var{width}, @var{height}, @var{isColor})\n\
undocumented internal function\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  if (nargin != 6)
    {
      print_usage();
      return retval;
    }

  if (! writer_type_loaded)
    {
      CvVideoWriter_FFMPEG::register_type();
      writer_type_loaded = true;
      av_register_all();
    }

  std::string filename = args(0).string_value ();

  // codec tag, in OpenCV "fourcc" is used interchangeably
  // empty fourcc selects default codec_id for guessed container
  unsigned int tag;
  std::string fourcc = args(1).string_value ();

  // FIXME no error handling yet
  double fps   = args(2).double_value ();
  int width    = args(3).int_value ();
  int height   = args(4).int_value ();
  bool isColor = args(5).bool_value ();

  if (fourcc.size () == 0)
    {
      // get tag for default codec for guessed container from filename
      AVOutputFormat* foo = av_guess_format	(NULL, filename.c_str (), NULL);

      // list supported codecs for guessed format
      if (foo->codec_tag)
        {
          const AVCodecTag * ptags = foo->codec_tag[0];
          while (ptags->id != AV_CODEC_ID_NONE)
          {
              unsigned int tag = ptags->tag;
              //printf("fourcc tag 0x%08x/'%c%c%c%c' codec_id %04X\n", tag, CV_TAG_TO_PRINTABLE_CHAR4(tag), ptags->id);
              ptags++;
          }
        }

      tag = av_codec_get_tag (foo->codec_tag, foo->video_codec);
    }
  else if (fourcc.size () == 4)
    {
      tag = MKTAG(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
    }
  else
    error ("fourcc has to be empty or 4 chars long");

  // list codecs
  //~ AVCodec * codec = av_codec_next(NULL);
  //~ while(codec != NULL)
  //~ {
      //~ fprintf(stderr, "%s\n", codec->long_name);
      //~ codec = av_codec_next(codec);
  //~ }

  // list formats
  //~ AVOutputFormat * oformat = av_oformat_next(NULL);
  //~ while(oformat != NULL)
  //~ {
      //~ printf ("%s; %s; %s; %s\n", oformat->name, oformat->long_name, oformat->mime_type, oformat->extensions);

      //~ cv_ff_codec_tag_dump (oformat->codec_tag);

      //~ oformat = av_oformat_next(oformat);
  //~ }

  //~ AVOutputFormat * oformat = av_oformat_next(NULL);
  //~ while(oformat != NULL)
  //~ {
      //~ fprintf(stderr, "%s\n", oformat->long_name);
      //~ if (oformat->codec_tag != NULL)
      //~ {
          //~ int i = 0;

          //~ CV_CODEC_ID cid = CV_CODEC(CODEC_ID_MPEG1VIDEO);
          //~ while (cid != CV_CODEC(CODEC_ID_NONE))
          //~ {
              //~ cid = av_codec_get_id(oformat->codec_tag, i++);
              //~ fprintf(stderr, "    %d\n", cid);
          //~ }
      //~ }
      //~ oformat = av_oformat_next(oformat);
  //~ }

  //printf ("tag = %i = %#x = %c%c%c%c\n", tag, tag, CV_TAG_TO_PRINTABLE_CHAR4(tag));

#if 0
  // that would be a workaround:
  AVOutputFormat* foo = av_guess_format	(NULL, "foo.mp4", NULL);
  printf ("default video_codec = %i = %#x\n", foo->video_codec, foo->video_codec);

  unsigned int tag = av_codec_get_tag (foo->codec_tag, AV_CODEC_ID_H264);
  printf ("tag = %i = %#x\n", tag, tag);

  // vom tag über riff zum codec_id:
  tag = MKTAG('H', '2', '6', '4');
  const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), 0 };
  enum AVCodecID id = av_codec_get_id (table, tag);
  printf ("id = %i = %#x, AV_CODEC_ID_H264 = %#x\n", id, id, AV_CODEC_ID_H264);

  // und zum tag zurück, Achtung, das ergibt nicht mehr 0x21
  tag = av_codec_get_tag (table, AV_CODEC_ID_H264);
  printf ("tag = %i = %#x = %c%c%c%c\n", tag, tag, CV_TAG_TO_PRINTABLE_CHAR4(tag));

#endif

  // welche API wäre denn von Octave aus gewünscht?
  // Ich denke direkt AVCodecID angeben wäre sinnvoller, als die fourcc

  /*
   * codecs anzeigen:
   * andy@Ryzen5Babe:~/Downloads/libav-12.3$ grep -r show_codecs
   * cmdutils_common_opts.h:    { "codecs"     , OPT_EXIT, {.func_arg = show_codecs   },    "show available codecs" },
   * cmdutils.h:int show_codecs(void *optctx, const char *opt, const char *arg);
   * cmdutils.c:int show_codecs(void *optctx, const char *opt, const char *arg)
   */

  if (! error_state)
    {
      CvVideoWriter_FFMPEG *h = new CvVideoWriter_FFMPEG ();

      // https://docs.opencv.org/3.4.1/dd/d9e/classcv_1_1VideoWriter.html#ac3478f6257454209fa99249cc03a5c59
      // fourcc	4-character code of codec used to compress the frames. For example,
      // VideoWriter::fourcc('P','I','M','1') is a MPEG-1 codec,
      // VideoWriter::fourcc('M','J','P','G') is a motion-jpeg codec etc.
      // List of codes can be obtained at Video Codecs by FOURCC page.
      // FFMPEG backend with MP4 container natively uses other values as fourcc code: see http://mp4ra.org/#/codecs,
      // so you may receive a warning message from OpenCV about fourcc code conversion.

      // fps	Framerate of the created video stream.
      // isColor	If it is not zero, the encoder will expect and encode color frames,
      // otherwise it will work with grayscale frames (the flag is currently supported on Windows only).

      //printf ("h->open (%s, %i, %f, %u, %u, %u);\n", filename.c_str (), tag, fps, width, height, isColor);

      bool ret = h->open (filename.c_str (), tag, fps, width, height, isColor);
      //printf ("open returned %i\n", ret);
      retval.append (octave_value (h));
    }
  return retval;
}

// PKG_ADD: autoload ("__writer_write_frame__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__writer_write_frame__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__writer_write_frame__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{h} =} __writer_write_frame__ (@var{h}, @var{frame})\n\
undocumented internal function\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  if (nargin != 2)
    {
      print_usage();
      return retval;
    }

  //NDArray f = args(1).array_value();
  uint8NDArray f = args(1).uint8_array_value();
  if (error_state)
    {
      error("__writer_write_frame__: frame should be a matrix");
      return retval;
    }

  CvVideoWriter_FFMPEG* p = get_writer_from_ov (args(0));
  if (p)
    {
      int width = f.columns ();
      int height = f.rows ();
      int cn = 3;
      int step = width * cn;
      int origin = 0;

      //printf ("width=%i, height=%i, step=%i\n", width, height, step);

      // permute, see also __cap_retrieve_frame__
      // for opposite

      Array<octave_idx_type> perm (dim_vector (3, 1));
      perm(0) = 2;
      perm(1) = 1;
      perm(2) = 0;

      f = f.permute (perm);

      unsigned char *t = reinterpret_cast<unsigned char*>(f.fortran_vec());

      bool ret = p->writeFrame (t, step, width, height, cn, origin);
      //printf ("ret = %i\n", ret);

    }
  return retval;
}

// PKG_ADD: autoload ("__writer_close__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__writer_close__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__writer_close__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{h} =} __writer_close__ (@var{h})\n\
undocumented internal function\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  if (nargin != 1)
    {
      print_usage();
      return retval;
    }

  CvVideoWriter_FFMPEG* p = get_writer_from_ov (args(0));
  if (p)
    p->close ();

  //printf ("__writer_close__\n");

  return retval;
}
