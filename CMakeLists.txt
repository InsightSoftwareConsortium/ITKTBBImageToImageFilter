if(NOT ITK_SOURCE_DIR)
    include(itk-module-init.cmake)
endif()

project(TBBImageToImageFilter)

# assign includes to std module var -- empty if TBB_NOT_FOUND
if(ITK_USE_TBB)
  set(${PROJECT_NAME}_LIBRARIES tbb)
  set(${PROJECT_NAME}_SYSTEM_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
  set(${PROJECT_NAME}_SYSTEM_LIBRARIES ${TBB_LIBRARIES})

  # When this module is loaded by an app, load TBB too.
    set(${PROJECT_NAME}_EXPORT_CODE_INSTALL "
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
    set(TBB_INCLUDE_DIR \"${TBB_INCLUDE_DIR}\")
    set(TBB_LIBRARY_RELEASE \"${TBB_LIBRARY_RELEASE}\")
    set(TBB_LIBRARY_DEBUG \"${TBB_LIBRARY_DEBUG}\")
    find_package(TBB REQUIRED)
    ")
    set(${PROJECT_NAME}_EXPORT_CODE_BUILD "
    if(NOT ITK_BINARY_DIR)
        list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
        set(TBB_INCLUDE_DIR \"${TBB_INCLUDE_DIR}\")
        set(TBB_LIBRARY_RELEASE \"${TBB_LIBRARY_RELEASE}\")
        set(TBB_LIBRARY_DEBUG \"${TBB_LIBRARY_DEBUG}\")
        add_definitions(-DITK_USE_TBB)
    endif()
    ")
endif()

if(NOT ITK_SOURCE_DIR)
    find_package(ITK REQUIRED)
    list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
    include(ITKModuleExternal)
else()
    itk_module_impl()
endif()


