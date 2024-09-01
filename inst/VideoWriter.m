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

classdef VideoWriter < handle

  ## -*- texinfo -*-
  ## @deftypefn {} {@var{p} =} VideoWriter ()
  ## Create object @var{p} of the VideoWriter class.
  ## @end deftypefn
  ##
  ## @deftypefn {} {} VideoWriter.open
  ## Just checks if the file can be created, no need to call this before
  ## 'writeVideo' in this implementation.
  ##
  ## @end deftypefn
  ##
  ## @deftypefn {} {} VideoWriter.writeVideo
  ## This ultimately creates the video file with previously set params.
  ## Width and height of the first frame given determines the width and
  ## height of the video.
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

  properties (SetAccess = public, GetAccess = public)
    FrameRate              = 30;      # fps in [Hz]
  endproperties

  properties (SetAccess = private, GetAccess = public)

    ColorChannels          = 3;
    #Colormap               = [];       # not yet implemented
    #CompressionRatio       = 10;       # not yet implemented
    Duration               = 0;       # [s]
    FileFormat             = "avi";
    Filename               = "";
    FrameCount             = 0;

    # Height/Width is set by the first frame written and can't be changed after that
    Height                 = [];      # height of the video frames which can be different than requested due to padding or cropping
    Width                  = [];      # width of the video frames which can be different than requested due to padding or cropping

    #LosslessCompression    = false;  # FIXME: currently not used
    Path                   = "./";
    #Quality                = 75;     # FIXME: currently not used
    VideoBitsPerPixel      = 24;      # 8 * ColorChannels
    VideoCompressionMethod = "";      # Used video codec, for exmaple "h264" or "mpeg4"
    VideoFormat            = "";      # descriptive container format for example
                                      # "MP4 (MPEG-4 Part 14)" or "AVI (Audio Video Interleaved)"

    ## GNU Octave extensions
    FourCC                 = "";
    FFmpeg_versions        = "";

  endproperties

  properties (Hidden, SetAccess = protected)

    h      = [];
    opened = false;

  endproperties

  methods (Access = private)

    function update_variable_properties (v)

      ## update properties which may change with calls to writeVideo
      if (v.opened)
        opt = __writer_get_properties__ (v.h);
        v.FrameCount = opt.frame_idx;
        v.Duration = opt.frame_idx / v.FrameRate;
      else
        #warning ("VideoWriter isn't opened");
        # I think just don't update is the best here
        ;
      endif

    endfunction

  endmethods

  methods

    ## call VideoWriter ("foo.mpg", "xxx")
    function v = VideoWriter (filename, varargin)

      [v.Path, Filename, ext] = fileparts (filename);
      v.Filename = [Filename ext];

      ## Currently the container format is guessed using the filename extension
      ## and the default video codec is used or you can set FourCC
      ## to force a specific codec.

      ## List formats: "ffmpeg -formats"
      ## ffmpeg -h muxer=matroska
      ## ffmpeg -codecs
      ## https://superuser.com/questions/300897/what-is-a-codec-e-g-divx-and-how-does-it-differ-from-a-file-format-e-g-mp/300997#300997

      if (numel (varargin) > 0 && !isempty (varargin{1}))
        v.FourCC = varargin{1};
      else
        v.FourCC = "";
      endif

      v.FFmpeg_versions = __ffmpeg_defines__ ().LIBAV_IDENT;

    endfunction

    function disp (v)

      update_variable_properties (v);

      printf(" class VideoWriter:\n");
      printf("    ColorChannels          = %i\n", v.ColorChannels);
      #printf("    CompressionRatio       = %i\n", v.CompressionRatio);
      printf("    Duration [s]           = %.2f\n", v.Duration);
      printf("    FileFormat             = %s\n", v.FileFormat);
      printf("    Filename               = %s\n", v.Filename);
      printf("    FrameCount             = %i\n", v.FrameCount);
      printf("    FrameRate [1/s]        = %i\n", v.FrameRate);
      printf("    Height [px]            = %i\n", v.Height);
      printf("    Width [px]             = %i\n", v.Width);
      #printf("    LosslessCompression    = %s\n", v.LosslessCompression);
      printf("    Path                   = %s\n", v.Path);
      #printf("    Quality                = %i\n", v.Quality);
      printf("    VideoBitsPerPixel      = %i\n", v.VideoBitsPerPixel);
      printf("    VideoCompressionMethod = %s\n", v.VideoCompressionMethod);
      printf("    VideoFormat            = %s\n", v.VideoFormat);
      printf("    FourCC                 = %s\n", v.FourCC);
      printf("    FFmpeg_versions        = %s\n", v.FFmpeg_versions);

    endfunction

    function open (v)

      ## We can't create an instance of CvVideoWriter_FFMPEG right now
      ## because width,height, codec and so on aren't known yet.
      ##
      ## This implementation just opens the given filename to check if the
      ## file could be created. A new instance of CvVideoWriter_FFMPEG is
      ## created on the first call to writeVideo.

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
        v.opened = false;
      endif

    endfunction

    function writeVideo (v, in)

      # TODO / IDEAS:
      # pantxo: allow passing arbitrary image data types even if we end up converting them internally
      # to whatever is prefered for the chosen codec, e.g uint8 for RGB 24bits video formats.

      # input can be an image or a frame returned by getframe
      is_frame = isstruct (in) && isfield (in, "cdata");
      is_img = isnumeric (in);

      if (! is_frame && ! is_img)
        error ("writeVideo: 'in' has to be a numeric matrix or a frame with cdata")
      endif

      if (is_frame)
        in = in.cdata;
      endif

      if (! v.opened)

        n_ch = size (in, 3);
        if (n_ch != 1 && n_ch != 3)
          error ("writeVideo: 'in' has to be a 'H x W x 1' (grayscale or indexed) or 'H x W x 3' matrix (RGB)");
        endif

        v.ColorChannels = n_ch;
        v.VideoBitsPerPixel = 8 * v.ColorChannels;

        v.h = __writer_open__ (fullfile (v.Path, v.Filename), v.FourCC, v.FrameRate, columns (in), rows (in), v.ColorChannels == 3);

        ## update properties
        opt = __writer_get_properties__ (v.h);
        v.Width = opt.frame_width;
        v.Height = opt.frame_height;
        v.VideoFormat = opt.output_format_long_name;
        v.VideoCompressionMethod = opt.output_video_stream_codec;

        v.opened = true;
      endif

      # default ist BGR24, flip RGB -> BGR
      in = flip (in, 3);

      __writer_write_frame__ (v.h, in);

    endfunction

    function v = set.FrameRate (v, fps)

      if (v.opened && ! isequal (fps, v.FrameRate))
        error (["VideoWriter: cannot change the FrameRate propery of ", ...
                "an already open VideoWriter object"]);
      elseif (! isnumeric (fps) || ! isscalar (fps) || iscomplex (fps)
              || fps <= 0)
        error ("VideoWriter: FrameRate must be a positive scalar value");
      else
        v.FrameRate = fps;
      endif

    endfunction

    function val = get.Duration (v)

      update_variable_properties (v);
      val = v.Duration;

    endfunction

    function val = get.FrameCount (v)

      update_variable_properties (v);
      val = v.FrameCount;

    endfunction

  endmethods

