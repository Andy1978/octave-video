#include "cap_ffmpeg_impl_ov.hpp"

// PKG_ADD: autoload ("__ffmpeg_defines__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__ffmpeg_defines__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__ffmpeg_defines__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{def} =} __ffmpeg_defines__ ()\n\
undocumented internal function\n\
@end deftypefn")
{
  octave_value_list retval;
  octave_scalar_map opt;

  opt.contents ("LIBAVUTIL_BUILD") = LIBAVUTIL_BUILD;
  opt.contents ("LIBAVUTIL_IDENT") = LIBAVUTIL_IDENT;

  opt.contents ("LIBSWSCALE_BUILD") = LIBSWSCALE_BUILD;
  opt.contents ("LIBSWSCALE_IDENT") = LIBSWSCALE_IDENT;

  opt.contents ("LIBAVCODEC_BUILD") = LIBAVCODEC_BUILD;
  opt.contents ("LIBAVCODEC_IDENT") = LIBAVCODEC_IDENT;

  opt.contents ("LIBAVFORMAT_BUILD") = LIBAVFORMAT_BUILD;
  opt.contents ("LIBAVFORMAT_IDENT") = LIBAVFORMAT_IDENT;


  //join ident
  opt.contents ("LIBAV_IDENT") = LIBAVUTIL_IDENT ", " LIBSWSCALE_IDENT ", " LIBAVCODEC_IDENT ", " LIBAVFORMAT_IDENT;


  retval.append (opt);

  return retval;
}

// PKG_ADD: autoload ("__ffmpeg_output_formats__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__ffmpeg_output_formats__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__ffmpeg_output_formats__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{f} =} __ffmpeg_output_formats__ ()\n\
undocumented internal function\n\
@end deftypefn")
{
  av_register_all();

  octave_idx_type n = 0;

  // first loop to get numer of output formats
  AVOutputFormat * oformat = av_oformat_next(NULL);
  while (oformat != NULL)
  {
    n++;
    oformat = av_oformat_next (oformat);
  }

  Cell names (n, 1);
  Cell long_names (n, 1);
  Cell mime_types (n, 1);
  Cell extensions (n, 1);
  Cell codecs (n, 1);

  // second loop, now fill the cells
  oformat = av_oformat_next(NULL);
  int i = 0;
  while(oformat != NULL)
  {
    names (i) = oformat->name;
    long_names (i) = oformat->long_name;
    mime_types (i) = oformat->mime_type;
    extensions (i) = oformat->extensions;

    octave_map map_codecs;

    if (oformat->codec_tag)
      {
          // printf ("%s %s %s\n", oformat->name, oformat->long_name, oformat->mime_type);

          std::vector<std::string> video_codecs;
          const AVCodecTag * ptags = oformat->codec_tag[0];
          while (ptags->id != AV_CODEC_ID_NONE)
          {
              AVCodecID id = (AVCodecID) ptags->id;
              // get descriptor
              const AVCodecDescriptor* d = avcodec_descriptor_get (id);
              if (d)
                {
                  // only add encoder video codecs
                  if (d->type == AVMEDIA_TYPE_VIDEO)
                    {
                      // prüfen, ob es einen encoder gibt
                      if (avcodec_find_encoder (d->id))
                        {
                          unsigned int tag = ptags->tag;

                          if (! strcmp (oformat->name, "mp4")) // use riff
                            {
                              const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), 0 };
                              tag = av_codec_get_tag(table, id);
                            }

                          char buf[5];
                          snprintf (buf, 5, "%c%c%c%c", CV_TAG_TO_PRINTABLE_CHAR4(tag));

                          //printf("fourcc tag 0x%08x '%s' codec_id %04X\n", tag, buf, id);

                          video_codecs.push_back (buf);
                        }
                    }

                }

            ptags++;

          }

        // unique but keep order
        {
          auto last = std::unique(video_codecs.begin(), video_codecs.end());
          video_codecs.erase (last, video_codecs.end());
          Cell codec_fourcc (video_codecs.size (), 1);
          for (int k = 0; k < video_codecs.size (); ++k)
            codec_fourcc(k) = video_codecs[k];
          codecs (i) = codec_fourcc;
        }
      }

    oformat = av_oformat_next(oformat);
    i++;
  }

  octave_map m;

  m.assign ("name", names);
  m.assign ("long_name", long_names);
  m.assign ("mime_type", mime_types);
  m.assign ("extensions", extensions);
  m.assign ("codecs", codecs);

  return octave_value (m);
}

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

      // returns "valid" (true if open was successful)
      bool valid = h->open (filename.c_str ());
      if (valid)
        retval.append (octave_value (h));
      else
        error ("Opening '%s' failed : '%s'", filename.c_str (), get_last_err_msg().c_str ());
    }
  return retval;
}

