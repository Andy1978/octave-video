#include "cap_ffmpeg_impl_ov.hpp"

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

  CvCapture_FFMPEG* p = get_handler_from_ov (args(0));
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

  CvCapture_FFMPEG* p = get_handler_from_ov (args(0));
  if (p)
    {
      unsigned char* data;
      int step;
      int width = 0;
      int height = 0;
      int cn;
      bool ret = p->retrieveFrame(0, &data, &step, &width, &height, &cn);

      printf ("ret = %i, width = %i, height = %i\n", ret, width, height);

    }
  return retval;
}
