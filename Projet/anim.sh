#!/usr/bin/env sh

FRAME_COUNT=$(($1-1))

cd Project/build/
make
for i in $(seq 0 $FRAME_COUNT)
do
	echo $i
	./myFem ../data/mesh.txt ../data/anim/frame$i.txt
done

cd ../../ProjectPostProcessor
python3 plot.py

ffmpeg -framerate 30 -i anim/frame%d.png -c:v libx264 -r $FRAME_COUNT -pix_fmt yuv420p output.mp4 -y