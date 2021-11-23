# $Id: CMakeLists.txt 621583 2020-12-14 19:14:21Z ivanov $

NCBI_project_tags(core)
NCBI_add_library(corelib test_mt test_boost)
NCBI_add_subdirectory(test)
