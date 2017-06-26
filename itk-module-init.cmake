# Provide qn option to Use TBB or not.

option(ITK_USE_TBB "Thread optimization via TBB" ON)
if(ITK_USE_TBB)
    # Use local FindTBB.cmake.
    list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/CMake")
    find_package(TBB REQUIRED)
    # add compile flag
    add_definitions(-DITK_USE_TBB)
    # Add link directory for the lib
    link_directories(${TBB_LIBRARIES})
    set(tbb_lib tbb)
endif()
