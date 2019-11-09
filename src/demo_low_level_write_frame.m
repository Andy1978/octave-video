clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_get_properties__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

addpath ("../inst")

x = zeros (201, 303, 3);
x (:, 120, 1) = 255;

h = __writer_open__ ("./bar.avi", "", 30, columns (x), rows (x), true);

opt = __writer_get_properties__ (h)

p = zeros (60, 1);
for k=1:60

  # default ist BGR24, thus this gives a blue line
  __writer_write_frame__ (h, x);

  opt = __writer_get_properties__ (h)

endfor


__writer_close__ (h);

