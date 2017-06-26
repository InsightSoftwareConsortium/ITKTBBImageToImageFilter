project(TBBImageToImageFilter)

# assign includes to std module var -- empty if TBB_NOT_FOUND
set(${PROJECT_NAME}_SYSTEM_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
# Module standard library var -- empty if TBB_NOT_FOUND
set(${PROJECT_NAME}_LIBRARIES ${tbb_lib})

itk_module_impl()

