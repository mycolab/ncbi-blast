# $Id: CMakeLists.txt 621922 2020-12-17 17:38:33Z ivanov $

NCBI_project_tags(core)
NCBI_add_library(serial cserial)
NCBI_add_subdirectory(datatool test soap grpc_integration)


