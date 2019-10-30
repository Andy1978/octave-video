#!/bin/bash -e

cp ../video*.tar.gz .

docker build -f Dockerfile_debian_stretch_octave_4_4_1 -t debian_stretch_octave_4_4_1 .
docker build -f Dockerfile_debian_stretch_octave_5_1_0 -t debian_stretch_octave_5_1_0 .



rm -f video*.tar.gz
