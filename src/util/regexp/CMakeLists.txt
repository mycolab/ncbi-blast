# $Id: CMakeLists.txt 621585 2020-12-14 19:14:32Z ivanov $

NCBI_disable_pch()
NCBI_add_library(regexp)
NCBI_add_app(test_pcre)

