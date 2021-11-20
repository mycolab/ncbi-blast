#############################################################################
# $Id: CMake.NCBIComponents.cmake 620492 2020-11-23 18:41:26Z gouriano $
#############################################################################

##
## NCBI CMake components description
##
##
## As a result, the following variables should be defined for component XXX
##  NCBI_COMPONENT_XXX_FOUND
##  NCBI_COMPONENT_XXX_INCLUDE
##  NCBI_COMPONENT_XXX_DEFINES
##  NCBI_COMPONENT_XXX_LIBS
##  HAVE_LIBXXX


#############################################################################
set(NCBI_ALL_COMPONENTS "")
set(NCBI_ALL_LEGACY "")
set(NCBI_ALL_REQUIRES "")

set(NCBI_REQUIRE_MT_FOUND YES)
list(APPEND NCBI_ALL_REQUIRES MT)
if(BUILD_SHARED_LIBS)
    set(NCBI_REQUIRE_DLL_BUILD_FOUND YES)
    list(APPEND NCBI_ALL_REQUIRES DLL_BUILD)
endif()

if(NOT "${NCBI_PTBCFG_PROJECT_COMPONENTS}" STREQUAL "")
    foreach(_comp IN LISTS NCBI_PTBCFG_PROJECT_COMPONENTS)
        if("${_comp}" STREQUAL "")
            continue()
        endif()
        string(SUBSTRING ${_comp} 0 1 _sign)
        if ("${_sign}" STREQUAL "-")
            string(SUBSTRING ${_comp} 1 -1 _comp)
            set(NCBI_COMPONENT_${_comp}_DISABLED YES)
        else()
            set(NCBI_COMPONENT_${_comp}_DISABLED NO)
        endif()
    endforeach()
endif()

#############################################################################
#############################################################################
# components definition functions (unix)

string(REPLACE ":" ";" NCBI_PKG_CONFIG_PATH  "$ENV{PKG_CONFIG_PATH}")
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH TRUE)
#message("NCBI_PKG_CONFIG_PATH init = ${NCBI_PKG_CONFIG_PATH}")

#############################################################################
function(NCBI_get_component_locations _sub _type)
    if(APPLE)
        if("${CMAKE_BUILD_TYPE}" STREQUAL "")
            set(_cmake_build_type Release)
            set(_ncbi_build_type  Release${NCBI_PlatformBits})
        else()
            set(_cmake_build_type ${CMAKE_BUILD_TYPE})
            set(_ncbi_build_type  ${CMAKE_BUILD_TYPE}${NCBI_PlatformBits})
        endif()
    else()
        set(_cmake_build_type ${CMAKE_BUILD_TYPE})
        set(_ncbi_build_type  ${NCBI_BUILD_TYPE})
    endif()
    set(_dirs   ${NCBI_COMPILER}${NCBI_COMPILER_VERSION}-${_ncbi_build_type}/${_type}
                ${NCBI_COMPILER}-${_ncbi_build_type}/${_type}
                ${_ncbi_build_type}/${_type}
                ${_cmake_build_type}${NCBI_PlatformBits}/${_type}
                ${_type}${NCBI_PlatformBits} ${_type})
    if(DEFINED NCBI_COMPILER_COMPONENTS)
        set(_components ${NCBI_COMPILER_COMPONENTS})
        list(REVERSE _components)
        foreach(_c IN LISTS _components)
            set(_dirs ${_c}-${_ncbi_build_type}/${_type} ${_dirs})
        endforeach()
    endif()
    set(${_sub} ${_dirs} PARENT_SCOPE)
endfunction()

#############################################################################
function(NCBI_find_package _name _package)
    if(NCBI_COMPONENT_${_name}_DISABLED)
        message("DISABLED ${_name}")
        return()
    endif()
# root
    set(_root "")
    if (DEFINED NCBI_ThirdParty_${_name})
        set(_root ${NCBI_ThirdParty_${_name}})
    else()
        string(FIND ${_name} "." dotfound)
        string(SUBSTRING ${_name} 0 ${dotfound} _dotname)
        if (DEFINED NCBI_ThirdParty_${_dotname})
            set(_root ${NCBI_ThirdParty_${_dotname}})
