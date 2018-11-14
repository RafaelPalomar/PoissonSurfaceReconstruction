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