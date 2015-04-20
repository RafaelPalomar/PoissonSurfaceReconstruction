add_executable(PoissonReconstruction Example.cxx )
target_link_libraries( PoissonReconstruction vtkPoissonReconstruction ${ITK_LIBRARIES})

add_test( PoissonReconstructionTest
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/PoissonReconstruction
  ${PoissonReconstruction_SOURCE_DIR}/data/horsePoints.vtp
  5
  temp.vtp
)