#        else()
#            message("NOT FOUND ${_name}: NCBI_ThirdParty_${_name} not found")
#            return()
        endif()
    endif()
    set(_roots ${_root})
    NCBI_get_component_locations( _subdirs lib)

    if("${_root}" STREQUAL "")
        if(NCBI_TRACE_COMPONENT_${_name})
            find_package(${_package})
        else()
            find_package(${_package} QUIET)
        endif()
        string(TOUPPER ${_package} _uppackage)
        if (${_package}_FOUND OR ${_uppackage}_FOUND)
            if( DEFINED ${_uppackage}_LIBRARIES OR DEFINED ${_uppackage}_INCLUDE_DIRS OR
                DEFINED ${_uppackage}_LIBRARY   OR DEFINED ${_uppackage}_INCLUDE_DIR)
                set(_package ${_uppackage})
            endif()
            if(DEFINED ${_package}_INCLUDE_DIRS)
                set(_pkg_include ${${_package}_INCLUDE_DIRS})
            elseif(DEFINED ${_package}_INCLUDE_DIR)
                set(_pkg_include ${${_package}_INCLUDE_DIR})
            else()
                set(_pkg_include "")
            endif()
            if(DEFINED ${_package}_LIBRARIES)
                set(_pkg_libs ${${_package}_LIBRARIES})
            elseif(DEFINED ${_package}_LIBRARY)
                set(_pkg_libs ${${_package}_LIBRARY})
            else()
                set(_pkg_libs "")
            endif()
            set(NCBI_COMPONENT_${_name}_INCLUDE ${_pkg_include} PARENT_SCOPE)
            set(NCBI_COMPONENT_${_name}_LIBS ${_pkg_libs} PARENT_SCOPE)
            if(DEFINED ${_package}_DEFINITIONS)
                set(NCBI_COMPONENT_${_name}_DEFINES ${${_package}_DEFINITIONS} PARENT_SCOPE)
                set(_pkg_defines ${${_package}_DEFINITIONS})
            endif()
            if(DEFINED ${_package}_VERSION_STRING)
                set(_pkg_version ${${_package}_VERSION_STRING})
            elseif(DEFINED ${_package}_VERSION)
                set(_pkg_version ${${_package}_VERSION})
            else()
                set(_pkg_version "")
            endif()
            set(_root ${_pkg_libs})
            set(_all_found YES)
        endif()
    else()
        set(_all_found NO)
        if(NCBI_TRACE_COMPONENT_${_name})
            message("NCBI_find_package: ${_name}: checking ${_root}: ${_subdirs}")
        endif()
        foreach(_root IN LISTS _roots)
            foreach(_libdir IN LISTS _subdirs)
                if(EXISTS ${_root}/${_libdir}/pkgconfig)
                    set(_pkgcfg ${NCBI_PKG_CONFIG_PATH})
                    if(NOT ${_root}/${_libdir}/pkgconfig IN_LIST _pkgcfg)
                        list(INSERT _pkgcfg 0 ${_root}/${_libdir}/pkgconfig)
                    endif()
                    set(CMAKE_PREFIX_PATH ${_pkgcfg})
                    string(REPLACE ";" ":" _config_path  "${_pkgcfg}")
                    set(ENV{PKG_CONFIG_PATH} "${_config_path}")
                    if(NCBI_TRACE_COMPONENT_${_name})
                        message("PKG_CONFIG_PATH = $ENV{PKG_CONFIG_PATH}")
                    endif()
                    unset(${_name}_FOUND CACHE)
                    if(DEFINED ${_name}_STATIC_LIBRARIES)
                        foreach(_lib IN LISTS ${_name}_STATIC_LIBRARIES)
                            if(NOT "${pkgcfg_lib_${_name}_${_lib}}" STREQUAL "")
                                unset(pkgcfg_lib_${_name}_${_lib} CACHE)
                            endif()
                        endforeach()
                    endif()
                    if(NCBI_TRACE_COMPONENT_${_name})
                        pkg_check_modules(${_name} ${_package})
                    else()
                        pkg_search_module(${_name} QUIET ${_package})
                    endif()

                    if(${_name}_FOUND)
                        if(NOT ${_root}/${_libdir}/pkgconfig IN_LIST NCBI_PKG_CONFIG_PATH)
                            list(INSERT NCBI_PKG_CONFIG_PATH 0 ${_root}/${_libdir}/pkgconfig)
                            set(NCBI_PKG_CONFIG_PATH ${NCBI_PKG_CONFIG_PATH} PARENT_SCOPE)
                        endif()
                        set(NCBI_COMPONENT_${_name}_INCLUDE ${${_name}_INCLUDE_DIRS} PARENT_SCOPE)
                        set(NCBI_COMPONENT_${_name}_LIBS ${${_name}_LINK_LIBRARIES} PARENT_SCOPE)
                        if(NCBI_TRACE_COMPONENT_${_name})
                            message("${_name}_LIBRARIES = ${${_name}_LIBRARIES}")
                            message("${_name}_CFLAGS = ${${_name}_CFLAGS}")
                            message("${_name}_CFLAGS_OTHER = ${${_name}_CFLAGS_OTHER}")
                            message("${_name}_LDFLAGS = ${${_name}_LDFLAGS}")
                        endif()
                        if(NOT "${${_name}_CFLAGS}" STREQUAL "")
                            set(_pkg_defines "")
                            foreach( _value IN LISTS ${_name}_CFLAGS)
                                string(FIND ${_value} "-D" _pos)
                                if(${_pos} EQUAL 0)
                                string(SUBSTRING ${_value} 2 -1 _pos)
                                    list(APPEND _pkg_defines ${_pos})
                                endif()
                            endforeach()
                            set(NCBI_COMPONENT_${_name}_DEFINES ${_pkg_defines} PARENT_SCOPE)
                        endif()
                        set(_pkg_include ${${_name}_INCLUDE_DIRS})
                        set(_pkg_libs ${${_name}_LINK_LIBRARIES})
                        set(_pkg_version ${${_name}_VERSION})
