itk_module_test()

set(TBBImageToImageFilterTests
  itkTBBImageToImageFilterTest.cxx
  )

CreateTestDriver(TBBImageToImageFilter "${TBBImageToImageFilter-Test_LIBRARIES}" "${TBBImageToImageFilterTests}")

itk_add_test(NAME itkTBBImageToImageFilterTest
  COMMAND TBBImageToImageFilterTestDriver
)