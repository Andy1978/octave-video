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
#include <octave/oct-map.h>

#include <octave/file-stat.h>
#include <octave/oct-time.h>
#include <ctime>

#include "AVHandler.h"

std::string get_filedate(const std::string &fn) {
    time_t file_mod = file_stat(fn).mtime().unix_time();
    
    char *timestr = new char[30];
    strftime(timestr, 30, "%d-%b-%Y %H:%M:%S", localtime(&file_mod));
    
    return std::string(timestr);
}

DEFUN_DLD(aviinfo, args, ,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{info} =} aviinfo (@var{filename})\n\
Return the properties of an AVI file.\n\
@end deftypefn\n\
\n\
@seealso{avifile, aviread, addframe}")
{
    octave_value_list retval;

    if (args.length() != 1) {
        print_usage();
        return retval;
    }
   
    std::string filename = args(0).string_value();
    if (error_state) {
        print_usage();
        return retval;
    }

    AVHandler av = AVHandler();
    av.set_filename(filename);
    av.set_log(&octave_stdout);

    if (av.setup_read() != 0) {
        error("aviread: AVHandler setup failed");
        return retval;
    }

    Octave_map m;

    m.assign("Filename", av.get_filename());
    m.assign("FileSize", av.get_filesize());
    m.assign("FileModDate", get_filedate(filename));
    m.assign("NumFrames", av.get_total_frames());
    m.assign("FramesPerSecond", av.get_framerate());
    m.assign("Width", av.get_width());
    m.assign("Height", av.get_height());
    m.assign("ImageType", "truecolor");
    m.assign("VideoCompression", av.get_codec());
    m.assign("Quality", 100);
    m.assign("NumColormapEntries", 0);
    m.assign("AudioFormat", av.get_audio_codec());
    m.assign("AudioRate", av.get_audio_samplerate());
    m.assign("NumAudioChannels", av.get_audio_channels());

    //    m.assign("Title", av.get_title());
    //    m.assign("Author", av.get_author());
    //    m.assign("Comment", av.get_comment());
 
    retval.append(octave_value(m));
    return retval;
}
