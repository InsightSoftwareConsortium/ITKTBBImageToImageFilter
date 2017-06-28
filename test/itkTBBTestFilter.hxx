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

#ifndef itkTBBTestFilter_hxx
#define itkTBBTestFilter_hxx

#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

namespace itk {

template<typename TInputImage, typename TOutputImage>
void TBBTestFilter<TInputImage, TOutputImage>::TBBGenerateData(const OutputImageRegionType &outputRegionForThread)
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

} // itk

#endif // itkTBBTestFilter_hxx
