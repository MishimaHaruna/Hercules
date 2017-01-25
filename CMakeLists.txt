# This file is part of Hercules.
# http://herc.ws - http://github.com/HerculesWS/Hercules
#
# Copyright (C) 2016-2017  Hercules Dev Team
#
# Hercules is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.24)
project(Hercules C)

if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message(FATAL_ERROR "Out-of-source builds are currently not supported. Please build Hercules inside the source directory.")
endif()

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckCCompilerFlag)

#
# Function to check the validity of compiler option
# if valid it will be added
#
function(checkoption option)
  string(REPLACE "-" "" sub ${option}) 
  string(REPLACE "=" "IS" var ${sub}) 
  check_c_compiler_flag(${option} HAVE${var})
  if(${HAVE${var}})
    add_compile_options(${option})
  endif()
endfunction()

if(UNIX AND NOT CYGWIN AND NOT $ENV{CI})
  find_program(CMD_ID "id")
  if(CMD_ID)
    execute_process(COMMAND ${CMD_ID} "-u" OUTPUT_VARIABLE SYSINFO_EUID OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(SYSINFO_EUID EQUAL 0)
      message(FATAL_ERROR "\n"
        "********************************************************************************\n"
        "*                            W A R N I N G      /!\\                            *\n"
        "********************************************************************************\n"
        "* It appears that you're planning to run Hercules with root privileges. That's *\n"
        "* not necessary, nor recommended, and it may open your machine to unnecessary  *\n"
        "* security risks.  You should never ever run software as root unless it        *\n"
        "* requires the extra privileges (which Hercules does not.)                     *\n"
        "*                                                                              *\n"
        "* More info:                                                                   *\n"
        "* http://www.tldp.org/HOWTO/Security-HOWTO/local-security.html                 *\n"
        "* https://wiki.debian.org/sudo                                                 *\n"
        "* http://wiki.centos.org/TipsAndTricks/BecomingRoot                            *\n"
        "* http://fedoraproject.org/wiki/Configuring_Sudo                               *\n"
        "* https://help.ubuntu.com/community/RootSudo                                   *\n"
        "* http://www.freebsdwiki.net/index.php/Root                                    *\n"
        "*                                                                              *\n"
        "* If your service provider forces (or encourages) you to run server software   *\n"
        "* as root, please complain to them. It is a very bad idea.                     *\n"
        "********************************************************************************\n"
      )
    endif()
  endif()
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(_GNU_SOURCE TRUE)
endif()

add_definitions("-DHAVE_CONFIG_H")

#
# Check for monotonic clock
#
check_c_source_runs("
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  struct timespec tval;
  return clock_gettime(CLOCK_MONOTONIC, &tval);
}
" MONOTONIC)
if(${MONOTONIC_COMPILED})
  if(${MONOTONIC_EXITCODE} EQUAL 0)
    message(STATUS "Monotonic Clock - found")
    set(HAVE_MONOTONIC_CLOCK TRUE)
  else()
    message(STATUS "Monotonic Clock - not found")
  endif()
else()
  message(STATUS "Monotonic Clock - not found")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  #
  # Check to see if it makes sense to use -Wshadow
  #
  set(tmp "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Wshadow -Werror")
  check_c_source_compiles("
    int foo(void) {
      return 0;
    }
    int main(int argc, char **argv) {
      int foo = 0;
      return foo + 1;
    }
    " SHADOW
  )
  set(CMAKE_REQUIRED_FLAGS "${tmp}")
  if(${SHADOW})
    add_compile_options(-Wshadow)
  endif()
endif()
#
# Check for TLS (Thread Local Storage)
#
check_c_source_runs("
  __thread int x=-1;
  int main(int argc, char **argv)
  {
    x=0;
    return x;
  }
  " TLS
)
if(${TLS_COMPILED})
  if(${TLS_EXITCODE} EQUAL 0)
    message(STATUS "TLS - found")
    set(HAS_TLS TRUE)
  else()
    message(STATUS "TLS - not found")
  endif()
else()
  message(STATUS "TLS - not found")
endif()
#
# Assume Debug as default build type
#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  #
  # Debug specific options
  #
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "In debug mode")
    checkoption(-fno-omit-frame-pointer)
    checkoption(-Wnull-dereference)
    set(DEBUG TRUE)
    if(${USE_GDB})
      checkoption(-ggdb)
      add_link_options(-ggdb)
    else()
      checkoption(-g)
    endif()
  endif()
