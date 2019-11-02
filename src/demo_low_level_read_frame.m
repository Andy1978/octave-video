clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_get_properties__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

## Open Video, read frames, show them

x = __cap_open__ ("../samples/vtest.avi")
opt = __cap_get_properties__ (x);

im = [];
for k=1:opt.total_frames
  __cap_grab_frame__ (x);
  img = __cap_retrieve_frame__ (x);

  # default ist BGR24, flip
  img = flip (img, 3);

  if (isempty (im))
    im = image (img);
  else
    set (im, "cdata", img);
  endif
  drawnow
endfor

opt = __cap_get_properties__ (x)
