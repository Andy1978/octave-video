#!/bin/bash -e

cp ../video-2*.tar.gz .

## Debian buster
docker build -f Dockerfile_debian_buster -t debian_buster .

## Debian bullseye
docker build -f Dockerfile_debian_bullseye -t debian_bullseye .

## Debian bookworm
docker build -f Dockerfile_debian_bookworm -t debian_bookworm .

## Fedora 31
docker build -f Dockerfile_fedora31_ffmpeg -t fedora31_ffmpeg .

## Fedora 33
docker build -f Dockerfile_fedora33_ffmpeg -t fedora33_ffmpeg .

## Fedora 38
docker build -f Dockerfile_fedora38_ffmpeg -t fedora38_ffmpeg .

## Ubuntu 20.04
docker build -f Dockerfile_ubuntu_20_04 -t ubuntu_20_04 .

## Ubuntu 22.04
docker build -f Dockerfile_ubuntu_22_04 -t ubuntu_22_04 .

## Ubuntu 23.04
docker build -f Dockerfile_ubuntu_23_04 -t ubuntu_23_04 .

rm -f video*.tar.gz
