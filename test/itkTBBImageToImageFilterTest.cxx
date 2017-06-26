#include <iostream>

#include "itkTBBTestFilter.h"

#include <itkStaticAssert.h>
#include <itkImageRegionConstIterator.h>



int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;

#ifdef ITK_USE_TBB
    std::cout << "Test TBBImageToImageFilter with TBB library" << std::endl;
#else
    std::cout << "Test TBBImageToImageFilter without TBB library" << std::endl;
#endif

    typedef itk::Image<short, 2> ImageType;
    itk::TBBTestFilter<ImageType, ImageType>::Pointer tbbTestFilter = itk::TBBTestFilter<ImageType, ImageType>::New();

    ImageType::Pointer input = ImageType::New();
    ImageType::Pointer output = ImageType::New();

    // TODO get rid of the [-Wmissing-braces]
    ImageType::SizeType size = {4, 8};
    input->SetRegions(size);
    input->Allocate(true);
    input->FillBuffer(0);

    try
    {
        tbbTestFilter->SetInput(input);
        tbbTestFilter->Update();
        output = tbbTestFilter->GetOutput();

        itk::ImageRegionConstIterator<ImageType> it(output, output->GetLargestPossibleRegion());
        while(!it.IsAtEnd())
        {
            if(it.Get() != 1)
            {
                std::cerr << "TBBImageToImageFilterTest failed:" << std::endl
                          << "Value: " << it.Get() << " != 1 at: " << it.GetIndex() << std::endl;
                return EXIT_FAILURE;
            }
            ++it;
        }
    }
    catch (itk::ExceptionObject & exception)
    {
        std::cerr << "TBBImageToImageFilterTest failed:" << std::endl
                  << exception;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
