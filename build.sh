#!/usr/bin/env bash 
PROJECT=$(grep -oP 'project\(\K[\w]+' CMakeLists.txt)
source cmake/build.sh

parse $@
ask
main

