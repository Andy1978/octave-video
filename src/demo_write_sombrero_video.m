clear all;

autoload ("__cap_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__cap_grab_frame__", which ("cap_ffmpeg_wrapper.oct"))
autoload ("__cap_retrieve_frame__", which ("cap_ffmpeg_wrapper.oct"));

autoload ("__writer_open__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_close__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__writer_write_frame__", which ("cap_ffmpeg_wrapper.oct"));

addpath ("../inst")

w = VideoWriter ("sombrero.mp4");
open (w);

z = sombrero ();
hs = surf (z);
axis manual
nframes = 100;

for ii = 1:nframes
  set (hs, "zdata", z * sin (2*pi*ii/nframes));
  drawnow

  x = getframe (gcf).cdata;
  
  writeVideo (w, uint8(x));

endfor

close (w) 
