# $Id: CMakeLists.txt 621585 2020-12-14 19:14:32Z ivanov $

NCBI_project_tags(core)
NCBI_disable_pch()
NCBI_add_library(tables)
NCBI_add_subdirectory(test)

