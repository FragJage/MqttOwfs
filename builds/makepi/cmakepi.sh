#!/bin/sh
cmake -DCMAKE_TOOLCHAIN_FILE=Toolchain-RaspberryPi.cmake ../..
make
