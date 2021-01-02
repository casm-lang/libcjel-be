//
//  Copyright (C) 2015-2021 CASM Organization <https://casm-lang.org>
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

#include "CjelIRToLLPass.h"

#include <libcjel-ir/analyze/CjelIRDumpPass>

#include <libpass/PassRegistry>

using namespace libcjel_ir;
using namespace libcjel_be;

char CjelIRToLLPass::id = 0;

static libpass::PassRegistration< CjelIRToLLPass > PASS(
    "CJEL IR to LLVM IR", "generates LLVM IR code out of the CJEL IR", "el2ll", 0 );

bool CjelIRToLLPass::run( libpass::PassResult& pr )
{
    // const auto data = pr.result< libcjel_ir::CjelIRDumpPass >();
    // const auto module = data->module();

    // std::string fn = "obj/" + std::string( module->name() ) + ".vhd";
    // // stream = fopen( fn.c_str(), "w" );

    // module->iterate( Traversal::PREORDER, this );

    // // if( fclose( stream ) )
    // // {
    // //     fprintf( stderr, "error: unable to close file stream\n" );
    // // }

    return false;
}

void CjelIRToLLPass::visit_prolog( Module& value, Context& )
{
    // fprintf( stdout,
    //     "; ModuleID = '%s'\n"
    //     ";; begin of module: '%s'\n"
    //     "\n",
    //     value.name(), value.name() );
}
void CjelIRToLLPass::visit_epilog( Module& value, Context& )
{
    // fprintf( stdout,
    //     ";; end of module: '%s'\n"
    //     "\n",
    //     value.name() );
}

void CjelIRToLLPass::visit_prolog( Function& value, Context& )
{
    // fprintf( stdout,
    //     "define void @%s ;; Function\n"
    //     "( ",
    //     value.name() );
}
void CjelIRToLLPass::visit_interlog( Function& value, Context& )
{
    // fprintf( stdout,
    //     "\n)\n"
    //     "{\n"
    //     "begin:\n" );
}
void CjelIRToLLPass::visit_epilog( Function& value, Context& )
{
    // fprintf( stdout,
    //     "  ret void\n"
    //     "}\n"
    //     "\n" );
}

void CjelIRToLLPass::visit_prolog( Intrinsic& value, Context& )
{
    // fprintf( stdout,
    //     "define void @%s ;; Intrinsic\n"
    //     "( ",
    //     value.name() );
}
void CjelIRToLLPass::visit_interlog( Intrinsic& value, Context& )
{
    // fprintf( stdout, "\n)\n{\nbegin:\n" );
}
void CjelIRToLLPass::visit_epilog( Intrinsic& value, Context& )
{
    // fprintf( stdout,
    //     "  ret void\n"
    //     "}\n"
    //     "\n" );
}

