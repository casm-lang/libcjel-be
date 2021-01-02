#
#   Copyright (C) 2015-2021 CASM Organization <https://casm-lang.org>
#   All rights reserved.
#
#   Developed by: Philipp Paulweber
#                 <https://github.com/casm-lang/libcjel-be>
#
#   This file is part of libcjel-be.
#
#   libcjel-be is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   libcjel-be is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with libcjel-be. If not, see <http://www.gnu.org/licenses/>.
#
#   Additional permission under GNU GPL version 3 section 7
#
#   libcjel-be is distributed under the terms of the GNU General Public License
#   with the following clarification and special exception: Linking libcjel-be
#   statically or dynamically with other modules is making a combined work
#   based on libcjel-be. Thus, the terms and conditions of the GNU General
#   Public License cover the whole combination. As a special exception,
#   the copyright holders of libcjel-be give you permission to link libcjel-be
#   with independent modules to produce an executable, regardless of the
#   license terms of these independent modules, and to copy and distribute
#   the resulting executable under terms of your choice, provided that you
#   also meet, for each linked independent module, the terms and conditions
#   of the license of that module. An independent module is a module which
#   is not derived from or based on libcjel-be. If you modify libcjel-be, you
#   may extend this exception to your version of the library, but you are
#   not obliged to do so. If you do not wish to do so, delete this exception
#   statement from your version.
#

include_directories(
  ${LIBSTDHL_INCLUDE_DIR}
  ${LIBPASS_INCLUDE_DIR}
  ${LIBCJEL_IR_INCLUDE_DIR}
)

add_library( ${PROJECT}-cpp OBJECT
  transform/CjelIRToC11Pass.cpp
  transform/CjelIRToIEC61131SLPass.cpp
  transform/CjelIRToIEC61499Pass.cpp
  transform/CjelIRToLLPass.cpp
  transform/CjelIRToVHDLPass.cpp
)


configure_file(
  Version.in.h
  ${PROJECT}/Version.h
)
configure_file(
  Version.in
  ${PROJECT}/Version
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}/Version.h
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}/Version
  DESTINATION
    "include/${PROJECT}"
)


ecm_generate_headers( ${PROJECT}_HEADERS_CPP
  ORIGINAL
    CAMELCASE
  HEADER_NAMES
    CjelBE
    libcjel-be
  PREFIX
    ${PROJECT}
  REQUIRED_HEADERS
    ${PROJECT}_HEADERS
)
install(
  FILES
    ${${PROJECT}_HEADERS}
    ${${PROJECT}_HEADERS_CPP}
  DESTINATION
    "include/${PROJECT}"
)


# ecm_generate_headers( ${PROJECT}_ANALYZE_HEADERS_CPP
#   ORIGINAL
#     CAMELCASE
#   HEADER_NAMES
#     TODO
#   PREFIX
#     ${PROJECT}/analyze
#   RELATIVE
#     analyze
#   REQUIRED_HEADERS
#     ${PROJECT}_ANALYZE_HEADERS
# )
# install(
#   FILES
#     ${${PROJECT}_ANALYZE_HEADERS}
#     ${${PROJECT}_ANALYZE_HEADERS_CPP}
#   DESTINATION
#     "include/${PROJECT}/analyze"
# )


# ecm_generate_headers( ${PROJECT}_EXECUTE_HEADERS_CPP
#   ORIGINAL
#     CAMELCASE
#   HEADER_NAMES
#     TODO
#   PREFIX
#     ${PROJECT}/execute
#   RELATIVE
#     execute
#   REQUIRED_HEADERS
#     ${PROJECT}_EXECUTE_HEADERS
# )
# install(
#   FILES
#     ${${PROJECT}_EXECUTE_HEADERS}
#     ${${PROJECT}_EXECUTE_HEADERS_CPP}
#   DESTINATION
#     "include/${PROJECT}/execute"
# )


ecm_generate_headers( ${PROJECT}_TRANSFORM_HEADERS_CPP
  ORIGINAL
    CAMELCASE
  HEADER_NAMES
    CjelIRToC11Pass
    CjelIRToIEC61131SLPass
    CjelIRToIEC61499Pass
    CjelIRToLLPass
    CjelIRToVHDLPass
  PREFIX
    ${PROJECT}/transform
  RELATIVE
    transform
  REQUIRED_HEADERS
    ${PROJECT}_TRANSFORM_HEADERS
)
install(
  FILES
    ${${PROJECT}_TRANSFORM_HEADERS}
    ${${PROJECT}_TRANSFORM_HEADERS_CPP}
  DESTINATION
    "include/${PROJECT}/transform"
)
