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

## ToDo: https://savannah.gnu.org/bugs/?func=detailitem&item_id=57020

classdef VideoWriter < handle

  properties (SetAccess = public, GetAccess = public)

    ColorChannels          = 3;
    Colormap               = [];
    CompressionRatio       = 10;
    Duration               = 0;  # [s]
    FileFormat             = "avi";
    Filename               = "";
    FrameCount             = 0;
    FrameRate              = 30;
    Height                 = [];
    Width                  = [];
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

      ## List formats: "ffmpeg -formats"
      ## ffmpeg -h muxer=matroska
      ## ffmpeg -codecs
      ## https://superuser.com/questions/300897/what-is-a-codec-e-g-divx-and-how-does-it-differ-from-a-file-format-e-g-mp/300997#300997

      if (numel (varargin) > 0)
        v.FourCC = varargin{1};
      endif

    endfunction

    function disp (v)

      printf(" class VideoWriter:\n");
      printf("    ColorChannels          = %i\n", v.ColorChannels);
      printf("    CompressionRatio       = %i\n", v.CompressionRatio);
      printf("    Duration               = %f\n", v.Duration);
      printf("    FileFormat             = %s\n", v.FileFormat);
      printf("    Filename               = %s\n", v.Filename);
      printf("    FrameCount             = %i\n", v.FrameCount);
      printf("    FrameRate              = %i\n", v.FrameRate);
      printf("    Height                 = %i\n", v.Height);
      printf("    Width                  = %i\n", v.Width);
      printf("    LosslessCompression    = %s\n", v.LosslessCompression);
      printf("    Path                   = %s\n", v.Path);
      printf("    Quality                = %i\n", v.Quality);
      printf("    VideoBitsPerPixel      = %i\n", v.VideoBitsPerPixel);
      printf("    VideoCompressionMethod = %s\n", v.VideoCompressionMethod);
      printf("    VideoFormat            = %s\n", v.VideoFormat);
      printf("    FourCC                 = %s\n", v.FourCC);

    endfunction

    function open (v)

      ## This implementation just opens a dummy file to check if the file
      ## can be created. The real video output is created on the first call
      ## of writeVideo.

      fn = fullfile (v.Path, v.Filename);
      [fid, msg] = fopen (fn, "w");
      if (fid < 0)
        error ("VideoWriter open failed: '%s'", msg);
      endif

      s = fputs (fid, "dummy\n");
      fclose (fid);

    endfunction

    function close (v)

      if (v.opened)
        __writer_close__ (v.h);
      endif

    endfunction

    function writeVideo (v, frame)

      if (! v.opened)

        ## if height/width isn't defined yet, use size of frame
        if (isempty (v.Width) || isempty (v.Height))
          v.Width = columns (frame);
          v.Height = rows (frame);
        endif

        # FIXME: isColor is set always true here...
        v.h = __writer_open__ (fullfile (v.Path, v.Filename), v.FourCC, v.FrameRate, v.Width, v.Height, true);
        v.opened = true;
      endif

      # default ist BGR24, flip RGB -> BGR
      frame = flip (frame, 3);

      __writer_write_frame__ (v.h, frame);

    endfunction

    function getProfiles (v)

    endfunction

  endmethods

endclassdef

%!demo
%! fn = fullfile (tempdir(), "sombrero.mp4");
%! w = VideoWriter (fn);
%! open (w);
%! z = sombrero ();
%! hs = surf (z);
%! axis manual
%! nframes = 100;
%! for ii = 1:nframes
%!   set (hs, "zdata", z * sin (2*pi*ii/nframes));
%!   drawnow
%!   x = getframe (gcf).cdata;
%!   writeVideo (w, uint8(x));
%! endfor
%! close (w)
%! printf ("Now run '%s' in your favourite video player or try 'demo VideoReader'!\n", fn);

%!test
%! fn = fullfile (tempdir(), "rainbow.mp4");
%! width = 200;
%! height = 150;
%! nframes = 120;
%! p = permute (rainbow (width), [3 1 2]);
%! raw_video = zeros (height, width, 3, nframes);
%! w = VideoWriter (fn);
%! for k=1:nframes
%!   ps = circshift (p, k * 6);
%!   img = uint8 (255 * repmat (ps, height, 1));
%!   raw_video (:, :, :, k) = img;
%!   writeVideo (w, img);
%! endfor
%! close (w)
%! ## read video and compare
%! clear -x raw_video fn
%! r = VideoReader (fn);
%! for k=1:size (raw_video, 4)
%!   img = readFrame (r);
%!   d = double (img) - raw_video(:,:,:,k);
%!   # this doesn't work well due to compression....
%!   rel_err = sum (abs(d(:)))/numel(d)/255;
%!   assert (rel_err < 0.01)
%! endfor
%! close (r);
