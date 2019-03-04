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
## @deftypefn {} {@var{p} =} VideoWriter ()
## Create object @var{p} of the VideoWriter class.
## @end deftypefn

## -*- texinfo -*-
## @deftypefn  {} {} open ()
## Open file for writing. All properties will become read-only.
## @end deftypefn

classdef VideoWriter < handle

  properties (SetAccess = private, GetAccess = public)

    ColorChannels          = 3;
    Colormap               = [];
    CompressionRatio       = 10;
    Duration               = 0;  # [s]
    FileFormat             = "avi";
    Filename               = "";
    FrameCount             = 0;
    FrameRate              = 30;
    Height                 = 480;
    Width                  = 640;
    LosslessCompression    = false;
    Path                   = "./";
    Quality                = 75;
    VideoBitsPerPixel      = 24;
    VideoCompressionMethod = "none";
    VideoFormat            = "foo";

    ## GNU Octave extensions
    FourCC                 = "";

  endproperties

  properties (Hidden, SetAccess = protected)

    h      = [];
    opened = false;

  endproperties

  methods

    function v = VideoWriter (filename, varargin)

      [v.Path, Filename, ext] = fileparts (filename);
      v.Filename = [Filename ext];

      ## Currently the container format is guess using the filename
      ## and the default video codec is used or you can set FourCC
      ## to force a specific codec.

      if (numel (varargin) > 0)
        v.FourCC = varargin{1};
      endif

    endfunction

    function disp (v)

      printf(" class VideoWriter:\n");
      printf("    ColorChannels = %i\n", v.ColorChannels);
      printf("    Duration      = %f\n", v.Duration);
      printf("    FileFormat    = %s\n", v.FileFormat);
      printf("    Filename      = %s\n", v.Filename);
      printf("    FourCC        = %s\n", v.FourCC);

    endfunction

    function open (v)

      ## This implementation doesn't make anything meaningful here.
      ## The problem is that CvVideoWriter_FFMPEG::open needs width and height
      ## and there are possible unknown until the first call to writeVideo

    endfunction

    function close (v)

    endfunction

    function writeVideo (v, frame)

      if (! v.opened)
        v.h = __writer_open__ (fullfile (v.Path, v.Filename), v.FourCC)
        v.opened = true;
      endif

      __writer_write_frame__ (v.h, frame);

    endfunction

    function getProfiles (v)

    endfunction

  endmethods

endclassdef