if(OFF AND NOT APPLE)
                        if(NOT BUILD_SHARED_LIBS)
                            set(_lib "")
                            set(_libs "")
                            foreach(_lib IN LISTS ${_name}_LINK_LIBRARIES)
                                string(REGEX REPLACE "[.]so$" ".a" _stlib ${_lib})
                                if(EXISTS ${_stlib})
                                    list(APPEND _libs ${_stlib})
                                else()
                                    set(_libs "")
                                    break()
                                endif()
                            endforeach()
                            if(NOT "${_libs}" STREQUAL "")
                                set(NCBI_COMPONENT_${_name}_LIBS ${_libs} PARENT_SCOPE)
                            endif()
                        endif()
endif()
                        set(_all_found YES)
                        break()
                    endif()
                endif()
            endforeach()
            if(_all_found)
                break()
            endif()
    endforeach()
    endif()

    if(_all_found)
        message(STATUS "Found ${_name}: ${_root} (version ${_pkg_version})")
        if(NCBI_TRACE_COMPONENT_${_name})
            message("${_name}: include dir = ${_pkg_include}")
            message("${_name}: libs = ${_pkg_libs}")
            if(DEFINED _pkg_defines)
                message("${_name}: defines = ${_pkg_defines}")
            endif()
        endif()
        set(NCBI_COMPONENT_${_name}_FOUND YES PARENT_SCOPE)

        string(TOUPPER ${_name} _upname)
        set(HAVE_LIB${_upname} 1 PARENT_SCOPE)
        string(REPLACE "." "_" _altname ${_upname})
        set(HAVE_${_altname} 1 PARENT_SCOPE)

        list(APPEND NCBI_ALL_COMPONENTS ${_name})
        set(NCBI_ALL_COMPONENTS ${NCBI_ALL_COMPONENTS} PARENT_SCOPE)
    else()
        set(NCBI_COMPONENT_${_name}_FOUND NO PARENT_SCOPE)
        message("NOT FOUND ${_name}: package not found")
    endif()
endfunction()

#############################################################################
function(NCBI_define_component _name)

    if(NCBI_COMPONENT_${_name}_DISABLED)
        message("DISABLED ${_name}")
        return()
    endif()
    if(APPLE)
        if(BUILD_SHARED_LIBS OR TRUE)
            set(_suffixes .dylib .a)
        else()
            set(_suffixes .a .dylib)
        endif()
    else()
        if(BUILD_SHARED_LIBS OR TRUE)
            set(_suffixes .so .a)
        else()
            set(_suffixes .a .so)
        endif()
    endif()