endclassdef

%!demo
%! fn = fullfile (tempdir (), "sombrero.mp4");
%! w = VideoWriter (fn);
%! w.FrameRate = 50;
%! open (w);
%! z = sombrero ();
%! hs = surf (z);
%! axis manual
%! nframes = 200;
%! for ii = 1:nframes
%!   set (hs, "zdata", z * sin (2*pi*ii/nframes + pi/5));
%!   drawnow
%!   writeVideo (w, getframe (gcf));
%! endfor
%! close (w)
%! printf ("Now run 'open %s' to read the video with your default video player or try 'demo VideoReader'!\n", fn);

# Create synthetic video (rainbow circshifting from left to right),
# encode and decode it and compare decoded video with original data.
# Throw error if relative deviation exceeds given threshold [%].
#
# This check is error prone since codecs may produce more artifacts on
# different architectures. See bugreports:
#   (https://savannah.gnu.org/bugs/?58451)
#   (https://savannah.gnu.org/bugs/?64452)
#     both for ppc64el
#
%!function encode_decode (fn, tag, thres, exp_size)
%!
%! width = 200;
%! height = 150;
%! nframes = 120;
%! p = permute (rainbow (width), [3 1 2]);
%! raw_video = zeros (height, width, 3, nframes);
%! w = VideoWriter (fn, tag);
%! for k=1:nframes
%!   ps = circshift (p, k * 6, 2);
%!   img = uint8 (255 * repmat (ps, height, 1));
%!   raw_video (:, :, :, k) = img;
%!   writeVideo (w, img);
%! endfor
%! close (w)
%!
%! ## read video and compare
%! clear -x raw_video fn thres exp_size
%! r = VideoReader (fn);
%! n = size (raw_video, 4);
%! rel_err = zeros (n, 1);
%! for k=1:n
%!   img = readFrame (r);
%!   #img(1,1,1)
%!   d = double (img) - raw_video(:,:,:,k);
%!   rel_err(k) = sum (abs(d(:)))/numel(d)/255;
%! endfor
%! close (r);
%! median_rel_error = 1e2 * median(rel_err); # in %
%! printf ("INFO: median(relative error) = %.2f%%\n", median_rel_error);
%! if (median_rel_error > thres)
%!   error ("The median of the relative error exceeds the given threshold (%.2f%% > %.2f%%).\
%! Please inspect '%s' manually. You should see a horizontal rainbow running\
%! from left to right.", median_rel_error, thres, fn);
%! endif
%! s = stat (fn);
%! printf ("INFO: size = %i kB\n", s.size / 1e3);
%! rel_size_err = abs(exp_size - s.size) / exp_size;
%! rel_size_err_thres = 0.3; # 30%
%! if (rel_size_err > rel_size_err_thres)
%!   error ("The difference between the expected (%i kB) and observed (%i kB)\
%! filesize of the written video exceeds the given relative threshold (%.1f%%).\
%! Please inspect '%s' manually. You should see a horizontal rainbow running\
%! from left to right.", exp_size/1e3, s.size/1e3, rel_size_err_thres * 1e2, fn);
%! endif
%!endfunction

