/*=========================================================================
  Authors: David Doria at Rensselaer Polytechnic Institute and
  Arnaud Gelas at Harvard Medical School

  Contributors: Rafael Palomar at The Intervention Centre,
  Oslo University Hospital

  Copyright (c) 2010, David Doria at Rensselaer Polytechnic Institute and
  Arnaud Gelas at Harvard Medical School,
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
  Neither the name of the Rensselaer Polytechnic Institute and of Harvard
  Medical School nor the names of its contributors may be used to endorse
  or promote products derived from this software without specific prior
  written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =========================================================================*/

#include "vtkPoissonReconstruction.h"

//ITK includes
#include <itkPoint.h>
#include <itkVector.h>
#include <itkImage.h>
#include <itkNrrdImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkZeroCrossingImageFilter.h>
#include <itkDerivativeImageFilter.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkOrientImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkSobelOperator.h>
#include <itkNeighborhoodOperatorImageFilter.h>
#include <itkAnalyzeImageIO.h>

//VTK includes
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointData.h>
#include <vtkMaskPoints.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>


//STD includes
#include <iostream>

//-------------------------------------------------------------------------------
//Type definitions
typedef itk::Image<float ,3> ImageType;
typedef ImageType::Pointer ImagePointer;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ZeroCrossingImageFilter<ImageType, ImageType> ZeroCrossingFilterType;
typedef itk::DerivativeImageFilter<ImageType, ImageType> DerivativeFilterType;
typedef itk::OrientImageFilter<ImageType, ImageType> OrientFilterType;
typedef itk::SobelOperator<float, 3> SobelOperatorType;
typedef itk::NeighborhoodOperatorImageFilter<ImageType, ImageType> NeighborhoodOperatorImageFilterType;

