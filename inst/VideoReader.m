########################################################################
##
## Copyright (C) 2019-2023 Andreas Weber <octave@josoansi.de>
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
##
########################################################################

classdef VideoReader < handle

  ## -*- texinfo -*-
  ## @deftypefn {} {@var{p} =} VideoReader ()
  ## Create object @var{p} of the VideoReader class.
  ## @end deftypefn
  ##
  ## @deftypefn {} {} VideoReader.disp
  ## Outputs a list of all available properties.
  ##
  ## @end deftypefn
  ##
  ## @deftypefn {} {} VideoReader.readFrame
  ## Get next RGB24 frame. This also increases 'FrameNumber'.
  ##
  ## @end deftypefn
  ##
  ## @deftypefn {} {} __octave_video_set_verbosity_level__ (LEVEL)
  ## Internal function to increase chattiness of the underlying code
  ## for debugging purposes.
  ##
  ## @itemize
  ## @item 0: only errors
  ## @item 1: + warnings (default)
  ## @item 2: + info messages
  ## @item 3: + verbose info messages
  ## @item 4: + ffmpeg debug messages
  ## @end itemize
  ##
  ## @end deftypefn

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

      [v.h] = __cap_open__ (filename);

      v.FFmpeg_versions = __ffmpeg_defines__ ().LIBAV_IDENT;

      update_properties (v);

    endfunction

    function disp (v)

      printf (" class VideoReader:\n\n");
      printf ("  read-only access:\n");
      printf ("    Duration [s]    = %.2f\n", v.Duration);
      printf ("    BitsPerPixel    = %i\n", v.BitsPerPixel);
      printf ("    Bitrate         = %i\n", v.Bitrate);
      printf ("    FrameRate [fps] = %.2f\n", v.FrameRate);
      printf ("    Height [px]     = %i\n", v.Height);
      printf ("    Width [px]      = %i\n", v.Width);
      printf ("    Name            = '%s'\n", v.Name);
      printf ("    Path            = '%s'\n", v.Path);
      printf ("    NumberOfFrames  = %i\n", v.NumberOfFrames);
      printf ("    VideoFormat     = '%s'\n", v.VideoFormat);

      ## GNU Octave extensions
      printf ("    FrameNumber     = %i\n", v.FrameNumber);
      printf ("    VideoCodec      = '%s'\n", v.VideoCodec);
      printf ("    AspectRatio     = '%s'\n", mat2str (v.AspectRatio));
      printf ("    FFmpeg_versions = '%s'\n", v.FFmpeg_versions);

      printf ("\n");
      printf ("  read-write access:\n");
      printf ("    CurrentTime     = %i\n", v.CurrentTime);
      printf ("    Tag             = '%s'\n", v.Tag);

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

    function r = hasFrame (v)

      r = v.FrameNumber < v.NumberOfFrames;

    endfunction

    function close (v)

      __cap_close__ (v.h);
      update_properties (v);

    endfunction

  endmethods

  methods (Access = private)

    # internal function
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

# VideoReader currently is tested together with VideoWriter
%!test
%! assert (true);
