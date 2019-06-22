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

################  PLUGIN CONFIGURATION  ##############################
#                                                                    #
# When you add a plugin, add its name here:                          #
# Example: if you have a plugin named my_cool_plugin.c and another   #
# one named my_second_plugin.c, add them to the list like this:      #
#                                                                    #
# SET(MYPLUGINS my_cool_plugin my_second_plugin)                     #
#                                                                    #
# This is only needed if you want to build your plugin through       #
#   default build options. If you don't add it to this list,         #
#   you will still be able to build your plugin through              #
#   'cmake -DPLUGINS="foo bar"'                                      #
#                                                                    #
# Note: DO NOT include the .c extension!!!                           #

SET(MYPLUGINS)

#                                                                    #
#########  DO NOT EDIT ANYTHING BELOW THIS LINE!!!  ##################

SET(PLUGINS_OUTPUT_PATH "${CMAKE_BINARY_DIR}/plugins")
SET(PLUGIN_DEFAULT_LINKING
	mt19937ar
	${MYSQL_LIBRARIES}
	${ZLIB_LIBRARIES}
	${PCRE_LIBRARIES}
	${CMAKE_DL_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
	${ADDITIONAL_LINK_LIBRARIES}
	)

SET(HERCULES_PLUGINS "" CACHE STRING "Pass each individual plugin name with spaces in between to compile it")

FUNCTION(ADD_PLUGIN name)
	ADD_LIBRARY(${name} SHARED ${name}.c)
	TARGET_LINK_LIBRARIES(${name} ${PLUGIN_DEFAULT_LINKING})
	FOREACH(link_library ${ARGN})
		TARGET_LINK_LIBRARIES(${name} ${link_library})
	ENDFOREACH()
	SET_TARGET_PROPERTIES(${name} PROPERTIES PREFIX "")
ENDFUNCTION()

FUNCTION(ADD_HPM_PLUGIN name)
	STRING(TOUPPER ${name} module_name)
	SET(name HPMHooking_${name})

	ADD_LIBRARY(${name} SHARED HPMHooking.c)
	TARGET_LINK_LIBRARIES(${name} ${PLUGIN_DEFAULT_LINKING})
	TARGET_COMPILE_DEFINITIONS(${name} PRIVATE "HPMHOOKING_${module_name}")
	SET_TARGET_PROPERTIES(${name} PROPERTIES PREFIX "")
ENDFUNCTION()

IF(NOT HERCULES_PLUGINS)
	ADD_PLUGIN(sample)
	ADD_PLUGIN(db2sql)
	ADD_HPM_PLUGIN(char)
	ADD_HPM_PLUGIN(login)
	ADD_HPM_PLUGIN(map)
	FOREACH(_ADDITIONAL_PLUGIN ${MYPLUGINS})
		ADD_PLUGIN(${_ADDITIONAL_PLUGIN})
	ENDFOREACH()

ELSE()
	# First we convert plugin names to a list by replacing space by ;
	STRING(REPLACE " " ";" PLUGINS_LIST ${HERCULES_PLUGINS})

	FOREACH(PLUGIN_NAME ${PLUGINS_LIST})
		STRING(FIND ${PLUGIN_NAME} "HPMHooking_" SEARCH_RESULT)
		IF (SEARCH_RESULT EQUAL -1)
			ADD_PLUGIN(${PLUGIN_NAME})
		ELSE()
			STRING(SUBSTRING ${PLUGIN_NAME} 11 -1 HPM_SERVER_TYPE)
			ADD_HPM_PLUGIN(${HPM_SERVER_TYPE})
		ENDIF()
	ENDFOREACH()
ENDIF()

SET(LIBRARY_OUTPUT_PATH ${PLUGINS_OUTPUT_PATH})
