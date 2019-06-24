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

SET(HPMHOOK_GEN_DIRECTORY "${CMAKE_BINARY_DIR}/tools/HPMHookGen")
SET(DOXYGEN_CONFIG_PATH "doxygen.conf")
SET(HPMHOOK_GEN_DEFINATION "HPMHookGen.pl")
SET(HPMHOOK_GEN_DATACHECK "HPMDataCheckGen.pl")

ADD_CUSTOM_TARGET(hooks_build_doxygen ALL
                  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_CONFIG_PATH}"
                  WORKING_DIRECTORY ${HPMHOOK_GEN_DIRECTORY}
                  COMMENT "Extracting functions information..."
				  VERBATIM)

ADD_CUSTOM_TARGET(hooks_build_defination ALL
                  COMMAND "${PERL_EXECUTABLE}" "${HPMHOOK_GEN_DEFINATION}"
				  WORKING_DIRECTORY ${HPMHOOK_GEN_DIRECTORY}
				  COMMENT "Regenerating HPM Hook definitions..."
				  DEPENDS hooks_build_doxygen
                  VERBATIM)

ADD_CUSTOM_TARGET(hooks ALL
				  COMMAND "${PERL_EXECUTABLE}" "${HPMHOOK_GEN_DATACHECK}"
				  WORKING_DIRECTORY ${HPMHOOK_GEN_DIRECTORY}
				  COMMENT "Regenerating HPM Data Check definitions..."
				  DEPENDS hooks_build_defination
                  VERBATIM)