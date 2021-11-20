#############################################################################
# $Id: CMakeChecks.basic-checks.cmake 606763 2020-04-27 13:08:27Z ucko $
#############################################################################
# Basic check: headers, functions and types

include(CheckIncludeFile)
check_include_file(alloca.h HAVE_ALLOCA_H)
check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
check_include_file(atomic.h HAVE_ATOMIC_H)
check_include_file(com_err.h HAVE_COM_ERR_H)
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(getopt.h HAVE_GETOPT_H)
check_include_file(ieeefp.h HAVE_IEEEFP_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(langinfo.h HAVE_LANGINFO_H)
check_include_file(libgen.h HAVE_LIBGEN_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(locale.h HAVE_LOCALE_H)
check_include_file(malloc.h HAVE_MALLOC_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(net/inet/in.h HAVE_NET_INET_IN_H)
check_include_file(netdb.h HAVE_NETDB_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(netinet/tcp.h HAVE_NETINET_TCP_H)
check_include_file(odbcss.h HAVE_ODBCSS_H)
check_include_file(paths.h HAVE_PATHS_H)
check_include_file(poll.h HAVE_POLL_H)
check_include_file(roken.h HAVE_ROKEN_H)
check_include_file(select.h HAVE_SELECT_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(sys/eventfd.h HAVE_SYS_EVENTFD_H)
check_include_file(sys/file.h HAVE_SYS_FILE_H)
check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_file(sys/mount.h HAVE_SYS_MOUNT_H)
check_include_file(sys/param.h HAVE_SYS_PARAM_H)
check_include_file(sys/resource.h HAVE_SYS_RESOURCE_H)
check_include_file(sys/select.h HAVE_SYS_SELECT_H)
check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_file(sys/sockio.h HAVE_SYS_SOCKIO_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/statvfs.h HAVE_SYS_STATVFS_H)
check_include_file(sys/sysinfo.h HAVE_SYS_SYSINFO_H)
check_include_file(sys/sysmacros.h HAVE_SYS_SYSMACROS_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/vfs.h HAVE_SYS_VFS_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(wchar.h HAVE_WCHAR_H)
if (HAVE_WCHAR_H)
    set(HAVE_WSTRING 1)
endif (HAVE_WCHAR_H)
check_include_file(windows.h HAVE_WINDOWS_H)
check_include_file(sys/epoll.h HAVE_SYS_EPOLL_H)

include(CheckIncludeFileCXX)
check_include_file_cxx(fstream HAVE_FSTREAM)
check_include_file_cxx(fstream.h HAVE_FSTREAM_H)
check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(iostream.h HAVE_IOSTREAM_H)
check_include_file_cxx(limits HAVE_LIMITS)
check_include_file_cxx(strstrea.h HAVE_STRSTREA_H)
check_include_file_cxx(strstream HAVE_STRSTREAM)
check_include_file_cxx(strstream.h HAVE_STRSTREAM_H)

include(CheckFunctionExists)
check_function_exists(alarm HAVE_ALARM)
check_function_exists(asprintf HAVE_ASPRINTF)
check_function_exists(atoll HAVE_ATOLL)
check_function_exists(basename HAVE_BASENAME)
check_function_exists(daemon HAVE_DAEMON)
check_function_exists(dbopen HAVE_DBOPEN)
check_function_exists(erf HAVE_ERF)
check_function_exists(euidaccess HAVE_EUIDACCESS)
check_function_exists(eventfd HAVE_EVENTFD)
check_function_exists(freehostent HAVE_FREEHOSTENT)
check_function_exists(fseeko HAVE_FSEEKO)
check_function_exists(fstat HAVE_FSTAT)
check_function_exists(getaddrinfo HAVE_GETADDRINFO)
check_function_exists(gethostent_r HAVE_GETHOSTENT_R)
check_function_exists(gethrtime HAVE_GETHRTIME)
check_function_exists(getipnodebyaddr HAVE_GETIPNODEBYADDR)
check_function_exists(getipnodebyname HAVE_GETIPNODEBYNAME)
check_function_exists(getloadavg HAVE_GETLOADAVG)
check_function_exists(getlogin_r HAVE_GETLOGIN_R)
check_function_exists(getnameinfo HAVE_GETNAMEINFO)
check_function_exists(getopt HAVE_GETOPT)
check_function_exists(getpagesize HAVE_GETPAGESIZE)
check_function_exists(getpass HAVE_GETPASS)
check_function_exists(getpassphrase HAVE_GETPASSPHRASE)
check_function_exists(getpwuid HAVE_GETPWUID)
check_function_exists(getrusage HAVE_GETRUSAGE)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(getuid HAVE_GETUID)
check_function_exists(inet_ntoa_r HAVE_INET_NTOA_R)
check_function_exists(inet_ntop HAVE_INET_NTOP)
check_function_exists(lchown HAVE_LCHOWN)
#TODO return type check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(lutimes HAVE_LUTIMES)
check_function_exists(malloc_options HAVE_MALLOC_OPTIONS)
check_function_exists(memrchr HAVE_MEMRCHR)
check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(poll HAVE_POLL)
check_function_exists(pthread_atfork HAVE_PTHREAD_ATFORK)
check_function_exists(pthread_cond_timedwait_relative_np
                      HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP)
check_function_exists(pthread_condattr_setclock HAVE_PTHREAD_CONDATTR_SETCLOCK)
check_function_exists(pthread_setconcurrency HAVE_PTHREAD_SETCONCURRENCY)
check_function_exists(putenv HAVE_PUTENV)
check_function_exists(readpassphrase HAVE_READPASSPHRASE)
check_function_exists(readv HAVE_READV)
check_function_exists(sched_yield HAVE_SCHED_YIELD)
check_function_exists(select HAVE_SELECT)
check_function_exists(setenv HAVE_SETENV)
check_function_exists(setitimer HAVE_SETITIMER)
check_function_exists(setrlimit HAVE_SETRLIMIT)
check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(socketpair HAVE_SOCKETPAIR)
check_function_exists(statfs HAVE_STATFS)
check_function_exists(statvfs HAVE_STATVFS)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strlcat HAVE_STRLCAT)
check_function_exists(strlcpy HAVE_STRLCPY)
check_function_exists(strndup HAVE_STRNDUP)
check_function_exists(strnlen HAVE_STRNLEN)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(memcchr HAVE_MEMCCHR)
check_function_exists(memrchr HAVE_MEMRCHR)
check_function_exists(strsep HAVE_STRSEP)
check_function_exists(strtok_r HAVE_STRTOK_R)
check_function_exists(timegm HAVE_TIMEGM)
check_function_exists(utimes HAVE_UTIMES)
check_function_exists(vasprintf HAVE_VASPRINTF)
check_function_exists(vprintf HAVE_VPRINTF)
check_function_exists(vsnprintf HAVE_VSNPRINTF)
check_function_exists(writev HAVE_WRITEV)

# TODO Need to check the number of arguments for gethostbyaddr_r, gethostbyname_r, getservbyname_r 
check_function_exists(gethostbyaddr_r HAVE_GETHOSTBYADDR_R)
check_function_exists(gethostbyname_r HAVE_GETHOSTBYNAME_R)
check_function_exists(getservbyname_r HAVE_GETSERVBYNAME_R)

check_include_file_cxx(cxxabi.h HAVE_CXXABI_H)
if (${HAVE_CXXABI_H})
    include(CheckCXXSymbolExists)
    check_cxx_symbol_exists(abi::__cxa_demangle cxxabi.h HAVE_CXA_DEMANGLE)
endif ()

include(CheckSymbolExists)
check_symbol_exists(FIONBIO "sys/ioctl.h" BSD_COMP)

include(CheckTypeSize)
check_type_size(char SIZEOF_CHAR)
check_type_size(double SIZEOF_DOUBLE)
check_type_size(float SIZEOF_FLOAT)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size("long double" SIZEOF_LONG_DOUBLE)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size(short SIZEOF_SHORT)
check_type_size(size_t SIZEOF_SIZE_T)
check_type_size(void* SIZEOF_VOIDP)
check_type_size(__int64 SIZEOF___INT64)
if (HAVE_WCHAR_H)
check_type_size(wchar_t SIZEOF_WCHAR_T)
endif()

set(CMAKE_EXTRA_INCLUDE_FILES_BACKUP CMAKE_EXTRA_INCLUDE_FILES)
set(CMAKE_EXTRA_INCLUDE_FILES sys/socket.h)
check_type_size(socklen_t HAVE_SOCKLEN_T)
set(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES_BACKUP})

set(CMAKE_EXTRA_INCLUDE_FILES_BACKUP CMAKE_EXTRA_INCLUDE_FILES)
set(CMAKE_EXTRA_INCLUDE_FILES sys/socket.h)
check_type_size(socklen_t SOCKLEN_T)
set(CMAKE_EXTRA_INCLUDE_FILES CMAKE_EXTRA_INCLUDE_FILES_BACKUP)

set(CMAKE_EXTRA_INCLUDE_FILES_BACKUP CMAKE_EXTRA_INCLUDE_FILES)
set(CMAKE_EXTRA_INCLUDE_FILES sys/sem.h)
check_type_size(semun_t SEMUN)
set(CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES_BACKUP})

