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
#include "oct-avifile.h"

DEFINE_OCTAVE_ALLOCATOR(Avifile);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA(Avifile, "avifile", "avifile");

Avifile::Avifile(std::string fn) : octave_base_value(),
    filename(fn), frames(0), frame_rows(0), frame_columns(0) {

    av = new AVHandler();
    av->set_filename(fn);
    av->set_log(&octave_stdout);
}

Avifile::Avifile(const Avifile& m) {
    filename = m.filename;
    frames = m.frames;
    frame_rows = m.frame_rows;
    frame_columns = m.frame_columns;
    av = m.av;

    octave_stdout << "avifile: copy constructor shouldn't be called" << std::endl;
}

void
Avifile::print(std::ostream& os, bool pr_as_read_syntax = false) const {
    os << "AVI movie [" << filename << "][" << av->get_codec() << "]: "
       << frames << " frame" << (frames != 1 ? "s" : "") << ", "
       << frame_rows << "x" << frame_columns << "\n";
}

void
Avifile::addframe(const NDArray &f) {
    if (frames == 0) {
//      FIXME: this suppresses warnings from ffmpeg, but completely prevents
//             working with many videos.
//      if ( (f.columns() % 2 != 0) || (f.rows() % 2 != 0) ) {
//          error("avifile: matrix dimensions must be multiple of two");
//          return;
//      }

        if ( (f.columns() == 0) || (f.rows() == 0) ) {
            error("avifile: matrix must have non-zero dimensions");
            return;
        }

        frame_columns = f.columns();
        frame_rows = f.rows();

        av->set_height(frame_rows);
        av->set_width(frame_columns);

        if (av->setup_write() != 0) {
            error("avifile: AVHandler setup failed");
            return;
        }
    }
    if ( (frame_columns != f.columns()) ||
         (frame_rows != f.rows()) ) {
        error("avifile: all frames must have the same dimensions (%dx%d)",
              frame_rows, frame_columns);
        return;
    }

    // convert matrix to AVFrame
    AVFrame *rgbframe = av->get_rgbframe();

    dim_vector d = f.dims();
    unsigned char bands = 0;

    if  ( (d.length() == 3) && (d(2) == 3) ) {
        // RGB image
        bands = 3;
    } else if ( d.length() == 2 ) {
        // gray or B&W image
        bands = 1;
    } else {
        error("avifile: invalid matrix dimensions");
        return;
    }
    
    for (unsigned int y = 0; y < frame_rows; y++) {
      for (unsigned int x = 0; x < frame_columns; x++) {
        if (bands == 3) {
          rgbframe->data[0][y * rgbframe->linesize[0] + 3*x + 2] = (unsigned char)(f(y,x,0)*255);
          rgbframe->data[0][y * rgbframe->linesize[0] + 3*x + 1] = (unsigned char)(f(y,x,1)*255);
          rgbframe->data[0][y * rgbframe->linesize[0] + 3*x + 0] = (unsigned char)(f(y,x,2)*255);
        }
        else {
          rgbframe->data[0][y * rgbframe->linesize[0] + 3*x + 0] = (unsigned char)(f(y,x)*255);
          rgbframe->data[0][y * rgbframe->linesize[0] + 3*x + 1] = (unsigned char)(f(y,x)*255);
          rgbframe->data[0][y * rgbframe->linesize[0] + 3*x + 2] = (unsigned char)(f(y,x)*255);
        }
      }
    }
    
    if (av->write_frame() < 0) {
        error("avifile: error writing frame");
        return;
    }
    frames++;
}

Avifile::~Avifile(void) {
    octave_stdout << "avifile: writing headers and closing " << filename << std::endl;
    delete av;
}