void CjelIRToLLPass::visit_prolog( Reference& value, Context& )
{
    // fprintf( stdout, "%s %%%s%s",
    //     "i32" // value.type().name() // TODO: FIXME!!!
    //     ,
    //     value.identifier()->name(),
    //     ( value.callableUnit()->isLastParameter( &value ) ? "" : "\n, " ) );
}
void CjelIRToLLPass::visit_epilog( Reference& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( Structure& value, Context& )
{
    // if( value.elements().size() == 0 )
    // {
    //     // all bit types can be represented in LLVM IR directly!
    //     return;
    // }

    // fprintf( stdout,
    //     ";; begin of structure: '%s'\n"
    //     "%%%s = type\n"
    //     "{ ",
    //     value.identifier()->name(), value.identifier()->name() );

    // u16 cnt = 0;
    // for( const Structure* s : value.elements() )
    // {
    //     cnt++;

    //     fprintf( stdout, "%s%s ;; %s\n%s", typeString( *( (Value*)s ) ),
    //         s->elements().size() > 0 ? "*" : "", s->identifier()->name(),
    //         cnt < value.elements().size() ? ", " : "" );
    // }

    // fprintf( stdout,
    //     "}\n"
    //     ";; end of structure: '%s'\n"
    //     "\n",
    //     value.identifier()->name() );
}
void CjelIRToLLPass::visit_epilog( Structure& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( Variable& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( Variable& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( libcjel_ir::Memory& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( libcjel_ir::Memory& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( ParallelScope& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( ParallelScope& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( SequentialScope& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( SequentialScope& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( TrivialStatement& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( TrivialStatement& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( BranchStatement& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_interlog( BranchStatement& value, Context& )
{
}
void CjelIRToLLPass::visit_epilog( BranchStatement& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( LoopStatement& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_interlog( LoopStatement& value, Context& )
{
}
void CjelIRToLLPass::visit_epilog( LoopStatement& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( CallInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( CallInstruction& value, Context& )
{
}

//
// IdCallInstruction
//

void CjelIRToLLPass::visit_prolog( IdCallInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( IdCallInstruction& value, Context& )
{
}

//
// StreamInstruction
//

void CjelIRToLLPass::visit_prolog( StreamInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( StreamInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( NopInstruction& value, Context& )
{
    assert( 0 );
    // fprintf( stream, "    ;; nop\n" );
}
void CjelIRToLLPass::visit_epilog( NopInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( AllocInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( AllocInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( IdInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( IdInstruction& value, Context& )
{
}

//
// CastInstruction
//

void CjelIRToLLPass::visit_prolog( CastInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( CastInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( ExtractInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( ExtractInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( LoadInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( LoadInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( StoreInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( StoreInstruction& value, Context& )
{
}

//
// NotInstruction
//

void CjelIRToLLPass::visit_prolog( NotInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( NotInstruction& value, Context& )
{
}

//
// LnotInstruction
//

void CjelIRToLLPass::visit_prolog( LnotInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( LnotInstruction& value, Context& )
{
}

//
// AndInstruction
//

void CjelIRToLLPass::visit_prolog( AndInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( AndInstruction& value, Context& )
{
}

//
// OrInstruction
//

void CjelIRToLLPass::visit_prolog( OrInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( OrInstruction& value, Context& )
{
}

//
// XorInstruction
//

void CjelIRToLLPass::visit_prolog( XorInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( XorInstruction& value, Context& )
{
}

//
// AddUnsignedInstruction
//

void CjelIRToLLPass::visit_prolog( AddUnsignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( AddUnsignedInstruction& value, Context& )
{
}

//
// AddSignedInstruction
//

void CjelIRToLLPass::visit_prolog( AddSignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( AddSignedInstruction& value, Context& )
{
}

//
// DivSignedInstruction
//

void CjelIRToLLPass::visit_prolog( DivSignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( DivSignedInstruction& value, Context& )
{
}

//
// ModUnsignedInstruction
//

void CjelIRToLLPass::visit_prolog( ModUnsignedInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( ModUnsignedInstruction& value, Context& )
{
}

//
// EquInstruction
//

void CjelIRToLLPass::visit_prolog( EquInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( EquInstruction& value, Context& )
{
}

//
// NeqInstruction
//

void CjelIRToLLPass::visit_prolog( NeqInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( NeqInstruction& value, Context& )
{
}

//
// ZeroExtendInstruction
//

void CjelIRToLLPass::visit_prolog( ZeroExtendInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( ZeroExtendInstruction& value, Context& )
{
}

//
// TruncationInstruction
//

void CjelIRToLLPass::visit_prolog( TruncationInstruction& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( TruncationInstruction& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( BitConstant& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( BitConstant& value, Context& )
{
}

void CjelIRToLLPass::visit_prolog( StructureConstant& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( StructureConstant& value, Context& )
{
}

//
// StringConstant
//

void CjelIRToLLPass::visit_prolog( StringConstant& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( StringConstant& value, Context& )
{
}

//
// Interconnect
//

void CjelIRToLLPass::visit_prolog( Interconnect& value, Context& )
{
    assert( 0 );
}
void CjelIRToLLPass::visit_epilog( Interconnect& value, Context& )
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
