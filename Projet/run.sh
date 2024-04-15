#!/usr/bin/env sh

cd ProjectPreProcessor/build/
make
./myFem

cd ../../Project/build/
make
./myFem

cd ../../ProjectPostProcessor/build/
make
./myFem