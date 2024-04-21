#!/usr/bin/env sh

CSV_FILE=perf.csv

echo "" > $CSV_FILE

SEQ=`seq 0.25 -0.005 0.01`

SIZE=""
for i in $SEQ
do
	SIZE=$SIZE,`echo $i | sed 's/,/./'`
done
echo $SIZE >> $CSV_FILE

TIME=""
for i in $SEQ
do
	python3 mesh.py `echo $i | sed 's/,/./'`
	cd ../ProjectPreProcessor/build
	./myFem mesh
	cd ../../Project/build
	
	CUR_TIME=`/usr/bin/time -f %e ./myFem ../data/mesh.txt ../data/problem.txt 2>&1`
	TIME=$TIME,$CUR_TIME

	echo $i

	cd ../../Scripts
done
echo $TIME >> $CSV_FILE