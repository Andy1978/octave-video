clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

addpath ("../inst")

w = VideoWriter ("foobar.mp4");
#w = VideoWriter ("foobar.mkv", "VP90");
open (w);

#for k=1:10
#  x = uint8 (randi (255, 50, 100, 3));
#  writeVideo (w, x)
#endfor

#close (w);

