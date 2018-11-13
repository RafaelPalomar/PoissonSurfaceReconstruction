FROM ubuntu:14.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install libinsighttoolkit3-dev \
    	    	    libvtk6-dev build-essential \
		    git \
		    cmake -y

RUN apt-get install qtbase5-dev -y
RUN apt-get install gcc-4.8 g++-4.8 libproj-dev -y
RUN apt-get install python2.7-dev -y

ADD https://api.github.com/repos/RafaelPalomar/PoissonSurfaceReconstruction/git/refs/heads/master version.json

RUN git clone  -b feature/dockerization https://github.com/RafaelPalomar/PoissonSurfaceReconstruction /src/poisson

WORKDIR /build

RUN cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.8 -DCMAKE_C_COMPILER=/usr/bin/gcc-4.8 ../src/poisson 

RUN make

RUN mkdir /scripts
WORKDIR /scripts
COPY scripts/run.sh /scripts
RUN chmod +x /scripts/run.sh
CMD /scripts/run.sh
