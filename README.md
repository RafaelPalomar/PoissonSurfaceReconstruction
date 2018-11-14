This is a dockerized version of the Poisson Surface Reconstruction by Michael Kazhdam and Matthew Bolitho adapted to VTK by David Doria. It includes an example application which converts segmentation data (labelmaps in .nrrd format) to 3D reconstructions.

The code in this repository can be used in three different ways:
* As a library: for what you are required to build the project (requires CMake and VTK)
* As an executable application which can convert medical segmentations (in .nrrd) to 3D reconstructions. You need to build the above library and the example (requires ITK)
* As a docker container that converts one or more .nrrd files to 3D reconstructions .vtk files

# Running the docker image
1. Build the docker image
```bash
cd PoissonSurfaceReconstruction
docker build -t PoissonSurfaceReconstruction .
```

2. Run the docker image
```bash
docker run -e FILES="$(ls /tmp/dataset/*.nrrd)" -e DEPTHS="5 6 7" -v /tmp/dataset:/inputdata -v /tmp:/outputdata PoissonSurfaceReconstruction
```

The docker container is prepared to receive a list of files (space separated) in the environment variable `FILES` (in the example above, the list of files is produced by the command `ls` in the directory `/tmp/dataset` (it is important that files other than .nrrd images are not passed to the container as they will produce unexpected results). A the depth reconstruction parameter can be specified by using the `DEPTH` variable, which accepts a list (space separated) set of values  (in the example above the list of depths is passed by `DEPTHS="5 6 7"`; for every depth parameter, a new output model will be produced for every input image. In addition, the container expects that the volume where the input data is located, is mounted in the container directory `/inputdata`; similarly, the output directory is expected to be mounted in the internal directory `/outputdata`.
