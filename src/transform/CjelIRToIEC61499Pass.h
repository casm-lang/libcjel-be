//
//  Copyright (C) 2015-2023 CASM Organization <https://casm-lang.org>
//  All rights reserved.
//
//  Developed by: Philipp Paulweber et al.
//  <https://github.com/casm-lang/libcjel-be/graphs/contributors>
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
//  Additional permission under GNU GPL version 3 section 7
//
//  libcjel-be is distributed under the terms of the GNU General Public License
//  with the following clarification and special exception: Linking libcjel-be
//  statically or dynamically with other modules is making a combined work
//  based on libcjel-be. Thus, the terms and conditions of the GNU General
//  Public License cover the whole combination. As a special exception,
//  the copyright holders of libcjel-be give you permission to link libcjel-be
//  with independent modules to produce an executable, regardless of the
//  license terms of these independent modules, and to copy and distribute
//  the resulting executable under terms of your choice, provided that you
//  also meet, for each linked independent module, the terms and conditions
//  of the license of that module. An independent module is a module which
//  is not derived from or based on libcjel-be. If you modify libcjel-be, you
//  may extend this exception to your version of the library, but you are
//  not obliged to do so. If you do not wish to do so, delete this exception
//  statement from your version.
//

#ifndef _LIBCJEL_BE_TO_IEC61499_PASS_H_
#define _LIBCJEL_BE_TO_IEC61499_PASS_H_

#include <libpass/Pass>
#include <libpass/PassData>
#include <libpass/PassResult>

#include <libcjel-ir/Visitor>

/**

   https://en.wikipedia.org/wiki/IEC_61499

*/

namespace libcjel_be
{
    class CjelIRToIEC61499Pass final
    : public libpass::Pass
    , public libcjel_ir::Visitor
    {
      public:
        static char id;

        bool run( libpass::PassResult& pr ) override;

        LIBCJEL_IR_VISITOR_INTERFACE;
    };
}

#endif  // _LIBCJEL_BE_TO_IEC61499_PASS_H_

//
//  Local variables:
//  mode: c++
//  indent-tabs-mode: nil
//  c-basic-offset: 4
//  tab-width: 4
//  End:
//  vim:noexpandtab:sw=4:ts=4:
//
