#
#   Copyright (C) 2015-2022 CASM Organization <https://casm-lang.org>
#   All rights reserved.
#
#   Developed by: Philipp Paulweber et al.
#                 <https://github.com/casm-lang/libcjel-be/graphs/contributors>
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

# LIBCJEL_BE_FOUND        - system has found the package
# LIBCJEL_BE_INCLUDE_DIRS - the package include directories
# LIBCJEL_BE_LIBRARY      - the package library

include( LibPackage )

libfind_pkg_check_modules( LIBCJEL_BE_PKGCONF libcjel-be )

find_path( LIBCJEL_BE_INCLUDE_DIR
  NAMES libcjel-be/libcjel-be.h
  PATHS ${LIBCJEL_BE_PKGCONF_INCLUDE_DIRS}
)

find_library( LIBCJEL_BE_LIBRARY
  NAMES libcjel-be cjel-be
  PATHS ${LIBCJEL_BE_PKGCONF_LIBRARY_DIRS}
)

set( LIBCJEL_BE_PROCESS_INCLUDES LIBCJEL_BE_INCLUDE_DIR )
set( LIBCJEL_BE_PROCESS_LIBS     LIBCJEL_BE_LIBRARY )

libfind_process( LIBCJEL_BE )

if( EXISTS "${LIBCJEL_BE_INCLUDE_DIR}" AND
    EXISTS "${LIBCJEL_BE_LIBRARY}" AND
    ${LIBCJEL_BE_INCLUDE_DIR} AND
    ${LIBCJEL_BE_LIBRARY}
    )
  set( LIBCJEL_BE_FOUND TRUE PARENT_SCOPE )
else()
  set( LIBCJEL_BE_FOUND FALSE PARENT_SCOPE )
endif()
