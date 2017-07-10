//
//  Copyright (c) 2015-2017 Philipp Paulweber
//  All rights reserved.
//
//  Developed by: Philipp Paulweber
//                https://github.com/casm-lang/libcjel-be
//
//  This file is part of libcjel-be.
//
//  libcjel-be is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  libcjel-be is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with libcjel-be. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _LIB_CJELBE_H_
#define _LIB_CJELBE_H_

#include "src/CjelBE.h"

#include "src/transform/CjelIRToC11Pass.h"
#include "src/transform/CjelIRToLLPass.h"
#include "src/transform/CjelIRToVHDLPass.h"

namespace libcjel_be
{
}

#define TODO                                                                   \
    fprintf( stdout, "%s:%i: '%s' implementation TODO\n", __FILE__, __LINE__,  \
        __FUNCTION__ )

#endif /* _LIB_CJELBE_H_ */

//
//  Local variables:
//  mode: c++
//  indent-tabs-mode: nil
//  c-basic-offset: 4
//  tab-width: 4
//  End:
//  vim:noexpandtab:sw=4:ts=4:
//
