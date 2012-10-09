/* Copyright (C) 2004 Stefan van der Walt <stefan@sun.ac.za>

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

#include <octave/oct.h>
#include "oct-avifile.h"

DEFUN_DLD(addframe, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} addframe (@var{avi}, @var{image})\n\
Add a frame to an AVI file.\n\
\n\
@var{avi} is created using @code{avifile}.  @var{image} must be \n\
of class @code{double} and scaled to be in [0-1].\n\
@end deftypefn\n\
\n\
@seealso{avifile, aviread, aviinfo}")
{
    octave_value_list retval;

    if (args.length() != 2) {
        print_usage();
        return retval;
    }

    if (args(0).type_id() != Avifile::static_type_id()) {
        print_usage();
        return retval;
    }
    
    NDArray f = args(1).array_value();
    if (error_state) {
        error("addframe: frame should be a matrix");
        return retval;
    }

    // Check if input image is in [0-1]
    if (f.any_element_is_negative() || (1-f).any_element_is_negative()) {
        error("addframe: input frame should be in [0-1]");
        return retval;
    }

    Avifile *m = (Avifile*)args(0).internal_rep();
    m->addframe(f);
    if (error_state) {
        octave_stdout << "addframe: error adding frame to avifile" << std::endl;
    }

    return retval;
}

/*
%!test
%!  m = avifile("test.avi")
%!  addframe(m, zeros(2,2))
*/
