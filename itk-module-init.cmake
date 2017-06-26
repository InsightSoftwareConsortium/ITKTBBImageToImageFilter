# Provide qn option to Use TBB or not.

option(ITK_USE_TBB "Thread optimization via TBB" ON)
if(ITK_USE_TBB)
    # Use local FindTBB.cmake.
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
    find_package(TBB REQUIRED)
    # add compile flag
    add_definitions(-DITK_USE_TBB)
endif()
