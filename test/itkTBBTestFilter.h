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
