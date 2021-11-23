# $Id: CMakeLists.txt 621931 2020-12-17 17:40:15Z ivanov $

NCBI_add_library(ncbi_xreader ncbi_xloader_genbank)
NCBI_add_subdirectory(
  cache pubseq id2 id1 pubseq2 gicache test
)

