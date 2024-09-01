.PHONY: run clean style

#LIBS= -lavformat -lavcodec -lavutil

# bisher ausreichend für simple1
LIBS= -lavformat -lavcodec

run: simple1
	./simple1

simple1: simple1.c
	gcc -Wall -Wextra $^ $(LIBS) -o $@

ffmpeg:
	git clone https://git.ffmpeg.org/ffmpeg.git

style:
	astyle --style=gnu -s2 simple1.c

clean:
	rm -f simple1

realclean: clean
	rm -rf ffmpeg