%!test
%! # raw/uncompressed video
%! fn = [tempname() "_rainbow_raw.avi"];
%! # median(rel_err) = 0% on Debian GNU/Linux 12 on AMD Ryzen 7 5700X
%! encode_decode (fn, "RGBA", 0.5, 14408600); # allow 0.5% median error

%!test
%! # mp4 + mp4v
%! fn = [tempname() "_rainbow_mp4v.mp4"];
%! # median(rel_err) = 0.70% on Debian GNU/Linux 12 on AMD Ryzen 7 5700X
%! encode_decode (fn, "mp4v", 15.0, 69554); # allow 15.0% median error

%!test
%! # mkv + VP9
%! fn = [tempname() "_rainbow_vp9.mkv"];
%! # median(rel_err) = 0.63% on Debian GNU/Linux 12 on AMD Ryzen 7 5700X
%! encode_decode (fn, "VP90", 15.0, 15822); # allow 15.0% median error

%!test
%! # mp4 + default codec (= avc1)
%! fn = [tempname() "_rainbow_h264.mp4"];
%! # median(rel_err) = 0.71% on Debian GNU/Linux 12 on AMD Ryzen 7 5700X
%! # but 14.8%, reported here: https://savannah.gnu.org/bugs/?func=detailitem&item_id=64452
%! encode_decode (fn, "", 15.0, 12157);

%!test
%! # mkv + default codec (= H264 on Debian GNU/Linux 12)
%! fn = [tempname() "_rainbow_default.mkv"];
%! # median(rel_err) = 0.71% on Debian GNU/Linux 12 on AMD Ryzen 7 5700X
%! encode_decode (fn, [], 15.0, 12143); # allow 15.0% median error

%!test <*64383>
%! fn = fullfile (tempdir (), "comma_not_dot,mp4");
%! w = VideoWriter (fn);
%! open (w);
%! fail ("writeVideo (w, rand(100,100,3))", "failed");
