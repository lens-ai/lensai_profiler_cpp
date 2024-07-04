#!/bin/bash

# Name of the Docker image
IMAGE_NAME="build-datatracer"

# Build the Docker image
docker create --name datatracer_container -t $IMAGE_NAME
docker start datatracer_container
sleep 10
# Copy the compiled .so files from the container to the current working directory
docker cp datatracer_container:/home/user/datatracer/build/libimageprofiler.so ./
docker cp datatracer_container:/home/user/datatracer/build/libmodelprofiler.so ./
docker cp datatracer_container:/home/user/datatracer/build/libimagesampler.so ./

# Stop and remove the container
docker stop datatracer_container
docker rm datatracer_container

echo "Copied .so files to the current directory"

