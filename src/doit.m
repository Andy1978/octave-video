clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

x = __cap_open__ ("../samples/vtest.avi");
#x = __cap_open__ ("dge.mp4");

if (0)
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
endif

__cap_grab_frame__ (x);
img = __cap_retrieve_frame__ (x);

##########

w = __writer_open__ ("foo.mp4", "");
#w = __writer_open__ ("foo.mkv");

#for k=1:100
  x = uint8 (randi (255, 50, 100, 3));
  __writer_write_frame__ (w, x);
  __writer_write_frame__ (w, x);
#endfor
__writer_close__ (w)

###########

addpath ("../inst")
w = VideoWriter ("foo.mp4")
