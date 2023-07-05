#!/bin/bash -e

# INFO: "test VideoWriter" writes and reads a video
OCMD='pkg load video; system ("head -n2 /etc/os-release"); printf ("OCTAVE_VERSION = %s\n", OCTAVE_VERSION); x=VideoWriter("foo.avi"); printf ("FFmpeg_versions = %s\n", x.FFmpeg_versions); test VideoWriter; printf ("---\n");'
echo "octave command: ${OCMD}" | tee run.log

docker run -it debian_buster octave -q --eval "$OCMD" | tee -a run.log

docker run -it debian_bullseye octave -q --eval "$OCMD"

docker run -it debian_bookworm octave -q --eval "$OCMD"

docker run -it fedora31_ffmpeg octave -q --eval "$OCMD"

docker run -it fedora33_ffmpeg octave -q --eval "$OCMD"

docker run -it fedora38_ffmpeg octave -q --eval "$OCMD"

docker run -it ubuntu_20_04 octave -q --eval "$OCMD"

docker run -it ubuntu_22_04 octave -q --eval "$OCMD"

docker run -it ubuntu_23_04 octave -q --eval "$OCMD"
