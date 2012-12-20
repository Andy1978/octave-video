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

#include "config.h"
#include <octave/oct.h>
#include "oct-avifile.h"

template <typename Num>
void setp(Num &p, Num v) {
    if (!error_state) {
        p = v;
    } else {
        error_state = 0;
    }
}

DEFUN_DLD(avifile, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{f} =} avifile (@var{filename}, [@var{parameter}, @var{value}, @dots{}])\n\
@deftypefnx {Loadable Function} avifile (\"codecs\")\n\
Create an AVI-format video file.\n\
\n\
The supported parameters are\n\
\n\
@table @asis\n\
@item @code{\"compression\"} or @code{\"codec\"}\n\
The type of encoder used (default: @code{\"msmpeg4v2\"})\n\
\n\
@item @code{\"fps\"}\n\
Encoding frame rate per second (default: @code{25.0})\n\
\n\
@item @code{\"gop\"}\n\
Group-of-pictures -- the number of frames after which a keyframe\n\
is inserted (default: @code{10})\n\
\n\
@item @code{\"bitrate\"}\n\
Encoding bitrate (default: @code{400000})\n\
@end table\n\
\n\
To see a list of the available codecs, do @code{avifile(\"codecs\")}.\n\
@end deftypefn\n\
\n\
@seealso{addframe, aviinfo, aviread}")
{
    octave_value_list retval;

    if ( (args.length() == 1) && (args(0).string_value() == "codecs") ) {
        AVHandler::print_codecs();
        return retval;
    }

    if ((args.length() == 0) || (args.length() % 2 != 1)) {
        print_usage();
        return retval;
    }

    std::string filename = args(0).string_value();
    if (error_state) {
        print_usage();
        return retval;
    }

    // Parse parameters
    std::string codec = "mpeg4";
    unsigned int bitrate = 400000;
    int gop_size = 10;
    double fps = 25;
    std::string title = "";
    std::string author = "";
    std::string comment = "Created using Octave-Avifile";

    for (unsigned int i = 1; i < args.length(); i++) {
        std::string p = args(i).string_value();
        octave_value v = args(i+1);
        if (!error_state) {
            if ((p == "codec") || (p == "compression")) { setp(codec, v.string_value()); }
            else if (p == "bitrate") { setp(bitrate, (unsigned int)v.int_value()); }
            else if (p == "gop") { setp(gop_size, v.int_value()); }
            else if (p == "fps") { setp(fps, v.double_value()); }
            else if (p == "title") { setp(title, v.string_value()); }
            else if (p == "author") { setp(author, v.string_value()); }
            else if (p == "comment") { setp(comment, v.string_value()); }
            else {
                error("avifile: unknown parameter \"%s\"", p.c_str());
                return retval;
            }
        }
        i++;
    }

    Avifile *m = new Avifile(filename);
    if (error_state) {
        return retval;
    }
    m->av->set_codec(codec);
    m->av->set_bitrate(bitrate);
    m->av->set_gop_size(gop_size);
    m->av->set_framerate(fps);

    // Doesn't look like these values are ever encoded
    m->av->set_title(title);
    m->av->set_author(author);
    m->av->set_comment(comment);

    retval.append(octave_value(m));
    return retval;
}

/*
%!test
%!  fn="test_avifile1.avi";
%!  m = avifile(fn, "codec", "mpeg4");
%!  for i = 1:100
%!    I = zeros(100,100);
%!    I(i,:) = i;
%!    I(:,i) = 200-i;
%!    addframe(m, I/255);
%!  endfor
%!  clear m
%!  assert(exist(fn,"file"))
%! 	delete(fn)
*/