# root
    set(_root "")
    if (DEFINED NCBI_ThirdParty_${_name})
        set(_root ${NCBI_ThirdParty_${_name}})
    else()
        string(FIND ${_name} "." dotfound)
        string(SUBSTRING ${_name} 0 ${dotfound} _dotname)
        if (DEFINED NCBI_ThirdParty_${_dotname})
            set(_root ${NCBI_ThirdParty_${_dotname}})
        else()
            message("NOT FOUND ${_name}: NCBI_ThirdParty_${_name} not found")
            return()
        endif()
    endif()

    set(_args ${ARGN})
    set(_roots ${_root})

# include dir
    NCBI_get_component_locations(_subdirs include)
    if(NCBI_TRACE_COMPONENT_${_name})
        message("${_name}: checking ${_root}: ${_subdirs}")
    endif()
    set(_inc "")
    foreach(_libdir IN LISTS _subdirs)
        if (EXISTS ${_root}/${_libdir})
            set(_inc ${_root}/${_libdir})
            break()
        endif()
    endforeach()
    if("${_inc}" STREQUAL "")
        message("NOT FOUND ${_name}: ${_root}/include not found")
        return()
    endif()
    set(NCBI_COMPONENT_${_name}_INCLUDE ${_inc} PARENT_SCOPE)
    if(NCBI_TRACE_COMPONENT_${_name})
        message("${_name}: include dir = ${_inc}")
    endif()

# libraries
    NCBI_get_component_locations(_subdirs lib)
    if(NCBI_TRACE_COMPONENT_${_name})
        message("${_name}: checking ${_root}: ${_subdirs}")
    endif()
    if (BUILD_SHARED_LIBS AND DEFINED NCBI_ThirdParty_${_name}_SHLIB)
        set(_roots ${NCBI_ThirdParty_${_name}_SHLIB} ${_roots})
        set(_subdirs shlib64 shlib lib64 lib)
    endif()

    set(_all_found YES)
    set(_all_libs "")
    foreach(_root IN LISTS _roots)
        foreach(_libdir IN LISTS _subdirs)
            set(_all_found NO)
            set(_all_libs "")
            if (EXISTS ${_root}/${_libdir})
                set(_all_found YES)
                set(_all_libs "")
                foreach(_lib IN LISTS _args)
                    set(_this_found NO)
                    foreach(_sfx IN LISTS _suffixes)
                        if(EXISTS ${_root}/${_libdir}/lib${_lib}${_sfx})
                            list(APPEND _all_libs ${_root}/${_libdir}/lib${_lib}${_sfx})
                            set(_this_found YES)
                            if(NCBI_TRACE_COMPONENT_${_name})
                                message("${_name}: found:  ${_root}/${_libdir}/lib${_lib}${_sfx}")
                            endif()
                            break()
                        endif()
                    endforeach()
                    if(NOT _this_found)
                        if(NCBI_TRACE_COMPONENT_${_name})
                            message("${_name}: not found: ${_root}/${_libdir}/lib${_lib}${_sfx}")
                        endif()
                        set(_all_found NO)
                        break()
                    endif()
                endforeach()
            endif()
            if("${_args}" STREQUAL "")
                set(_all_found YES)
            endif()
            if(_all_found)
                break()
            endif()
        endforeach()
        if(_all_found)
            break()
        endif()
    endforeach()

    if(_all_found)
        message(STATUS "Found ${_name}: ${_root}")
        set(NCBI_COMPONENT_${_name}_FOUND YES PARENT_SCOPE)
#        set(NCBI_COMPONENT_${_name}_INCLUDE ${_root}/include)
        set(NCBI_COMPONENT_${_name}_LIBS ${_all_libs} PARENT_SCOPE)

        string(TOUPPER ${_name} _upname)
        set(HAVE_LIB${_upname} 1 PARENT_SCOPE)
        string(REPLACE "." "_" _altname ${_upname})
        set(HAVE_${_altname} 1 PARENT_SCOPE)

        list(APPEND NCBI_ALL_COMPONENTS ${_name})
        set(NCBI_ALL_COMPONENTS ${NCBI_ALL_COMPONENTS} PARENT_SCOPE)
    else()
        set(NCBI_COMPONENT_${_name}_FOUND NO PARENT_SCOPE)
        message("NOT FOUND ${_name}: some libraries not found at ${_root}")
    endif()

endfunction()