// PKG_ADD: autoload ("__cap_get_properties__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__cap_get_properties__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__cap_get_properties__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {[@var{h}, @var{opt}] =} __cap_get_properties__ (@var{h})\n\
Gets CvCapture_FFMPEG properties like bitrate, fps, total_frames, duration_sec...\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  if (nargin != 1)
    error("__cap_get_properties__ needs one parameter");

  CvCapture_FFMPEG* h = get_cap_from_ov (args(0));
  if (h)
    {
      octave_scalar_map opt;
      opt.contents ("total_frames") = h->get_total_frames ();
      opt.contents ("duration_sec") = h->get_duration_sec ();
      opt.contents ("fps")          = h->get_fps ();
      opt.contents ("bitrate")      = h->get_bitrate ();
      opt.contents ("width")        = h->frame.width;
      opt.contents ("height")       = h->frame.height;

      // Current position of the video file in milliseconds
      opt.contents ("pos")          = (h->picture_pts == AV_NOPTS_VALUE_) ? 0 : h->dts_to_sec(h->picture_pts) * 1000;

      // Relative position of the video file: 0=start of the film, 1=end of the film.
      opt.contents ("rel_pos") = h->r2d(h->ic->streams[h->video_stream]->time_base);

      //  0-based index of the frame to be decoded/captured next.
      opt.contents ("frame_number") = h->frame_number;

      opt.contents ("video_codec_name") = h->get_video_codec_name ();

      // aspect ratio
      // 0, 1 is "undefined"
      {
        AVRational s = h->get_sample_aspect_ratio ();
        opt.contents ("aspect_ration_num") = s.num;
        opt.contents ("aspect_ration_den") = s.den;
      }

      retval.append (opt);
     }
  return retval;
}

// PKG_ADD: autoload ("__cap_grab_frame__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__cap_grab_frame__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__cap_grab_frame__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{f} =} __cap_grab_frame__ (@var{h})\n\
\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

  if (nargin != 1)
    error("__cap_grab_frame__ needs one parameter");

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

          for (int x = 0; x < width; ++x)
            for (int y = 0; y < height; ++y)
              for (int c = 0; c < cn; ++c)
                img (y, x, c) = data[x * cn + y * step + c];


          retval(0) = octave_value(img);

#endif
        }

    }
  return retval;
}

// PKG_ADD: autoload ("__cap_close__", "cap_ffmpeg_wrapper.oct");
// PKG_DEL: autoload ("__cap_close__", "cap_ffmpeg_wrapper.oct", "remove");
DEFUN_DLD(__cap_close__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{h} =} __cap_close__ (@var{h})\n\
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

  CvCapture_FFMPEG* p = get_cap_from_ov (args(0));
  if (p)
    p->close ();

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
@deftypefn {Loadable Function} {[@var{h}, @var{opt] =} __writer_open__ (@var{filename}, @var{fourcc}, @var{fps}, @var{width}, @var{height}, @var{isColor})\n\
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
#if 0
      if (foo->codec_tag)
        {
          const AVCodecTag * ptags = foo->codec_tag[0];
          while (ptags->id != AV_CODEC_ID_NONE)
          {
              unsigned int tag = ptags->tag;
              printf("fourcc tag 0x%08x/'%c%c%c%c' codec_id %04X\n", tag, CV_TAG_TO_PRINTABLE_CHAR4(tag), ptags->id);
              ptags++;
          }
        }
#endif

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

      //~ //cv_ff_codec_tag_dump (oformat->codec_tag);

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

      bool valid = h->open (filename.c_str (), tag, fps, width, height, isColor);
      if (valid)
        {
          retval.append (octave_value (h));

          if (nargout > 0)
            {
              octave_scalar_map opt;
              opt.contents ("ok")           = h->ok;
              opt.contents ("frame_width")  = h->frame_width;
              opt.contents ("frame_height") = h->frame_height;
              retval.append (opt);

              // FIXME: implement more
            }
        }
      else
        {
          // FIXME: CvVideoWriter_FFMPEG::open just returns false without explanation why
          error ("Opening '%s' for writing failed", filename.c_str ());
        }
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
