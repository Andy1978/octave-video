autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

x = __cap_open__ ("vtest.avi");
#x = __cap_open__ ("dge.mp4");

im = [];
for k=1:500
  __cap_grab_frame__ (x);
  img = __cap_retrieve_frame__ (x);
  if (isempty (im))
    im = image (img);
  else
    set (im, "cdata", img);
  endif
  drawnow
endfor