endif()

#
# Check for function strnlen
#
check_function_exists(strnlen HAVE_STRNLEN)
#
# Check for function setrlimit
#
check_function_exists(setrlimit HAVE_SETRLIMIT)
#
# Check for execinfo
#
check_include_file("execinfo.h" HAVE_EXECINFO)
#
# check for epoll if enabled
#
if(${ENABLE_EPOLL})
  check_include_file("sys/epoll.h" SOCKET_EPOLL)
  if(${SOCKET_EPOLL})
    set(DSOCKET_EPOLL TRUE)
  else()
    message(FATAL_ERROR "EPOLL not found while explictly enabled")
  endif()
endif()
#
# check for xlocale
#
check_function_exists(uselocale HAVE_USELOCALE)
check_function_exists(newlocale HAVE_NEWLOCALE)
check_function_exists(freelocale HAVE_FREELOCALE)
check_include_file("xlocale.h" HAVE_XLOCALE_H)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  #
  # Add valid compile options
  #
  foreach(option 
    -Wall
    -Wcast-qual
    -Wempty-body
    -Wextra
    -Wformat
    -Wformat-security
    -Wformat-signedness
    -Wformat-y2k
    -Winit-self
    -Wint-conversion
    -Wmisleading-indentation
    -Wnested-externs
    -Wno-clobbered
    -Wno-format-nonliteral
    -Wno-missing-braces
    -Wno-missing-field-initializers
    -Wno-sign-compare
    -Wno-suggest-attribute=format
    -Wno-switch
    -Wno-unused-parameter
    -Wold-style-definition
    -Woverlength-strings
    -Wpointer-arith
    -Wredundant-decls
    -Wsuggest-attribute=noreturn
    -Wswitch-bool
    -Wundef
    -Wvla
    -ffast-math
    -fno-strict-aliasing
    -fno-var-tracking
    -pipe
  )
    checkoption(${option})
  endforeach()
  #
  # -Werror only if not disabled explicitly
  #
  if(NOT DISABLE_WERROR)
    checkoption(-Werror)
  endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  #
  # Add valid compile options
  #
  foreach(option 
    /Wall
  )
    checkoption(${option})
  endforeach()
  if(NOT DISABLE_WERROR)
    checkoption(/W3)
    #checkoption(/W4)
    #checkoption(/WX)
  endif()
endif()

#
# Uncomment next two lines to debug CMake
#
#add_compile_options(-v)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)
conan_cmake_configure(
  REQUIRES
    libmysqlclient/8.0.25
    pcre/8.45
    zlib/1.2.11
  GENERATORS
    cmake_find_package
  )
conan_cmake_autodetect(settings)
conan_cmake_install(
  PATH_OR_REFERENCE .
  INSTALL_FOLDER ${CMAKE_BINARY_DIR}/CMakeFiles/conan
  BUILD missing
  REMOTE conancenter
  SETTINGS ${settings}
)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/CMakeFiles/conan/")

find_package(MySQL 5.6 REQUIRED)
find_package(PCRE 8.0 REQUIRED)
find_package(ZLIB 1.2 REQUIRED)

find_package(Threads REQUIRED)

if(WIN32)
  list(APPEND ADDITIONAL_LINK_LIBRARIES wsock32)
  list(APPEND ADDITIONAL_LINK_LIBRARIES ws2_32)
  # First for the generic no-config case (e.g. with mingw)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  # Second, for multi-config builds (e.g. msvc)
  foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
  endforeach()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFD_SETSIZE=4096")
  if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
  endif()
