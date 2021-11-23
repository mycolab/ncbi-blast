# $Id: CMakeLists.txt 621927 2020-12-17 17:39:26Z ivanov $

NCBI_requires(ODBC)
NCBI_add_library(ncbi_xdbapi_odbc)
NCBI_add_subdirectory(samples)

