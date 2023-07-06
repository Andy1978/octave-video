#!/bin/bash -e

# INFO: "test VideoWriter" writes and reads a video
OCMD='pkg load video; system ("head -n2 /etc/os-release"); printf ("OCTAVE_VERSION = %s\n", OCTAVE_VERSION); x=VideoWriter("foo.avi"); printf ("FFmpeg_versions = %s\n", x.FFmpeg_versions); test VideoWriter;'
echo "octave command: ${OCMD}"
echo

do_run () {
  # Info: If you want to play in an interactive shell, just add --persist before --eval
  docker run -it "$1" octave -q --eval "$OCMD"
  echo "---"
}

do_run debian_buster
do_run debian_bullseye
do_run debian_bookworm
do_run fedora31_ffmpeg
do_run fedora33_ffmpeg
do_run fedora38_ffmpeg
do_run ubuntu_20_04
do_run ubuntu_22_04
do_run ubuntu_23_04
