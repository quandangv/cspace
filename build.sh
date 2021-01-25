#!/usr/bin/env bash 
PROJECT=$(grep -oP 'project\(\K[\w]+' CMakeLists.txt)

#git submodule update --init --recursive -- cmake

source cmake/utils.sh
parse_options $@
ask_options
build

