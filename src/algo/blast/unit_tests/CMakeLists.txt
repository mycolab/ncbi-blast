# $Id: CMakeLists.txt 621932 2020-12-17 17:40:31Z ivanov $

NCBI_project_tags(test)
NCBI_set_test_resources(ServiceMapper)
NCBI_add_subdirectory(blast_format blastdb seqdb_reader api)