#############################################################################
macro(NCBI_find_library _name)
    if(NOT NCBI_COMPONENT_${_name}_DISABLED)
        set(_args ${ARGN})
        set(_all_libs "")
        set(_notfound_libs "")
        foreach(_lib IN LISTS _args)
            find_library(${_lib}_LIBS ${_lib})
            if (${_lib}_LIBS)
                list(APPEND _all_libs ${${_lib}_LIBS})
            else()
                list(APPEND _notfound_libs ${_lib})
            endif()
        endforeach()
        if("${_notfound_libs}" STREQUAL "")
            set(NCBI_COMPONENT_${_name}_FOUND YES)
            set(NCBI_COMPONENT_${_name}_LIBS ${_all_libs})
            list(APPEND NCBI_ALL_COMPONENTS ${_name})
            message(STATUS "Found ${_name}: ${NCBI_COMPONENT_${_name}_LIBS}")

            string(TOUPPER ${_name} _upname)
            set(HAVE_LIB${_upname} 1)
            set(HAVE_${_upname} 1)
        else()
            set(NCBI_COMPONENT_${_name}_FOUND NO)
            message("NOT FOUND ${_name}: some libraries not found: ${_notfound_libs}")
        endif()
    else()
        message("DISABLED ${_name}")
    endif()
endmacro()

#############################################################################
# Windows
function(NCBI_define_Wcomponent _name)

    if(NCBI_COMPONENT_${_name}_DISABLED)
        message("DISABLED ${_name}")
        return()
    endif()
# root
    if (DEFINED NCBI_ThirdParty_${_name})
        set(_root ${NCBI_ThirdParty_${_name}})
    else()
        string(FIND ${_name} "." dotfound)
        string(SUBSTRING ${_name} 0 ${dotfound} _dotname)
        if (DEFINED NCBI_ThirdParty_${_dotname})
            set(_root ${NCBI_ThirdParty_${_dotname}})
        else()
            message("NOT FOUND ${_name}: NCBI_ThirdParty_${_name} not found")
            return()
        endif()
    endif()
# include dir
    if (EXISTS ${_root}/include)
        set(_found YES)
    else()
        message("NOT FOUND ${_name}: ${_root}/include not found")
        set(_found NO)
    endif()
# libraries
    set(_args ${ARGN})
    if (_found)
        if(BUILD_SHARED_LIBS)
            set(_locations lib_dll lib_static lib)
        else()
            set(_locations lib_static lib_dll lib)
        endif()
        set(_rt ${NCBI_CONFIGURATION_RUNTIMELIB})

        foreach(_libdir IN LISTS _locations)
            set(_found YES)
            foreach(_cfg ${NCBI_CONFIGURATION_TYPES})
                if(NCBI_TRACE_COMPONENT_${_name})
                    message("${_name}: checking ${_root}/${_libdir}/${_cfg}${_rt}")
                endif()
                foreach(_lib IN LISTS _args)
                    if(NOT EXISTS ${_root}/${_libdir}/${_cfg}${_rt}/${_lib})
                        if(NCBI_TRACE_COMPONENT_${_name})
                            message("${_name}: ${_root}/${_libdir}/${_cfg}${_rt}/${_lib} not found")
                        endif()
                        set(_found NO)
                    endif()
                endforeach()
            endforeach()
            if (_found)
#                set(_libtype ${_libdir}/\$\(Configuration\))
                set(_libtype ${_libdir}/$<CONFIG>${_rt})
                break()
            endif()
        endforeach()

        if (NOT _found)
            set(_found YES)
            foreach(_cfg ${NCBI_CONFIGURATION_TYPES})
                if(NCBI_TRACE_COMPONENT_${_name})
                    message("${_name}: checking ${_root}/${_cfg}${_rt}")
                endif()
                foreach(_lib IN LISTS _args)
                    if(NOT EXISTS ${_root}/${_cfg}${_rt}/${_lib})
                        if(NCBI_TRACE_COMPONENT_${_name})
                            message("${_name}: ${_root}/${_cfg}${_rt}/${_lib} not found")
                        endif()
                        set(_found NO)
                    endif()
                endforeach()
            endforeach()
            if (_found)
