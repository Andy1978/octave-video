.PHONY: clean

# Use ccache to speed up consecutive builds
CXX = ccache g++

standalone: standalone.cc ../../src/cap_ffmpeg_impl_ov.hpp
	mkoctfile -Wall -Wextra -o $@ -I../../src --link-stand-alone $< -lswscale -lavformat -lavcodec -lavutil

clean:
	rm -f standalone
