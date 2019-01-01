//
//  Copyright (C) 2015-2019 CASM Organization <https://casm-lang.org>
//  All rights reserved.
//
//  Developed by: Philipp Paulweber
//                <https://github.com/casm-lang/libcjel-be>
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

#include "CjelIRToIEC61131SLPass.h"

#include <libcjel-ir/analyze/CjelIRDumpPass>

#include <libpass/PassRegistry>

using namespace libcjel_ir;
using namespace libcjel_be;

char CjelIRToIEC61131SLPass::id = 0;

static libpass::PassRegistration< CjelIRToIEC61131SLPass > PASS(
    "CjelToPLL-IEC-61131-3-SLPass",
    "generates structural language code (IEC 61131-3 SL) out of the CJEL IR",
    "el2iec61131-3-sl",
    0 );

static FILE* stream = stderr;

static Module* module = 0;

bool CjelIRToIEC61131SLPass::run( libpass::PassResult& pr )
{
    // const auto data = pr.result< libcjel_ir::CjelIRDumpPass >();
    // const auto module = data->module();

    // std::string fn = "obj/" + std::string( module->name() ) + ".iec61131.sl";
    // // stream = fopen( fn.c_str(), "w" );

    // module->iterate( Traversal::PREORDER, this );

    // // if( fclose( stream ) )
    // // {
    // //     fprintf( stderr, "error: unable to close file stream\n" );
    // // }

    return false;
}

//
// Module
//

void CjelIRToIEC61131SLPass::visit_prolog( Module& value, Context& c )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( Module& value, Context& )
{
}

//
// Function
//

void CjelIRToIEC61131SLPass::visit_prolog( Function& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( Function& value, Context& )
{
}

//
// Intrinsic
//

void CjelIRToIEC61131SLPass::visit_prolog( Intrinsic& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_interlog( Intrinsic& value, Context& c )
{
}
void CjelIRToIEC61131SLPass::visit_epilog( Intrinsic& value, Context& c )
{
}

//
// Reference
//

void CjelIRToIEC61131SLPass::visit_prolog( Reference& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( Reference& value, Context& )
{
}

//
// Structure
//

void CjelIRToIEC61131SLPass::visit_prolog( Structure& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( Structure& value, Context& )
{
}

//
// Variable
//

void CjelIRToIEC61131SLPass::visit_prolog( Variable& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( Variable& value, Context& )
{
}

//
// Memory
//

void CjelIRToIEC61131SLPass::visit_prolog( libcjel_ir::Memory& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( libcjel_ir::Memory& value, Context& )
{
}

//
// ParallelScope
//

void CjelIRToIEC61131SLPass::visit_prolog( ParallelScope& value, Context& c )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( ParallelScope& value, Context& c )
{
}

//
// SequentialScope
//

void CjelIRToIEC61131SLPass::visit_prolog( SequentialScope& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( SequentialScope& value, Context& )
{
}

//
// TrivialStatement
//

void CjelIRToIEC61131SLPass::visit_prolog( TrivialStatement& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( TrivialStatement& value, Context& )
{
}

//
// BranchStatement
//

void CjelIRToIEC61131SLPass::visit_prolog( BranchStatement& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_interlog( BranchStatement& value, Context& )
{
}
void CjelIRToIEC61131SLPass::visit_epilog( BranchStatement& value, Context& )
{
}

//
// LoopStatement
//

void CjelIRToIEC61131SLPass::visit_prolog( LoopStatement& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_interlog( LoopStatement& value, Context& )
{
}
void CjelIRToIEC61131SLPass::visit_epilog( LoopStatement& value, Context& )
{
}

//
// CallInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( CallInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( CallInstruction& value, Context& )
{
}

//
// IdCallInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( IdCallInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( IdCallInstruction& value, Context& )
{
}

//
// StreamInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( StreamInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( StreamInstruction& value, Context& )
{
}

//
// NopInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( NopInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( NopInstruction& value, Context& )
{
}

//
// AllocInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( AllocInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( AllocInstruction& value, Context& )
{
}

//
// IdInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( IdInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( IdInstruction& value, Context& )
{
}

//
// CastInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( CastInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( CastInstruction& value, Context& )
{
}

//
// ExtractInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( ExtractInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( ExtractInstruction& value, Context& )
{
}

//
// LoadInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( LoadInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( LoadInstruction& value, Context& )
{
}

//
// StoreInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( StoreInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( StoreInstruction& value, Context& )
{
}

//
// NotInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( NotInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( NotInstruction& value, Context& )
{
}

//
// LnotInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( LnotInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( LnotInstruction& value, Context& )
{
}

//
// AndInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( AndInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( AndInstruction& value, Context& )
{
}

//
// OrInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( OrInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( OrInstruction& value, Context& )
{
}

//
// XorInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( XorInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( XorInstruction& value, Context& )
{
}

//
// AddUnsignedInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( AddUnsignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( AddUnsignedInstruction& value, Context& )
{
}

//
// AddSignedInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( AddSignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( AddSignedInstruction& value, Context& )
{
}

//
// DivSignedInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( DivSignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( DivSignedInstruction& value, Context& )
{
}

//
// ModUnsignedInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( ModUnsignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( ModUnsignedInstruction& value, Context& )
{
}

//
// EquInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( EquInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( EquInstruction& value, Context& )
{
}

//
// NeqInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( NeqInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( NeqInstruction& value, Context& )
{
}

//
// ZeroExtendInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( ZeroExtendInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( ZeroExtendInstruction& value, Context& )
{
}

//
// TruncationInstruction
//

void CjelIRToIEC61131SLPass::visit_prolog( TruncationInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( TruncationInstruction& value, Context& )
{
}

//
// BitConstant
//

void CjelIRToIEC61131SLPass::visit_prolog( BitConstant& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( BitConstant& value, Context& )
{
}

//
// StructureConstant
//

void CjelIRToIEC61131SLPass::visit_prolog( StructureConstant& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( StructureConstant& value, Context& )
{
}

//
// StringConstant
//

void CjelIRToIEC61131SLPass::visit_prolog( StringConstant& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( StringConstant& value, Context& )
{
}

//
// Interconnect
//

void CjelIRToIEC61131SLPass::visit_prolog( Interconnect& value, Context& )
{
    assert( 0 );
}
void CjelIRToIEC61131SLPass::visit_epilog( Interconnect& value, Context& )
{
}

//
//  Local variables:
//  mode: c++
//  indent-tabs-mode: nil
//  c-basic-offset: 4
//  tab-width: 4
//  End:
//  vim:noexpandtab:sw=4:ts=4:
//
