itk_module_test()

set(TEST_NAME itkTBBImageToImageFilterTest)

add_executable(${TEST_NAME}
    itkTBBImageToImageFilterTest.cxx
    # For IDE purpose
    itkTBBTestFilter.h
    itkTBBTestFilter.hxx
)

message(" ${itk-module-test}_LIBRARIES:  ${${itk-module-test}_LIBRARIES}")
target_link_libraries(${TEST_NAME}
    ${${itk-module-test}_LIBRARIES}
)

itk_add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
