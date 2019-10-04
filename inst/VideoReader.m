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

classdef VideoReader < handle

  properties (SetAccess = private, GetAccess = public)

    Duration       = 123.4;    # [s]
    BitsPerPixel   = 24;       #
    FrameRate      = 30;       # [1/s]
    Height         = 100;      # [px]
    Width          = 100;      # [px]
    Name           = "foobar"; # filename
    Path           = "./";
    NumberOfFrames = 111;
    VideoFormat    = "RGB24";

  endproperties

  properties (SetAccess = public, GetAccess = public)

    CurrentTime    = 23.4;    # [s]
    Tag            = "world";

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
      filename
      varargin

      v.h = __cap_open__ (fullfile (v.Path, v.Name));

    endfunction

    function disp (v)

      disp ("this is VideoReader");

    endfunction

    function frame = readFrame (v)

      __cap_grab_frame__ (v.h);
      frame = __cap_retrieve_frame__ (v.h);

      # default ist BGR24, flip
      frame = flip (frame, 3);

    endfunction

    function r = hasFrame (v)

      r = true;

    endfunction

  endmethods

endclassdef
