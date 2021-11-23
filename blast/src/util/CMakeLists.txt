# $Id: CMakeLists.txt 631498 2021-05-19 13:46:25Z ivanov $

NCBI_add_library(util)
NCBI_add_subdirectory(
    bitset 
    regexp 
    xregexp 
    compress 
    diff 
    image 
    tables 
    sequtil 
    qparse 
    lmdb 
    lmdbxx 
    test 
    demo
    profile
)