endif()

# Memory Manager
set(HERCULES_MEMMGR "builtin" CACHE STRING "memory managers: no, builtin, memwatch, dmalloc, gcollect, bcheck (defaults to builtin)")
set_property(CACHE HERCULES_MEMMGR PROPERTY STRINGS "no" "builtin" "memwatch" "dmalloc" "gcollect" "bcheck")
if(HERCULES_MEMMGR)
  if(HERCULES_MEMMGR STREQUAL "no")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_MEMMGR")
  elseif(HERCULES_MEMMGR STREQUAL "memwatch")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMWATCH")
    check_include_file(memwatch.h HAVE_MEMWATCH_H)
    if(NOT HAVE_MEMWATCH_H)
      message(FATAL_ERROR "memwatch header not found")
    endif()
  elseif(HERCULES_MEMMGR STREQUAL "dmalloc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDMALLOC -DDMALLOC_FUNC_CHECK")
    find_library(DMALLOC_LIBRARY dmalloc REQUIRED)
    if(DMALLOC_LIBRARY)
      list(APPEND ADDITIONAL_LINK_LIBRARIES ${DMALLOC_LIBRARY})
    else()
      message(FATAL_ERROR "dmalloc library not found")
    endif()
    check_include_file(dmalloc.h HAVE_DMALLOC_H)
    if(NOT HAVE_DMALLOC_H)
      message(FATAL_ERROR "dmalloc header not found")
    endif()
  elseif(HERCULES_MEMMGR STREQUAL "gcollect")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGCOLLECT")
    find_library(GCOLLECT_LIBRARY gc REQUIRED)
    if(GCOLLECT_LIBRARY)
      list(APPEND ADDITIONAL_LINK_LIBRARIES ${GCOLLECT_LIBRARY})
    else()
      message(FATAL_ERROR "gcollect library not found")
    endif()
    check_include_file(gc.h HAVE_GCOLLECT_H)
    if(NOT HAVE_GCOLLECT_H)
      message(FATAL_ERROR "gcollect header not found")
    endif()
  elseif(HERCULES_MEMMGR STREQUAL "bcheck")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBCHECK")
  elseif(HERCULES_MEMMGR STREQUAL "builtin")
    # Default
  else()
    message(FATAL_ERROR "Invalid memory manager specified")
  endif()
else()
  message(FATAL_ERROR "Invalid memory manager specified")
endif()

# Packetver
set(HERCULES_PACKETVER "" CACHE STRING "Sets the PACKETVER define. (see src/common/mmo.h)")
if(HERCULES_PACKETVER)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPACKETVER=${HERCULES_PACKETVER}")
endif()

