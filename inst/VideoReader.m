## Copyright (C) 2019 Andreas Weber <octave@josoansi.de>
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

  endproperties

  properties (SetAccess = public, GetAccess = public)

    CurrentTime    = 0;    # [s]
    Tag            = "";

  endproperties

  properties (Hidden, SetAccess = protected)

    h      = [];

  endproperties

  methods

    function v = VideoReader (filename, varargin)

      [v.Path, Filename, ext] = fileparts (filename);
      v.Name = [Filename ext];

      # varargin could be name/value property pairs
      # FIXME: implement me

      [v.h, opt] = __cap_open__ (fullfile (v.Path, v.Name));
      v.Duration = opt.duration_sec;
      v.FrameRate = opt.fps;
      v.NumberOfFrames = opt.total_frames;
      v.Bitrate = opt.bitrate;
      v.Width = opt.width;
      v.Height = opt.height;

    endfunction

    function disp (v)

      printf (" class VideoReader:\n");
      printf ("    Duration       = %fs\n", v.Duration);
      printf ("    BitsPerPixel   = %i\n", v.BitsPerPixel);
      printf ("    Bitrate        = %i\n", v.Bitrate);
      printf ("    FrameRate      = %.2ffps\n", v.FrameRate);
      printf ("    Height         = %i\n", v.Height);
      printf ("    Width          = %i\n", v.Width);
      printf ("    Name           = %s\n", v.Name);
      printf ("    Path           = %s\n", v.Path);
      printf ("    NumberOfFrames = %i\n", v.NumberOfFrames);
      printf ("    VideoFormat    = %s\n", v.VideoFormat);

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

    function r = hasFrame (v)

      # FIXME: implement me!
      r = true;

    endfunction

  endmethods

endclassdef

%!demo
%! fn = fullfile (tempdir(), "sombrero.mp4");
%! if (! exist (fn, "file"))
%!   error ("'%s' isn't available. Please run 'demo VideoWriter' first to create it!", fn);
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
