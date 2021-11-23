# $Id: CMakeLists.txt 621931 2020-12-17 17:40:15Z ivanov $

NCBI_requires(LMDB)
NCBI_add_library(writedb)
NCBI_add_subdirectory(unit_test)

