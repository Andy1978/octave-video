#!/bin/bash -e

cp ../video-2*.tar.gz .

## Debian bullseye
docker build -f Dockerfile_debian_bullseye -t debian_bullseye .

## Debian bookworm
docker build -f Dockerfile_debian_bookworm -t debian_bookworm .

## Debian trixie
docker build -f Dockerfile_debian_trixie -t debian_trixie .

## Fedora 38
docker build -f Dockerfile_fedora38_ffmpeg -t fedora38_ffmpeg .

## Fedora 44
docker build -f Dockerfile_fedora44_ffmpeg -t fedora44_ffmpeg .

## Ubuntu 22.04
docker build -f Dockerfile_ubuntu_22_04 -t ubuntu_22_04 .

## Ubuntu 24.04
docker build -f Dockerfile_ubuntu_24_04 -t ubuntu_24_04 .

## Ubuntu 26.04
docker build -f Dockerfile_ubuntu_26_04 -t ubuntu_26_04 .

rm -f video*.tar.gz
