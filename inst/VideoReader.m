## Copyright (C) 2019-2020 Andreas Weber <octave@josoansi.de>
##
## This file is part of octave-video.
##
## This program is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; see the file COPYING.  If not, see
## <https://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {} {@var{p} =} VideoReader ()
## Create object @var{p} of the VideoReader class.
## @end deftypefn

## ToDo: https://savannah.gnu.org/bugs/?func=detailitem&item_id=57020

classdef VideoReader < handle

  properties (SetAccess = private, GetAccess = public)

    Duration       = 0;        # [s]
    BitsPerPixel   = 24;       #
    Bitrate        = 0;
    FrameRate      = 0;        # [1/s]
    Height         = 0;        # [px]
    Width          = 0;        # [px]
    Name           = "";       # filename
    Path           = "./";
    NumberOfFrames = 0;
    VideoFormat    = "RGB24";

    ## GNU Octave extensions
    FrameNumber    = 0;        # 0-based index of the frame to be decoded/captured next.
    VideoCodec     = "";       # Name of used video codec as obtained by AVCodecDescriptor->name
    AspectRatio    = [0, 1];
    FFmpeg_versions        = "";

  endproperties

  properties (SetAccess = public, GetAccess = public)

    CurrentTime    = 0;    # [s]
    Tag            = "";

  endproperties

  properties (Hidden, SetAccess = protected)

    h      = [];

  endproperties

  methods (Hidden)


  endmethods

  methods

    function v = VideoReader (filename, varargin)

      [v.Path, Filename, ext] = fileparts (filename);
      v.Name = [Filename ext];

      # varargin could be name/value property pairs
      # FIXME: implement me

      [v.h] = __cap_open__ (fullfile (v.Path, v.Name));

      v.FFmpeg_versions = __ffmpeg_defines__ ().LIBAV_IDENT;

      update_properties (v);

    endfunction

    function disp (v)

      printf (" class VideoReader:\n");
      printf ("    Duration [s]    = %.2f\n", v.Duration);
      printf ("    BitsPerPixel    = %i\n", v.BitsPerPixel);
      printf ("    Bitrate         = %i\n", v.Bitrate);
      printf ("    FrameRate [fps] = %.2f\n", v.FrameRate);
      printf ("    Height [px]     = %i\n", v.Height);
      printf ("    Width [px]      = %i\n", v.Width);
      printf ("    Name            = %s\n", v.Name);
      printf ("    Path            = %s\n", v.Path);
      printf ("    NumberOfFrames  = %i\n", v.NumberOfFrames);
      printf ("    FrameNumber     = %i\n", v.FrameNumber);
      printf ("    VideoFormat     = %s\n", v.VideoFormat);
      printf ("    VideoCodec      = %s\n", v.VideoCodec);
      printf ("    AspectRatio     = %s\n", mat2str (v.AspectRatio));

    endfunction

    function frame = readFrame (v)

      r = __cap_grab_frame__ (v.h);
      if (r)
        frame = __cap_retrieve_frame__ (v.h);
        # default ist BGR24, flip
        frame = flip (frame, 3);
      else
        frame = [];
      endif

    endfunction

    function val = get.FrameNumber (v)

      val = __cap_get_properties__ (v.h).frame_number;

    endfunction

    # internal function
    # FIXME: perhaps this can be hidden or method Access = private but what would be the benefit?
    function update_properties (v)

      prop = __cap_get_properties__ (v.h);

      v.Duration    = prop.duration_sec;
      v.FrameRate   = prop.fps;
      v.NumberOfFrames = prop.total_frames;
      v.FrameNumber    = prop.frame_number;
      v.Bitrate        = prop.bitrate;
      v.Width          = prop.width;
      v.Height         = prop.height;
      v.VideoCodec     = prop.video_codec_name;
      v.AspectRatio    = [prop.aspect_ration_num prop.aspect_ration_den];

    endfunction

    function r = hasFrame (v)

      r = v.FrameNumber < v.NumberOfFrames;

    endfunction

    function close (v)

      __cap_close__ (v.h);

    endfunction

  endmethods

endclassdef

%!demo
%! fn = fullfile (tempdir(), "sombrero.mp4");
%! if (! exist (fn, "file"))
%!   warning ("'%s' doesn't exist, running demo VideoWriter first...", fn);
%!   demo ("VideoWriter");
%! endif
%! x = VideoReader (fn);
%! im = [];
%! while (! isempty (img = readFrame (x)))
%!   if (isempty (im))
%!     im = image (img);
%!     axis off;
%!   else
%!     set (im, "cdata", img);
%!   endif
%!   drawnow
%!   pause (1/30);
%! endwhile

%!demo
%! r = VideoReader("https://raw.githubusercontent.com/opencv/opencv/master/samples/data/vtest.avi")
%! im = [];
%! while (r.hasFrame())
%!   img = readFrame (r);
%!   if (isempty (im))
%!     im = image (img);
%!     axis off;
%!   else
%!     set (im, "cdata", img);
%!   endif
%!   drawnow
%! endwhile

# VideoReader is currently tested with VideoWriter
%!test
%! assert (true);
