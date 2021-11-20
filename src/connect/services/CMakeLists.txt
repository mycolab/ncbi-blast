# $Id: CMakeLists.txt 621583 2020-12-14 19:14:21Z ivanov $

NCBI_project_tags(grid)
NCBI_add_library(xconnserv ncbi_xcache_netcache ncbi_xblobstorage_netcache)
NCBI_add_subdirectory(test)

