#############################################################################
# $Id: CMakeLists.txt 632666 2021-06-04 07:10:51Z blastadm $
#############################################################################

cmake_minimum_required(VERSION 3.7)
if ("${PROJECT_NAME}" STREQUAL "")
    project(ncbi_cpp)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/build-system/cmake/CMakeLists.top_builddir.txt)
