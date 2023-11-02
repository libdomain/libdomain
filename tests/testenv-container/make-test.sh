#!/bin/bash -ex

BUILD_DIR=build

chown -R builder2:builder2 /app/
cd /app/ && rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR && pwd && cmake -DLIBDOMAIN_BUILD_TESTS:BOOL=ON -B . .. && make && ctest --output-on-failure
