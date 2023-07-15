.PHONY: clean check

# Use ccache to speed up consecutive builds
CXX = ccache g++

standalone: standalone.cc ../../src/cap_ffmpeg_impl_ov.hpp
	mkoctfile -Wall -Wextra -o $@ -I../../src --link-stand-alone $< -lswscale -lavformat -lavcodec -lavutil

check:
	cppcheck -q --enable=all --force --language=c++ standalone.cc cap_ffmpeg_impl_ov.hpp ffmpeg_codecs.hpp


clean:
	rm -f standalone
