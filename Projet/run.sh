#!/usr/bin/env sh

if [ $1 -z ]; then
	MESH_PATH=../data/mesh.txt
else
	MESH_NAME=$1
fi
if [ $2 -z ]; then
	PROBLEM_PATH=../data/problem.txt
else
	PROBLEM_PATH=$2
fi

cd ProjectPreProcessor/build/
make
./myFem `echo $MESH_PATH | sed 's/\.txt//'`

cd ../../Project/build/
make
./myFem $MESH_PATH $PROBLEM_PATH

cd ../../ProjectPostProcessor/build/
make
./myFem $MESH_PATH $PROBLEM_PATH `echo $PROBLEM_PATH | sed 's/\.txt/UV.txt/'`