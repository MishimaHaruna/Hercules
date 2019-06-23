# This file is part of Hercules.
# http://herc.ws - http://github.com/HerculesWS/Hercules
#
# Copyright (C) 2019  Hercules Dev Team
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

SET(TESTS_OUTPUT_PATH "${CMAKE_BINARY_DIR}/tests")
SET(TESTS_DEFAULT_LINKING
	herculescore
	mt19937ar
	${MYSQL_LIBRARIES}
	${ZLIB_LIBRARIES}
	${PCRE_LIBRARIES}
	${CMAKE_DL_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
	${ADDITIONAL_LINK_LIBRARIES}
	)

FUNCTION(CREATE_TEST testname sources)
	SET(name "test_${testname}")
	ADD_EXECUTABLE(${name} ${sources})
	TARGET_LINK_LIBRARIES(${name} PRIVATE ${TESTS_DEFAULT_LINKING})

	FOREACH(link_library ${ARGN})
		TARGET_LINK_LIBRARIES(${name} ${link_library})
	ENDFOREACH()

	ADD_TEST(NAME ${name}
		COMMAND ${name}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)

	IF(HERCULES_TEST_COVERAGE)
		ADD_DEPENDENCIES(coverage ${name})
	ENDIF(HERCULES_TEST_COVERAGE)
	
	IF(HERCULES_SANITIZER_ENABLED)
		add_sanitizers(${name})
	ENDIF()
ENDFUNCTION()

CREATE_TEST(libconfig test_libconfig.c)
CREATE_TEST(spinlock test_spinlock.c)

SET(EXECUTABLE_OUTPUT_PATH ${TESTS_OUTPUT_PATH})
