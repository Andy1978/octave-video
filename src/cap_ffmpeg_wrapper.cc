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

// PKG_ADD: autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
// PKG_DEL: autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"), "remove");
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
    }
  return retval;
}

// PKG_ADD: autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"));
// PKG_DEL: autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"), "remove");
DEFUN_DLD(__cap_grab_frame__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{f} =} __cap_grab_frame__ (@var{h}, [@var{preview}])\n\
Get a snapshot from v4l2_handler @var{h}\n\
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

// PKG_ADD: autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));
// PKG_DEL: autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"), "remove");
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

      assert (cn == 3);
      assert (step == width * 3);

      printf ("ret = %i, width = %i, height = %i, step = %i, cn = %i\n", ret, width, height, step, cn);

      if (ret)
        {
          dim_vector dv (3, width, height);
          uint8NDArray img (dv);

          // Achtung: step und cn noch nicht berücksichtigt
          unsigned char *p = reinterpret_cast<unsigned char*>(img.fortran_vec());
          memcpy(p, data, img.numel ());

          Array<octave_idx_type> perm (dim_vector (3, 1));
          perm(0) = 2;
          perm(1) = 1;
          perm(2) = 0;

          retval(0) = octave_value(img.permute (perm));
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

// PKG_ADD: autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
// PKG_DEL: autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"), "remove");
DEFUN_DLD(__writer_open__, args, nargout,
          "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{h} =} __writer_open__ (@var{filename})\n\
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

  //if (!type_loaded)
  //  {
  //    CvVideoWriter_FFMPEG::register_type();
  //    type_loaded = true;
  //  }
  std::string filename = args(0).string_value ();

  int fourcc   = 0x00000021;
  double fps   = 30.0;
  int width    = 100;
  int height   = 50;
  bool isColor = true;

  if (! error_state)
    {
      CvVideoWriter_FFMPEG *h = new CvVideoWriter_FFMPEG ();
      bool ret = h->open (filename.c_str (), fourcc, fps, width, height, isColor);
      printf ("open returned %i\n", ret);
      retval.append (octave_value (h));
    }
  return retval;
}

// PKG_ADD: autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));
// PKG_DEL: autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"), "remove");
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

      unsigned char *t = reinterpret_cast<unsigned char*>(f.fortran_vec());

      bool ret = p->writeFrame (t, step, width, height, cn, origin);
      printf ("ret = %i\n", ret);

    }
  return retval;
}

// PKG_ADD: autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
// PKG_DEL: autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"), "remove");
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

  return retval;
}