//-------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if ( argc < 4 )
    {
      cout << "PoissonReconstruction takes 3 arguments: " << endl;
      cout << "1-Input file (*.nrrd)" << endl;
      cout << "2-Depth" << endl;
      cout << "3-Output file (*.vtp)" << endl;
      return EXIT_FAILURE;
    }

  std::string inputFileName = argv[1];
  std::string outputFileName = argv[3];
  int            depth = atoi(argv[2]);

  //Read image
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputFileName);
  reader->Update();
  ImageType::Pointer segmentationImage = reader->GetOutput();

  ImageType::Pointer orientedSegmentationImage = reader->GetOutput();


  //-------------------------------------------------------------------------------
  // COMPUTE THE CLOUD OF POINTS

  //Create the sobel operator (x direction)
  SobelOperatorType xSobelOperator;
  itk::Size<3> radius;
  radius.Fill(1);
  xSobelOperator.SetDirection(0);
  xSobelOperator.CreateToRadius(radius);

  //Apply sobel operator (x direction)
  NeighborhoodOperatorImageFilterType::Pointer xFilter = NeighborhoodOperatorImageFilterType::New();
  xFilter->SetOperator(xSobelOperator);
  xFilter->SetInput(orientedSegmentationImage);
  xFilter->Update();
  ImageType::Pointer xDerivativeImage = xFilter->GetOutput();

  //Create the sobel operator (y direction)
  SobelOperatorType ySobelOperator;
  ySobelOperator.SetDirection(1);
  ySobelOperator.CreateToRadius(radius);

  //Apply sobel operator (y direction)
  NeighborhoodOperatorImageFilterType::Pointer yFilter = NeighborhoodOperatorImageFilterType::New();
  yFilter->SetOperator(ySobelOperator);
  yFilter->SetInput(orientedSegmentationImage);
  yFilter->Update();
  ImageType::Pointer yDerivativeImage = yFilter->GetOutput();

  //Create the sobel operator (x direction)
  SobelOperatorType zSobelOperator;
  zSobelOperator.SetDirection(2);
  zSobelOperator.CreateToRadius(radius);

  //Apply sobel operator (x direction)
  NeighborhoodOperatorImageFilterType::Pointer zFilter = NeighborhoodOperatorImageFilterType::New();
  zFilter->SetOperator(zSobelOperator);
  zFilter->SetInput(orientedSegmentationImage);
  zFilter->Update();
  ImageType::Pointer zDerivativeImage = zFilter->GetOutput();


  //Create magnitude image
  typedef itk::ImageDuplicator< ImageType > DuplicatorType;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(orientedSegmentationImage);
  duplicator->Update();
  ImageType::Pointer magnitudeImage = duplicator->GetOutput();


  itk::ImageRegionConstIteratorWithIndex<ImageType> xIt(xDerivativeImage,
                                                        xDerivativeImage->GetLargestPossibleRegion());
  itk::ImageRegionConstIteratorWithIndex<ImageType> yIt(yDerivativeImage,
                                                        yDerivativeImage->GetLargestPossibleRegion());
  itk::ImageRegionConstIteratorWithIndex<ImageType> zIt(zDerivativeImage,
                                                        zDerivativeImage->GetLargestPossibleRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> magnitudeIt(magnitudeImage,
                                                           magnitudeImage->GetLargestPossibleRegion());
  itk::ImageRegionConstIteratorWithIndex<ImageType> orientedSegmentationIt(orientedSegmentationImage,
                                                                           orientedSegmentationImage->GetLargestPossibleRegion());
  itk::ImageRegionConstIteratorWithIndex<ImageType> segmentationIt(segmentationImage,
                                                                   segmentationImage->GetLargestPossibleRegion());

  ImageType::IndexType index;
  ImageType::PointType point;

  vtkSmartPointer<vtkPolyData> cloudOfPoints =
    vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkFloatArray> pointGradient =
    vtkSmartPointer<vtkFloatArray>::New();
  pointGradient->SetNumberOfComponents(3);
  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();

  //Compute the gradient image (only interior of liver)
  magnitudeIt.GoToBegin();
  orientedSegmentationIt.GoToBegin();
  xIt.GoToBegin();
  yIt.GoToBegin();
  zIt.GoToBegin();
  while(!magnitudeIt.IsAtEnd())
    {

      double gradientVector[3];
      gradientVector[0] = xIt.Get();
      gradientVector[1] = yIt.Get();
      gradientVector[2] = zIt.Get();

      if(orientedSegmentationIt.Get() != 0){
        magnitudeIt.Set(sqrt(gradientVector[0]*gradientVector[0]+
                             gradientVector[1]*gradientVector[1]+
                             gradientVector[2]*gradientVector[2]));

        if(magnitudeIt.Get()>0){
          index = segmentationIt.GetIndex();
          segmentationImage->TransformIndexToPhysicalPoint(index, point);

          vtkIdType id = points->InsertNextPoint(point[0],point[1],point[2]);
          pointGradient->InsertNextTuple(gradientVector);
        }
      }
      else
        magnitudeIt.Set(0);

      ++magnitudeIt;
      ++orientedSegmentationIt;
      ++segmentationIt;
      ++xIt;
      ++yIt;
      ++zIt;
    }


  cloudOfPoints->SetPoints(points);
  pointGradient->SetName("Normals");
  cloudOfPoints->GetPointData()->SetNormals(pointGradient);

  vtkSmartPointer<vtkTransform> transform =
    vtkSmartPointer<vtkTransform>::New();
  transform->Scale(-1,-1,1);

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputData(cloudOfPoints);
  transformFilter->SetTransform(transform);

  //-------------------------------------------------------------------------------
  // COMPUTE THE POISSON SURFACE RECONSTRUCTION

  vtkSmartPointer< vtkPoissonReconstruction > poissonFilter =
    vtkSmartPointer< vtkPoissonReconstruction >::New();
  poissonFilter->SetDepth(depth);
  poissonFilter->SetInputConnection( transformFilter->GetOutputPort() );
  poissonFilter->Update();

  //-------------------------------------------------------------------------------
  // WRITE THE RESULTS

  vtkSmartPointer< vtkXMLPolyDataWriter > writer =
    vtkSmartPointer< vtkXMLPolyDataWriter >::New();
  writer->SetInputConnection( poissonFilter->GetOutputPort() );
  writer->SetFileName( outputFileName.c_str() );
  writer->Update();

  return EXIT_SUCCESS;
}
