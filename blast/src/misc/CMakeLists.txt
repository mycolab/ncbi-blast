# $Id: CMakeLists.txt 621674 2020-12-15 14:42:44Z ivanov $

NCBI_add_subdirectory(
  clog 
  grid_cgi 
  xmlwrapp 
  eutils_client 
  hydra_client 
  discrepancy
  xmlreaders 
  hgvs 
  netstorage 
  jsonwrapp 
  biosample_util 
  data_loaders_util
  lapackwrapp 
  cgi_redirect 
  pmcidconv_client 
  fix_pub
  third_party 
  third_party_static
)

