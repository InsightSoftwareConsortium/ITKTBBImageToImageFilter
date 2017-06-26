project(TBBImageToImageFilter)

# assign includes to std module var
set(${PROJECT_NAME}_SYSTEM_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})

# Module standard library var
message("tbb_lib:  ${tbb_lib}")
set(${PROJECT_NAME}_LIBRARIES ${tbb_lib})

itk_module_impl()

