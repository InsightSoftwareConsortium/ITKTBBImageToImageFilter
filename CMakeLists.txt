project(TBBImageToImageFilter)

file(READ README.md DOCUMENTATION)

if(NOT ITK_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.9.5 FATAL_ERROR)
    find_package(ITK REQUIRED)
    list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
    include(itk-module-init.cmake)
endif()

if(ITK_USE_TBB)
  set(${PROJECT_NAME}_LIBRARIES tbb)

  # When this module is loaded by an app, load TBB too.
    set(${PROJECT_NAME}_EXPORT_CODE_INSTALL "
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
    set(TBB_INCLUDE_DIR \"${TBB_INCLUDE_DIR}\")
    set(TBB_LIBRARY_RELEASE \"${TBB_LIBRARY_RELEASE}\")
    set(TBB_LIBRARY_DEBUG \"${TBB_LIBRARY_DEBUG}\")
    find_package(TBB REQUIRED)
    add_definitions(-DITK_USE_TBB)
    ")

    set(${PROJECT_NAME}_EXPORT_CODE_BUILD "
    if(NOT ITK_BINARY_DIR)
        list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
        set(TBB_INCLUDE_DIR \"${TBB_INCLUDE_DIR}\")
        set(TBB_LIBRARY_RELEASE \"${TBB_LIBRARY_RELEASE}\")
        set(TBB_LIBRARY_DEBUG \"${TBB_LIBRARY_DEBUG}\")
        find_package(TBB REQUIRED)
        add_definitions(-DITK_USE_TBB)
    endif()
    ")

    set(${PROJECT_NAME}_SYSTEM_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
    # TODO ${TBB_LIBRARIES} contains the path to the libraries, not the libraries name. (bug in FindTBB.cmake?)
    set(${PROJECT_NAME}_SYSTEM_LIBRARY_DIRS ${TBB_LIBRARIES})
else()
  # TODO this should not be needed.
  # But if it is not done, CMake continues to link against libtbb and the flag -DITK_USE_TBB is still defined
  # After a previous build using TBB.
  unset(${PROJECT_NAME}_LIBRARIES)
  unset(${PROJECT_NAME}_SYSTEM_INCLUDE_DIRS)
  unset(${PROJECT_NAME}_SYSTEM_LIBRARY_DIRS)
  remove_definitions(-DITK_USE_TBB)
endif()

if(NOT ITK_SOURCE_DIR)
    include(ITKModuleExternal)
else()
    itk_module_impl()
endif()
