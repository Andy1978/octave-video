clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

addpath ("../inst")

x = zeros (200, 300, 3);
x (:, 120, 1) = 255;

[h, opt] = __writer_open__ ("./bar.mp4", "", 30, columns (x), rows (x), true);

opt
 
for k=1:60

  # default ist BGR24, thus this gives a blue line
  __writer_write_frame__ (h, x);

endfor

__writer_close__ (h);

