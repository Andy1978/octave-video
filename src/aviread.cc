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

#include <octave/oct.h>
#include "AVHandler.h"

DEFUN_DLD(aviread, args, ,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{image} =} aviread (@var{filename}, @var{N})\n\
Load frame @var{N} from the AVI file @var{filename}.\n\
@end deftypefn\n\
\n\
@seealso{avifile, aviinfo, addframe}")
{
    octave_value_list retval;

    if (args.length() != 2) {
        print_usage();
        return retval;
    }
   
    std::string filename = args(0).string_value();
    if (error_state) {
        print_usage();
        return retval;
    }

    unsigned int framenr = (unsigned int)args(1).scalar_value();
    if (error_state) {
        print_usage();
    }

    AVHandler av = AVHandler();
    av.set_filename(filename);
    av.set_log(&octave_stdout);

    if (av.setup_read() != 0) {
        error("aviread: AVHandler setup failed");
        return retval;
    }

    if (av.read_frame(framenr) != 0) {
        error("aviread: cannot read frame %d", framenr);
        return retval;
    }

    AVFrame *frame = av.get_rgbframe();

    dim_vector d = dim_vector(av.get_height(), av.get_width(), 3);
    NDArray image = NDArray(d, 0);
    for (unsigned int y = 0; y < av.get_height(); y++) {
        for (unsigned int x = 0; x < av.get_width(); x++) {
          image(y, x, 0) = (double)frame->data[0][y * frame->linesize[0] + 3*x + 2]/255;
          image(y, x, 1) = (double)frame->data[0][y * frame->linesize[0] + 3*x + 1]/255;
          image(y, x, 2) = (double)frame->data[0][y * frame->linesize[0] + 3*x + 0]/255;
        }
    }

    retval.append(octave_value(image));
    return retval;
}

/*
%!demo
%! # Create an AVI with a low bitrate
%! x = avifile("test.avi");
%!
%! # Generate some picture
%! I = repmat(0:255, 256, 1)/255;
%! I = cat(3, I, repmat([0:255]', 1, 256)/255);
%! I = cat(3, I, 0.3*ones(256,256));
%!
%! # Display the picture
%! imshow(I(:,:,1), I(:,:,2), I(:,:,3));
%! 
%! # Write it to the AVI and close the AVI
%! addframe(x, I);
%! clear x
%! 
%! # Read the first frame from the AVI
%! I = aviread("test.avi", 1);
%!
%! # Display the frame read
%! imshow(I(:,:,1), I(:,:,2), I(:,:,3));
*/
