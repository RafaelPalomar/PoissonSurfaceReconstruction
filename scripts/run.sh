#!/bin/bash


for file in ${FILES}
do
    for depth in ${DEPTHS}
    do
	extension="${file##*.}"
	filename="${file%.*}"

	echo -n "Processing $(basename ${file})......"
	/build/bin/PoissonReconstruction /inputdata/$(basename ${file}) ${depth} /outputdata/${filename}_poisson.vtk || { echo "FAILED"; }
	echo "OK"	
    done
done
