# $Id: CMakeLists.txt 621584 2020-12-14 19:14:26Z ivanov $

NCBI_project_tags(web)
NCBI_add_library(cgi fcgi)
NCBI_add_subdirectory(test)
#NCBI_add_subdirectory(demo)

