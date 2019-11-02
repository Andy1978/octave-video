clear all;

autoload ("__ffmpeg_defines__", which ("cap_ffmpeg_wrapper.oct"));
autoload ("__ffmpeg_output_formats__", which ("cap_ffmpeg_wrapper.oct"));

__ffmpeg_defines__ ()
x = __ffmpeg_output_formats__ ()

x.long_name

# 68 ist mov
# 71 ist mp4
#x(71)
#x(13);

%~ for k=1:numel(x)

  %~ if (! isempty (x(k).codecs))
   %~ unknown = cellfun ("any", strfind(x(k).codecs, "?"));
   
   %~ if (any (unknown))
    %~ x(k)
   %~ endif
   
  %~ endif
%~ endfor
