set(DOCUMENTATION "This module contains a class to derivating ImageToimageFilter that aim to optimize thread managemet using the TBB library." )

itk_module(TBBImageToImageFilter
  DEPENDS
  ITKCommon
  TEST_DEPENDS
    ITKTestKernel
  EXCLUDE_FROM_DEFAULT
  DESCRIPTION
    "${DOCUMENTATION}"
)
