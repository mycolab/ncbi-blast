# $Id: CMakeLists.txt 621931 2020-12-17 17:40:15Z ivanov $

NCBI_project_tags(test)
NCBI_requires(Boost.Test.Included)
NCBI_add_app(blastdb_format_unit_test)