# Packetver-RE
set(HERCULES_PACKETVER_RE FALSE CACHE BOOL "Sets or unsets the PACKETVER_RE define - see src/common/mmo.h (currently disabled by default)")
if(HERCULES_PACKETVER_RE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_PACKETVER_RE")
endif()

# Epoll
set(HERCULES_EPOLL FALSE CACHE BOOL "use epoll(4) on Linux")
if(HERCULES_EPOLL)
  check_include_file("sys/epoll.h" EPOLL_FOUND)
  if(EPOLL_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSOCKET_EPOLL")
  else()
    message(FATAL_ERROR "epoll support explicitly enabled but not available")
  endif()
endif()

# Obfuscation Keys
set(HERCULES_KEY1 "" CACHE STRING "Set the first obfuscation key in the form 0xXXXXXXXX (all three keys must be specified)")
set(HERCULES_KEY2 "" CACHE STRING "Set the second obfuscation key in the form 0xXXXXXXXX (all three keys must be specified)")
set(HERCULES_KEY3 "" CACHE STRING "Set the third obfuscation key in the form 0xXXXXXXXX (all three keys must be specified)")
if(HERCULES_KEY1 AND HERCULES_KEY2 AND HERCULES_KEY3)
  if(NOT HERCULES_KEY1 MATCHES "0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]")
    message(FATAL_ERROR "Invalid obfuscation key 1 specified")
  elseif(NOT HERCULES_KEY2 MATCHES "0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]")
    message(FATAL_ERROR "Invalid obfuscation key 2 specified")
  elseif(NOT HERCULES_KEY3 MATCHES "0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]")
    message(FATAL_ERROR "Invalid obfuscation key 3 specified")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOBFUSCATIONKEY1=${HERCULES_KEY1} -DOBFUSCATIONKEY2=${HERCULES_KEY2} -DOBFUSCATIONKEY3=${HERCULES_KEY3}")
  endif()
elseif(HERCULES_KEY1 OR HERCULES_KEY2 OR HERCULES_KEY3)
  message(FATAL_ERROR "Missing obfuscation key(s). Either none or all three should be specified.")
ENDIF()

# Buildbot
set(HERCULES_BUILDBOT FALSE CACHE BOOL "Enables buildbot-specific code")
if(HERCULES_BUILDBOT)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBUILDBOT")
endif()

# RDTSC as Tick Source
set(HERCULES_RDTSC FALSE CACHE BOOL "Uses rdtsc as timing source (disabled by default).
Enable it when you've timing issues.
(For example:  in conjunction with XEN or Other Virtualization mechanisms)\n
Note:
  Please ensure that you've disabled dynamic CPU-Frequencys, such as power saving options.
  (On most modern Dedicated Servers cpufreq is preconfigured, see your distribution's
  manual how to disable it).
  Furthermore, If your CPU has built-in CPU-Frequency scaling features (such as Intel's
  SpeedStep(R)), do not enable this option. Recent CPUs (Intel Core or newer) guarantee
  a fixed increment rate for their TSC, so it should be safe to use, but please doublecheck
  the documentation of both your CPU and OS before enabling this option."
)
if(HERCULES_RDTSC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_RDTSC")
endif()

# Profiler
set(HERCULES_PROFILER "none" CACHE STRING "profilers: none, gprof (defaults to none)")
set_property(CACHE HERCULES_PROFILER PROPERTY STRINGS "none" "gprof")
if(HERCULES_PROFILER)
  if(HERCULES_PROFILER STREQUAL "gprof")
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    else()
      message(FATAL_ERROR "The gprof profiler is only available in GNU compilers")
    endif()
  elseif(HERCULES_PROFILER STREQUAL "none")
    # Default
  else()
    message(FATAL_ERROR "Invalid profiler specified")
  endif()
else()
  message(FATAL_ERROR "Invalid profiler specified")
endif()


find_library(M_LIBRARY m)
if(M_LIBRARY)
  list(APPEND ADDITIONAL_LINK_LIBRARIES ${M_LIBRARY})
endif()

string(TIMESTAMP YEAR "%Y" UTC)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty)

add_library(common "")
add_library(libconfig "")
add_library(mt19937ar "")

add_executable(map "")
add_executable(char "")
add_executable(login "")
add_executable(test_libconfig "")
add_executable(test_spinlock "")

add_subdirectory("src/common")
add_subdirectory("3rdparty/libconfig")
add_subdirectory("3rdparty/mt19937ar")

add_subdirectory("src/map")
add_subdirectory("src/char")
add_subdirectory("src/login")
add_subdirectory("src/test")

enable_testing()
add_test(NAME "libconfig" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND "test_libconfig")
add_test(NAME "libspinlock" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND "test_spinlock")

#ADD_SUBDIRECTORY(3rdparty/libconfig)
#ADD_SUBDIRECTORY(3rdparty/mt19937ar)
#ADD_SUBDIRECTORY(src/common)
#ADD_SUBDIRECTORY(src/login)

configure_file("src/common/config.h.cm" "${CMAKE_SOURCE_DIR}/src/common/config.h")
configure_file("3rdparty/libconfig/config.h.cm" "${CMAKE_SOURCE_DIR}/3rdparty/libconfig/config.h")
