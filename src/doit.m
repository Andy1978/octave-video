autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

x = __cap_open__ ("vtest.avi");

__cap_grab_frame__ (x);
img = __cap_retrieve_frame__ (x);

