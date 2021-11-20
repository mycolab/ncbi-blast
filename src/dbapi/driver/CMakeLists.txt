# $Id: CMakeLists.txt 621927 2020-12-17 17:39:26Z ivanov $

NCBI_add_library(dbapi_driver)
NCBI_add_subdirectory(
  util ctlib ftds95 ftds100 ftds-default mysql odbc samples
)

