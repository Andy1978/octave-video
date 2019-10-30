#!/bin/bash -e

docker run -it debian_stretch_octave_4_4_1 octave --eval "pkg load video; test VideoWriter; test VideoReader"

docker run -it debian_stretch_octave_5_1_0 octave --eval "pkg load video; test VideoWriter; test VideoReader"

docker run -it centos7_epel_ffmpeg octave --eval "pkg load video; test VideoWriter; test VideoReader"

docker run -it fedora31_ffmpeg octave --eval "pkg load video; test VideoWriter; test VideoReader"