#                set(_libtype \$\(Configuration\))
                set(_libtype $<CONFIG>${_rt})
            endif()
        endif()

        if (NOT _found)
            set(_locations lib libs)
            foreach(_libdir IN LISTS _locations)
                set(_found YES)
                if(NCBI_TRACE_COMPONENT_${_name})
                    message("${_name}: checking ${_root}/${_libdir}")
                endif()
                foreach(_lib IN LISTS _args)
                    if(NOT EXISTS ${_root}/${_libdir}/${_lib})
                        if(NCBI_TRACE_COMPONENT_${_name})
                            message("${_name}: ${_root}/${_libdir}/${_lib} not found")
                        endif()
                        set(_found NO)
                    endif()
                endforeach()
                if (_found)
                    set(_libtype ${_libdir})
                    break()
                endif()
            endforeach()
        endif()

        if (NOT _found)
            message("NOT FOUND ${_name}: some libraries not found at ${_root}")
        endif()
    endif()

    if (_found)
        message(STATUS "Found ${_name}: ${_root}")
        set(NCBI_COMPONENT_${_name}_FOUND YES PARENT_SCOPE)
        set(NCBI_COMPONENT_${_name}_INCLUDE ${_root}/include PARENT_SCOPE)
        foreach(_lib IN LISTS _args)
            set(NCBI_COMPONENT_${_name}_LIBS ${NCBI_COMPONENT_${_name}_LIBS} ${_root}/${_libtype}/${_lib})
        endforeach()
        set(NCBI_COMPONENT_${_name}_LIBS ${NCBI_COMPONENT_${_name}_LIBS} PARENT_SCOPE)
        if (EXISTS ${_root}/bin)
            set(NCBI_COMPONENT_${_name}_BINPATH ${_root}/bin PARENT_SCOPE)
        endif()

        string(TOUPPER ${_name} _upname)
        set(HAVE_LIB${_upname} 1 PARENT_SCOPE)
        string(REPLACE "." "_" _altname ${_upname})
        set(HAVE_${_altname} 1 PARENT_SCOPE)

        list(APPEND NCBI_ALL_COMPONENTS ${_name})
        set(NCBI_ALL_COMPONENTS ${NCBI_ALL_COMPONENTS} PARENT_SCOPE)
    else()
        set(NCBI_COMPONENT_${_name}_FOUND NO PARENT_SCOPE)
    endif()
endfunction()

#############################################################################
#############################################################################


#############################################################################
# local_lbsm
if(WIN32 OR CYGWIN)
    set(NCBI_COMPONENT_local_lbsm_FOUND NO)
else()
    if (EXISTS ${NCBITK_SRC_ROOT}/connect/ncbi_lbsm.c)
        set(NCBI_COMPONENT_local_lbsm_FOUND YES)
        list(APPEND NCBI_ALL_REQUIRES local_lbsm)
        set(HAVE_LOCAL_LBSM 1)
    else()
        set(NCBI_COMPONENT_local_lbsm_FOUND NO)
    endif()
endif()

#############################################################################
# LocalPCRE
if (EXISTS ${NCBITK_INC_ROOT}/util/regexp)
  set(NCBI_COMPONENT_LocalPCRE_FOUND YES)
  list(APPEND NCBI_ALL_REQUIRES LocalPCRE)
  set(NCBI_COMPONENT_LocalPCRE_INCLUDE ${NCBITK_INC_ROOT}/util/regexp)
  set(NCBI_COMPONENT_LocalPCRE_NCBILIB regexp)
else()
  set(NCBI_COMPONENT_LocalPCRE_FOUND NO)
endif()

#############################################################################
# LocalZ
if (EXISTS ${NCBITK_INC_ROOT}/util/compress/zlib)
  set(NCBI_COMPONENT_LocalZ_FOUND YES)
  list(APPEND NCBI_ALL_REQUIRES LocalZ)
  set(NCBI_COMPONENT_LocalZ_INCLUDE ${NCBITK_INC_ROOT}/util/compress/zlib)
  set(NCBI_COMPONENT_LocalZ_NCBILIB z)
else()
  set(NCBI_COMPONENT_LocalZ_FOUND NO)
endif()

#############################################################################
# LocalBZ2
if (EXISTS ${NCBITK_INC_ROOT}/util/compress/bzip2)
  set(NCBI_COMPONENT_LocalBZ2_FOUND YES)
  list(APPEND NCBI_ALL_REQUIRES LocalBZ2)
  set(NCBI_COMPONENT_LocalBZ2_INCLUDE ${NCBITK_INC_ROOT}/util/compress/bzip2)
  set(NCBI_COMPONENT_LocalBZ2_NCBILIB bz2)
