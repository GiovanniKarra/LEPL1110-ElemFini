#!/usr/bin/env sh

if [ $# -eq 0 ]; then
	MESH_NAME=mesh
else
	MESH_NAME=$1
fi

cd ProjectPreProcessor/build/
make
./myFem $MESH_NAME

cd ../../Project/build/
make
./myFem $MESH_NAME

cd ../../ProjectPostProcessor/build/
make
./myFem $MESH_NAME