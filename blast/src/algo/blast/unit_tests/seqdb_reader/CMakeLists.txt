# $Id: CMakeLists.txt 621932 2020-12-17 17:40:31Z ivanov $

NCBI_requires(Boost.Test.Included)
NCBI_add_app(seqdb_unit_test seqdb_lmdb_unit_test)

