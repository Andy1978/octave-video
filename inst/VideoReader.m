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

  methods

    function v = VideoReader (filename, varargin)

      # varargin could be name/value property pairs
      filename
      varargin

    endfunction

    function disp (v)

      disp ("this is VideoReader");

    endfunction

    function frame = readFrame (v)

      frame = rand (10, 10, 3);

    endfunction

    function r = hasFrame (v)

      r = true;

    endfunction

  endmethods

endclassdef
