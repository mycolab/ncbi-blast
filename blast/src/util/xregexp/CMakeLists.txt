# $Id: CMakeLists.txt 621585 2020-12-14 19:14:32Z ivanov $

NCBI_project_tags(core)
NCBI_requires(PCRE)
NCBI_add_library(xregexp xregexp_template_tester)

