#!/bin/bash -ex
docker run \
    --rm \
    -it \
    --mount type=bind,source=`pwd`,target=`pwd` \
    --mount type=volume,source=gcc-conan,target=/root/.conan \
    -w `pwd` \
    swear_paper  \
    "${@}"
