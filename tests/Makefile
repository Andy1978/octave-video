.PHONY: clean realclean

# Use ccache to speed up consecutive builds
CXX = ccache g++

run:
	make -C .. dist
	./build.sh
	./run.sh | tee docker_results.log

standalone: standalone.cc ../src/cap_ffmpeg_impl_ov.hpp
	mkoctfile -o $@ -I../src --link-stand-alone $< -lswscale -lavformat -lavcodec -lavutil

clean:
	rm -f standalone

realclean: clean
	rm -f docker_results.log
