#ifndef __itkTBBTestFilter_hxx
#define __itkTBBTestFilter_hxx

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

#endif // __itkTBBTestFilter_hxx
