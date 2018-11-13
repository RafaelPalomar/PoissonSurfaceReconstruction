#!/bin/bash


for file in ${FILES}
do
    for depth in ${DEPTHS}
    do
	base=$(basename ${file})
	extension="${base##*.}"
	filename="${base%.*}"

	echo -n "Processing $(base)......"
	/build/bin/PoissonReconstruction /inputdata/$(basename ${file}) ${depth} /outputdata/${filename}_poisson.vtk || { echo "FAILED"; }
	echo "OK"	
    done
done
