# There is a little problem on windoze when using Matroska mkv containers
# (works as expected with AVI container)
#
# The video duration differs when created a video with 16 frames and 2fps:
# windoze: 7.5s
# debian : 8.0s
#
# since ic->streams[video_stream]->nb_frames returns 0 for this mkv
# (cap_ffmpeg_impl_ov.hpp line 1481 int64_t CvCapture_FFMPEG::get_total_frames())
# total_frames is calculated from the duration which causes a "off by one" error on windoze.
#
# Tested with windoze 10, GNU Octave Version: 10.1.0 (hg id: 417c47651ed5)
# FFmpeg_versions        = Lavu56.31.100, SwS5.5.100, Lavc58.54.100, Lavf58.29.100

fn = "noise_vp9.mkv";
tag = "VP90";
nframes = 16;
if (1)
   w = VideoWriter (fn, tag);
   w.FrameRate = 2;
   for k=1:nframes
     img = randi (255, 150, 200, 3, "uint8");
     writeVideo (w, img);
   endfor
   close (w)
endif

[s, out] = system (['ffprobe -v error -select_streams v:0 -count_packets -show_entries stream=nb_read_packets -of csv=p=0 ' fn]);
assert (str2double (out), nframes);

[s, out] = system (['ffprobe -i ' fn ' -print_format json -loglevel fatal -show_streams -count_frames -select_streams v']);
out = jsondecode (out);

assert (str2double (out.streams.nb_read_frames), nframes)

if (isfield (out.streams, "nb_frames"))
  out.streams.nb_frames
else
  printf ("nb_frames gibt es nicht...\n");
endif

#clear -x nframes fn
r = VideoReader (fn);
assert (r.NumberOfFrames, nframes);
