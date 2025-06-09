#! /bin/sh

cmake -S . -B build/
cd build && make &&
./lights_monitor
