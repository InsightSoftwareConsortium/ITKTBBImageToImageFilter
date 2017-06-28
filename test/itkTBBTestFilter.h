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

#ifndef itkTBBTestFilter_h
#define itkTBBTestFilter_h

#include "itkTBBImageToImageFilter.h"

#include <itkMacro.h>

namespace itk {

template< typename TInputImage, typename TOutputImage >
class TBBTestFilter : public TBBImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Standard class typedefs.
    typedef TBBTestFilter                                       Self;
    typedef TBBImageToImageFilter< TInputImage, TOutputImage >  Superclass;
    typedef SmartPointer< Self >                                Pointer;
    typedef SmartPointer< const Self >                          ConstPointer;

    // Superclass typedefs.
    typedef typename Superclass::OutputImageRegionType	OutputImageRegionType;
    typedef typename Superclass::OutputImagePixelType	OutputImagePixelType;

    // Run-time type information (and related methods).
    itkTypeMacro(TBBTestFilter, TBBImageToImageFilter);

    // Method for creation through the object factory
    itkNewMacro(Self);

public:
    virtual void TBBGenerateData(const OutputImageRegionType& outputRegionForThread);
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTBBTestFilter.hxx"
#endif // ITK_MANUAL_INSTANTIATION

#endif // itkTBBTestFilter_h
