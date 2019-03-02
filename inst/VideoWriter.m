classdef VideoWriter < handle

  properties (SetAccess = private, GetAccess = public)

    ColorChannels          = 3;
    Colormap               = [];
    CompressionRatio       = 10;
    Duration               = 123.4;  # [s]
    FileFormat             = "avi";
    Filename               = "";
    FrameCount             = 0;
    FrameRate              = 30;
    Height                 = 100;
    Width                  = 100;
    LosslessCompression    = false;
    Path                   = "./";
    Quality                = 75;
    VideoBitsPerPixel      = 24;
    VideoCompressionMethod = "none";
    VideoFormat            = "foo";

  endproperties

  methods

    function v = VideoWriter (filename, varargin)

      # varargin could be name/value property pairs
      Filename = filename;
      varargin

    endfunction

    function disp (v)

      disp ("this is VideoWriter");

    endfunction

    function open (v)

    endfunction

    function close (v)

    endfunction

    function writeVideo (v, frame)

    endfunction

    function getProfiles (v)

    endfunction

  endmethods

endclassdef
