#!/bin/bash


for file in ${FILES}
do
    for depth in ${DEPTHS}
    do
	extension="${filename##*.}"
	filename="${filename%.*}"

	/build/bin/PoissonReconstruction /inputdata/${file} ${depth} /outputdata/${filename}_poisson.${extension}
	
    done
done
