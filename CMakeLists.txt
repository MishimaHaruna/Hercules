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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
PROJECT(Hercules LANGUAGES C)

IF(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
	MESSAGE(FATAL_ERROR "Out-of-source builds are currently not supported. Please build Hercules inside the source directory.")
ENDIF()

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
INCLUDE(CheckEpoll)
INCLUDE(CheckIncludeFile)

# CMake 3.1 and higher include generator expressions of the form
# $<TARGETLIB:obj> in the SOURCES property.  These need to be
# stripped everywhere that access the SOURCES property, so we just
# defer to the OLD behavior of not including generator expressions
# in the output for now.
cmake_policy(SET CMP0051 OLD)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
if (FORCE_COLORED_OUTPUT)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

IF(UNIX AND NOT CYGWIN AND NOT $ENV{CI})
	FIND_PROGRAM(CMD_ID "id")
	IF(CMD_ID)
		EXECUTE_PROCESS(COMMAND ${CMD_ID} "-u" OUTPUT_VARIABLE SYSINFO_EUID OUTPUT_STRIP_TRAILING_WHITESPACE)
		IF(SYSINFO_EUID EQUAL 0)
			MESSAGE(FATAL_ERROR "\n"
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
		ENDIF(SYSINFO_EUID EQUAL 0)
	ENDIF(CMD_ID)
ENDIF(UNIX AND NOT CYGWIN AND NOT $ENV{CI})

# Memory Manager
SET(HERCULES_MEMMGR "builtin" CACHE STRING "memory managers: no, builtin, memwatch, dmalloc, gcollect, bcheck (defaults to builtin)")
SET_PROPERTY(CACHE HERCULES_MEMMGR PROPERTY STRINGS "no" "builtin" "memwatch" "dmalloc" "gcollect" "bcheck")
IF(HERCULES_MEMMGR)
	IF(HERCULES_MEMMGR STREQUAL "no")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_MEMMGR")
	ELSEIF(HERCULES_MEMMGR STREQUAL "memwatch")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMWATCH")
		CHECK_INCLUDE_FILE(memwatch.h HAVE_MEMWATCH_H)
		IF(NOT HAVE_MEMWATCH_H)
			MESSAGE(FATAL_ERROR "memwatch header not found")
		ENDIF(NOT HAVE_MEMWATCH_H)
	ELSEIF(HERCULES_MEMMGR STREQUAL "dmalloc")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDMALLOC -DDMALLOC_FUNC_CHECK")
		FIND_LIBRARY(DMALLOC_LIBRARY dmalloc REQUIRED)
		IF (DMALLOC_LIBRARY)
			LIST(APPEND ADDITIONAL_LINK_LIBRARIES ${DMALLOC_LIBRARY})
		ELSE(DMALLOC_LIBRARY)
			MESSAGE(FATAL_ERROR "dmalloc library not found")
		ENDIF(DMALLOC_LIBRARY)
		CHECK_INCLUDE_FILE(dmalloc.h HAVE_DMALLOC_H)
		IF(NOT HAVE_DMALLOC_H)
			MESSAGE(FATAL_ERROR "dmalloc header not found")
		ENDIF(NOT HAVE_DMALLOC_H)
	ELSEIF(HERCULES_MEMMGR STREQUAL "gcollect")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGCOLLECT")
		FIND_LIBRARY(GCOLLECT_LIBRARY gc REQUIRED)
		IF (GCOLLECT_LIBRARY)
			LIST(APPEND ADDITIONAL_LINK_LIBRARIES ${GCOLLECT_LIBRARY})
		ELSE(GCOLLECT_LIBRARY)
			MESSAGE(FATAL_ERROR "gcollect library not found")
		ENDIF(GCOLLECT_LIBRARY)
		CHECK_INCLUDE_FILE(gc.h HAVE_GCOLLECT_H)
		IF(NOT HAVE_GCOLLECT_H)
			MESSAGE(FATAL_ERROR "gcollect header not found")
		ENDIF(NOT HAVE_GCOLLECT_H)
	ELSEIF(HERCULES_MEMMGR STREQUAL "bcheck")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBCHECK")
	ELSEIF(HERCULES_MEMMGR STREQUAL "builtin")
		# Default
	ELSE()
		MESSAGE(FATAL_ERROR "Invalid memory manager specified")
	ENDIF()
ELSE(HERCULES_MEMMGR)
	MESSAGE(FATAL_ERROR "Invalid memory manager specified")
ENDIF(HERCULES_MEMMGR)

# Packetver
SET(HERCULES_PACKETVER "" CACHE STRING "Sets the PACKETVER define. (see src/common/mmo.h)")
IF(HERCULES_PACKETVER)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPACKETVER=${HERCULES_PACKETVER}")
ENDIF(HERCULES_PACKETVER)

# Packetver-RE
SET(HERCULES_PACKETVER_RE FALSE CACHE BOOL "Sets or unsets the PACKETVER_RE define - see src/common/mmo.h (currently disabled by default)")
IF(HERCULES_PACKETVER_RE)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_PACKETVER_RE")
ENDIF(HERCULES_PACKETVER_RE)

# Packetver-Zero
SET(HERCULES_PACKETVER_ZERO FALSE CACHE BOOL "Sets or unsets the PACKETVER_ZERO define - see src/common/mmo.h (currently disabled by default)")
IF(HERCULES_PACKETVER_ZERO)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPACKETVER_ZERO")
ENDIF(HERCULES_PACKETVER_ZERO)

# Packetver-Sak
SET(HERCULES_PACKETVER_SAK FALSE CACHE BOOL "Sets or unsets the PACKETVER_SAK define - see src/common/mmo.h (currently disabled by default)")
IF(HERCULES_PACKETVER_SAK)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPACKETVER_SAK")
ENDIF(HERCULES_PACKETVER_SAK)

# Packetver-Ad
SET(HERCULES_PACKETVER_AD FALSE CACHE BOOL "Sets or unsets the PACKETVER_AD define - see src/common/mmo.h (currently disabled by default)")
IF(HERCULES_PACKETVER_AD)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPACKETVER_AD")
ENDIF(HERCULES_PACKETVER_AD)

# Epoll
SET(HERCULES_EPOLL FALSE CACHE BOOL "use epoll(4) on Linux")
IF(HERCULES_EPOLL)
	CHECK_EPOLL(EPOLL_FOUND)
	IF(EPOLL_FOUND)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSOCKET_EPOLL")
	ELSE(EPOLL_FOUND)
		MESSAGE(FATAL_ERROR "epoll support explicitly enabled but not available")
	ENDIF(EPOLL_FOUND)
ENDIF(HERCULES_EPOLL)

# Obfuscation Keys
SET(HERCULES_KEY1 "" CACHE STRING "Set the first obfuscation key in the form 0xXXXXXXXX (all three keys must be specified)")
SET(HERCULES_KEY2 "" CACHE STRING "Set the second obfuscation key in the form 0xXXXXXXXX (all three keys must be specified)")
SET(HERCULES_KEY3 "" CACHE STRING "Set the third obfuscation key in the form 0xXXXXXXXX (all three keys must be specified)")
IF(HERCULES_KEY1 AND HERCULES_KEY2 AND HERCULES_KEY3)
	IF(NOT HERCULES_KEY1 MATCHES "0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]")
		MESSAGE(FATAL_ERROR "Invalid obfuscation key 1 specified")
	ELSEIF(NOT HERCULES_KEY2 MATCHES "0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]")
		MESSAGE(FATAL_ERROR "Invalid obfuscation key 2 specified")
	ELSEIF(NOT HERCULES_KEY3 MATCHES "0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]")
		MESSAGE(FATAL_ERROR "Invalid obfuscation key 3 specified")
	ELSE()
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOBFUSCATIONKEY1=${HERCULES_KEY1} -DOBFUSCATIONKEY2=${HERCULES_KEY2} -DOBFUSCATIONKEY3=${HERCULES_KEY3}")
	ENDIF()
ELSEIF(HERCULES_KEY1 OR HERCULES_KEY2 OR HERCULES_KEY3)
	MESSAGE(FATAL_ERROR "Missing obfuscation key(s). Either none or all three should be specified.")
ENDIF()

# Debug
SET(HERCULES_DEBUG TRUE CACHE BOOL "Compiles extra debug code. please note it's meaningless for visual studio projects and similar IDE. (TRUE by default) (Boolean)")
IF(HERCULES_DEBUG)
	SET(CMAKE_BUILD_TYPE "Debug")
ELSE()
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF(HERCULES_DEBUG)

# Buildbot
SET(HERCULES_BUILDBOT FALSE CACHE BOOL "Enables buildbot-specific code")
IF(HERCULES_BUILDBOT)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBUILDBOT")
ENDIF(HERCULES_BUILDBOT)

# RDTSC as Tick Source
SET(HERCULES_RDTSC FALSE CACHE BOOL "Uses rdtsc as timing source (disabled by default).
Enable it when you've timing issues.
(For example:  in conjunction with XEN or Other Virtualization mechanisms)\n
Note:
	Please ensure that you've disabled dynamic CPU-Frequencys, such as power saving options.
	(On most modern Dedicated Servers cpufreq is preconfigured, see your distribution's
	manual how to disable it).
	Furthermore, If your CPU has built-in CPU-Frequency scaling features (such as Intel's
	SpeedStep(R)), do not enable this option. Recent CPUs (Intel Core or newer) guarantee
	a fixed increment rate for their TSC, so it should be safe to use, but please doublecheck
	the documentation of both your CPU and OS before enabling this option.")
IF(HERCULES_RDTSC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_RDTSC")
ENDIF(HERCULES_RDTSC)

# Profiler
SET(HERCULES_PROFILER "none" CACHE STRING "profilers: none, gprof (defaults to none)")
SET_PROPERTY(CACHE HERCULES_PROFILER PROPERTY STRINGS "none" "gprof")
IF(HERCULES_PROFILER)
	IF(HERCULES_PROFILER STREQUAL "gprof")
		IF(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
		ELSE()
			MESSAGE(FATAL_ERROR "The gprof profiler is only available in GNU compilers")
		ENDIF()
	ELSEIF(HERCULES_PROFILER STREQUAL "none")
		# Default
	ELSE()
		MESSAGE(FATAL_ERROR "Invalid profiler specified")
	ENDIF()
ELSE()
	MESSAGE(FATAL_ERROR "Invalid profiler specified")
ENDIF()

# LTO
SET(HERCULES_LTO FALSE CACHE BOOL "Enables or Disables Linktime Code Optimization (LTO is disabled by default)")
IF(HERCULES_LTO)
	IF(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
		add_compile_options(-flto -ffat-lto-objects -Werror)
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto -ffat-lto-objects")
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -ffat-lto-objects")
	ELSE()
		message(FATAL_ERROR "The LTO build option is only available in GNU compilers")
	ENDIF()
ENDIF()

# Sanitizer
SET(HERCULES_SANITIZER_ENABLED FALSE INTERNAL)

SET(HERCULES_ADDRESS_SANITIZER FALSE CACHE BOOL "Enables address sanitizer. (disabled by default)")
IF(HERCULES_ADDRESS_SANITIZER)
	FIND_PACKAGE(Sanitizers)
	SET(SANITIZE_ADDRESS ON)
	SET(HERCULES_SANITIZER_ENABLED TRUE)
ENDIF(HERCULES_ADDRESS_SANITIZER)

SET(HERCULES_MEMORY_SANITIZER FALSE CACHE BOOL "Enables memory sanitizer. (disabled by default)")
IF(HERCULES_MEMORY_SANITIZER)
	FIND_PACKAGE(Sanitizers)
	SET(SANITIZE_MEMORY ON)
	SET(HERCULES_SANITIZER_ENABLED TRUE)
ENDIF(HERCULES_MEMORY_SANITIZER)

SET(HERCULES_THREAD_SANITIZER FALSE CACHE BOOL "Enables thread sanitizer. (disabled by default)")
IF(HERCULES_THREAD_SANITIZER)
	FIND_PACKAGE(Sanitizers)
	SET(SANITIZE_THREAD ON)
	SET(HERCULES_SANITIZER_ENABLED TRUE)
ENDIF(HERCULES_THREAD_SANITIZER)

# Werror
SET(HERCULES_WERROR FALSE CACHE BOOL "Enables -Werror in the compiler flags. (disabled by default)")
IF(HERCULES_WERROR)
	add_compile_options(-Werror)
ENDIF()

# Renewal
SET(HERCULES_DISABLE_RENEWAL FALSE CACHE BOOL "Disable Ragnarok Renewal support (Boolean) (override settings in src/config/renewal.h)")
IF(HERCULES_DISABLE_RENEWAL)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_RENEWAL")
ENDIF(HERCULES_DISABLE_RENEWAL)

# maxconn
SET(HERCULES_MAXCONN "16384" CACHE STRING "optionally set the maximum connections the core can handle (default: 16384) NOT USED YET - EXPERIMENTAL")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMAXCONN=${HERCULES_MAXCONN}")

# Coverage
SET(HERCULES_TEST_COVERAGE FALSE CACHE BOOL "Enable test coverage reports, it also forces a debug build. (requires gcov/lcov)")
IF(HERCULES_TEST_COVERAGE)
	IF(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
		INCLUDE(CodeCoverage)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -coverage")
		SETUP_TARGET_FOR_COVERAGE_GCOVR(coverage "ctest" "output/coverage/coverage" "--verbose")
	ELSE()
		MESSAGE(FATAL_ERROR "The gcov/lcov coverage is only available in GNU compilers")
	ENDIF()
ENDIF()

# Build Documents
OPTION(HERCULES_BUILD_DOCS "Enables a build option for building source documents through Doxygen (Boolean) (Disabled by default)" FALSE)
IF(HERCULES_BUILD_DOCS)
	FIND_PACKAGE(Doxygen REQUIRED)
ENDIF(HERCULES_BUILD_DOCS)

FIND_PACKAGE(MySQL 5 REQUIRED)
IF(MYSQL_FOUND)
	INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIRS})
ENDIF(MYSQL_FOUND)

FIND_PACKAGE(ZLIB REQUIRED)
IF(ZLIB_FOUND)
	INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
ENDIF(ZLIB_FOUND)

FIND_PACKAGE(Threads REQUIRED)

FIND_LIBRARY(M_LIBRARY m)
IF(M_LIBRARY)
	LIST(APPEND ADDITIONAL_LINK_LIBRARIES ${M_LIBRARY})
ENDIF(M_LIBRARY)

FIND_PACKAGE(PCRE REQUIRED)
IF(PCRE_FOUND)
	INCLUDE_DIRECTORIES(${PCRE_INCLUDE_DIRS})
ENDIF(PCRE_FOUND)

IF(WIN32)
	LIST(APPEND ADDITIONAL_LINK_LIBRARIES wsock32)
	LIST(APPEND ADDITIONAL_LINK_LIBRARIES ws2_32)
	# First for the generic no-config case (e.g. with mingw)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	# Second, for multi-config builds (e.g. msvc)
	FOREACH(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		STRING(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
		SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
	ENDFOREACH(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFD_SETSIZE=4096")
	IF(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
		#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
	ENDIF(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
ENDIF(WIN32)

STRING(TIMESTAMP YEAR "%Y" UTC)

ENABLE_TESTING()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdparty)

ADD_SUBDIRECTORY(3rdparty/libconfig)
ADD_SUBDIRECTORY(3rdparty/mt19937ar)
ADD_SUBDIRECTORY(src/common)
ADD_SUBDIRECTORY(src/login)
ADD_SUBDIRECTORY(src/char)
ADD_SUBDIRECTORY(src/map)
ADD_SUBDIRECTORY(src/plugins)
ADD_SUBDIRECTORY(src/test)
IF(HERCULES_BUILD_DOCS)
	ADD_SUBDIRECTORY(tools/doxygen)
ENDIF()
