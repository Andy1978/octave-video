clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

addpath ("../inst")

width = 300;
height = 200;
nframes = 60;

p = permute (rainbow (width), [3 1 2]);

raw_video = zeros (height, width, 3, nframes);

## create video

h = __writer_open__ ("rainbow.mp4", "", 30, width, height, true);

im = [];
for k=1:nframes

  ps = circshift (p, k * 6);
  img = uint8 (255 * repmat (ps, height, 1));

  raw_video (:, :, :, k) = img;

  # default ist BGR24, flip RGB -> BGR
  img = flip (img, 3);
   
  __writer_write_frame__ (h, img);

endfor

__writer_close__ (h);

## read video and compare

clear -x raw_video

h = __cap_open__ ("rainbow.mp4");

%~ im = [];
for k=1:size (raw_video, 4)
  __cap_grab_frame__ (h);
  img = __cap_retrieve_frame__ (h);

  # default ist BGR24, flip
  img = flip (img, 3);

  # difference
  d = double (img) - raw_video(:,:,:,k);

  # this doesn't work well due to compression....
  rel_err = sum (abs(d(:)))/numel(d)/255;
  assert (rel_err < 0.001)
  
  #printf ("min (d) = %i, max (d) = %i, rel.err = %.2f%%\n", min (d(:)), max (d(:)), rel_err*1e2);

  %~ if (isempty (im))
    %~ im = image (img);
  %~ else
    %~ set (im, "cdata", img);
  %~ endif
  %~ drawnow
  %~ pause (0.05)
endfor

