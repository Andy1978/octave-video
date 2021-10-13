#!/bin/bash -e

cp ../video-2*.tar.gz .

## Debian stretch
docker build -f Dockerfile_debian_stretch_octave_4_4_1 -t debian_stretch_octave_4_4_1 .
docker build -f Dockerfile_debian_stretch_octave_5_1_0 -t debian_stretch_octave_5_1_0 .

# Debian stretch, libav 12.3
docker build -f Dockerfile_debian_stretch_octave_4_4_1_libav_12.3 -t debian_stretch_octave_4_4_1_libav_12.3 .

## CentOS 7
docker build -f Dockerfile_centos7_epel_ffmpeg -t centos7_epel_ffmpeg .

## Fedora 31
docker build -f Dockerfile_fedora31_ffmpeg -t fedora31_ffmpeg .

## Ubuntu 20.04
docker build -f Dockerfile_ubuntu_20_04 -t ubuntu_20_04 .

rm -f video*.tar.gz