else()
  set(NCBI_COMPONENT_LocalBZ2_FOUND NO)
endif()

#############################################################################
# LocalLMDB
if (EXISTS ${NCBITK_INC_ROOT}/util/lmdb AND NOT CYGWIN)
  set(NCBI_COMPONENT_LocalLMDB_FOUND YES)
  list(APPEND NCBI_ALL_REQUIRES LocalLMDB)
  set(NCBI_COMPONENT_LocalLMDB_INCLUDE ${NCBITK_INC_ROOT}/util/lmdb)
  set(NCBI_COMPONENT_LocalLMDB_NCBILIB lmdb)
else()
  set(NCBI_COMPONENT_LocalLMDB_FOUND NO)
endif()

#############################################################################
# connext
if (EXISTS ${NCBITK_SRC_ROOT}/connect/ext/CMakeLists.txt)
  set(NCBI_REQUIRE_connext_FOUND YES)
  set(HAVE_LIBCONNEXT 1)
  list(APPEND NCBI_ALL_REQUIRES connext)
endif()

#############################################################################
# PubSeqOS
if (EXISTS ${NCBITK_SRC_ROOT}/objtools/data_loaders/genbank/pubseq/CMakeLists.txt)
  set(NCBI_REQUIRE_PubSeqOS_FOUND YES)
  list(APPEND NCBI_ALL_REQUIRES PubSeqOS)
endif()

#############################################################################
# FreeTDS
set(FTDS95_INCLUDE  ${NCBITK_INC_ROOT}/dbapi/driver/ftds95  ${NCBITK_INC_ROOT}/dbapi/driver/ftds95/freetds)
set(FTDS100_INCLUDE ${NCBITK_INC_ROOT}/dbapi/driver/ftds100 ${NCBITK_INC_ROOT}/dbapi/driver/ftds100/freetds)

set(NCBI_COMPONENT_FreeTDS_FOUND   YES)
set(HAVE_LIBFTDS 1)
list(APPEND NCBI_ALL_REQUIRES FreeTDS)
set(NCBI_COMPONENT_FreeTDS_INCLUDE ${FTDS100_INCLUDE})
#set(NCBI_COMPONENT_FreeTDS_LIBS    ct_ftds100)

#############################################################################
set(NCBI_COMPONENT_Boost.Test.Included_NCBILIB test_boost)
set(NCBI_COMPONENT_SQLITE3_NCBILIB sqlitewrapp)
set(NCBI_COMPONENT_Sybase_NCBILIB  ncbi_xdbapi_ctlib)
set(NCBI_COMPONENT_ODBC_NCBILIB    ncbi_xdbapi_odbc)
set(NCBI_COMPONENT_FreeTDS_NCBILIB ct_ftds100 ncbi_xdbapi_ftds)
set(NCBI_COMPONENT_connext_NCBILIB xconnext)

#############################################################################
if (MSVC)
  include(${NCBI_TREE_CMAKECFG}/CMake.NCBIComponentsMSVC.cmake)
elseif (APPLE)
  include(${NCBI_TREE_CMAKECFG}/CMake.NCBIComponentsXCODE.cmake)
else()
    if(ON)
        include(${NCBI_TREE_CMAKECFG}/CMake.NCBIComponentsUNIXex.cmake)
    else()
        include(${NCBI_TREE_CMAKECFG}/CMake.NCBIComponentsUNIX.cmake)
    endif()
endif()

#############################################################################
# FreeTDS
set(FTDS95_INCLUDE  ${NCBITK_INC_ROOT}/dbapi/driver/ftds95  ${NCBITK_INC_ROOT}/dbapi/driver/ftds95/freetds)
set(FTDS100_INCLUDE ${NCBITK_INC_ROOT}/dbapi/driver/ftds100 ${NCBITK_INC_ROOT}/dbapi/driver/ftds100/freetds)

#############################################################################
list(SORT NCBI_ALL_LEGACY)
list(APPEND NCBI_ALL_COMPONENTS ${NCBI_ALL_LEGACY})
list(SORT NCBI_ALL_COMPONENTS)
list(SORT NCBI_ALL_REQUIRES)
