/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkTBBImageToImageFilter.h"

#include <itkStaticAssert.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkTestingMacros.h>

namespace itk {

template< typename TInputImage, typename TOutputImage >
class TBBImageToImageFilterHelper : public TBBImageToImageFilter< TInputImage, TOutputImage >
{
public:
  // Standard class typedefs.
  typedef TBBImageToImageFilterHelper                         Self;
  typedef TBBImageToImageFilter< TInputImage, TOutputImage >  Superclass;
  typedef SmartPointer< Self >                                Pointer;
  typedef SmartPointer< const Self >                          ConstPointer;

  // Superclass typedefs.
  typedef typename Superclass::OutputImageRegionType          OutputImageRegionType;
  typedef typename Superclass::OutputImagePixelType           OutputImagePixelType;

  // Run-time type information (and related methods).
  itkTypeMacro(TBBTestFilter, TBBImageToImageFilter);

  // Method for creation through the object factory
  itkNewMacro(Self);

protected:
  void TBBGenerateData(const OutputImageRegionType& outputRegionForThread) override
  {
    // Allocate output
    typename TInputImage::ConstPointer input  = this->GetInput();
    typename TOutputImage::Pointer output = this->GetOutput();

    ImageRegionConstIterator<TInputImage> iit(input, outputRegionForThread);
    ImageRegionIterator<TOutputImage> oit(output, outputRegionForThread);
    while(!iit.IsAtEnd())
      {
      oit.Set(iit.Get() + 1);
      ++iit; ++oit;
      }
  }
public:

  // Helper to test that an exception is raised if ThreadedGenerateData is called
  void ThreadedGenerateDataHelper()
  {
    OutputImageRegionType outputRegionForThread;
    ThreadIdType threadId;
    Superclass::ThreadedGenerateData(outputRegionForThread, threadId);
  }
};

} // itk

int itkTBBImageToImageFilterTest( int, char* [] )
{
#ifdef ITK_USE_TBB
  std::cout << "Test TBBImageToImageFilter with TBB library" << std::endl;
#else
  std::cout << "Test TBBImageToImageFilter without TBB library" << std::endl;
#endif

  typedef itk::Image<short, 2>                                    ImageType;
  typedef itk::TBBImageToImageFilterHelper<ImageType, ImageType>  FIlterType;
  FIlterType::Pointer tbbTestFilter =                             FIlterType::New();

  ImageType::Pointer input =  ImageType::New();
  ImageType::Pointer output = ImageType::New();

  ImageType::SizeType size;
  size[0] = 4;
  size[1] = 4;
  input->SetRegions(size);
  input->Allocate(true);
  input->FillBuffer(0);

  tbbTestFilter->SetInput(input);
  TRY_EXPECT_EXCEPTION(tbbTestFilter->ThreadedGenerateDataHelper());
  TRY_EXPECT_NO_EXCEPTION(tbbTestFilter->Update());

  output = tbbTestFilter->GetOutput();
  itk::ImageRegionConstIterator<ImageType> it(output, output->GetLargestPossibleRegion());
  int testValue = EXIT_SUCCESS;
  while(!it.IsAtEnd()) {
    TEST_EXPECT_EQUAL_STATUS_VALUE(it.Get(), 1, testValue);
    ++it;
    }

  return testValue;
}
