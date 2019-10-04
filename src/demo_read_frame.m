clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

addpath ("../inst")

## Open Video, read frames, show them

x = VideoReader ("../samples/vtest.avi");

im = [];
for k=1:500
  img = readFrame (x);

  if (isempty (im))
    im = image (img);
  else
    set (im, "cdata", img);
  endif
  drawnow
endfor

