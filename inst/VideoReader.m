# https://de.mathworks.com/help/matlab/ref/videoreader.html

# opencv-3.4.1/modules/videoio/src/cap_ffmpeg_impl.hpp

classdef VideoReader < handle

  properties (SetAccess = private, GetAccess = public)
    
    Duration = 123;

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
  
  endmethods

endclassdef
