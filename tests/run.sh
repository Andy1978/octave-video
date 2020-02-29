#!/bin/bash -e

OCMD='pkg load video; system ("head -n2 /etc/os-release"); printf ("OCTAVE_VERSION = %s\n", OCTAVE_VERSION); x=VideoWriter("foo.avi"); printf ("FFmpeg_versions = %s\n", x.FFmpeg_versions); test VideoWriter; printf ("---\n");'

docker run -it debian_stretch_octave_4_4_1 octave -q --eval "$OCMD"

docker run -it debian_stretch_octave_5_1_0 octave -q --eval "$OCMD"

docker run -it centos7_epel_ffmpeg octave -q --eval "$OCMD"

docker run -it fedora31_ffmpeg octave -q --eval "$OCMD"

docker run -it debian_stretch_octave_4_4_1_libav_12.3 octave -q --eval "$OCMD"
