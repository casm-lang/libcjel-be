//
//  Copyright (c) 2015-2017 Philipp Paulweber
//  All rights reserved.
//
//  Developed by: Philipp Paulweber
//                https://github.com/casm-lang/libcsel-be
//
//  This file is part of libcsel-be.
//
//  libcsel-be is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  libcsel-be is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with libcsel-be. If not, see <http://www.gnu.org/licenses/>.
//

#include "CselIRToLLPass.h"

#include "../csel-ir/src/analyze/CselIRDumpPass.h"

using namespace libcsel_ir;
using namespace libcsel_be;

char CselIRToLLPass::id = 0;

static libpass::PassRegistration< CselIRToLLPass > PASS( "CSEL IR to LLVM IR",
    "generates LLVM IR code out of the CSEL IR", "el2ll", 0 );

static FILE* stream = stderr;

bool CselIRToLLPass::run( libpass::PassResult& pr )
{
    Module* value = (Module*)pr.result< libcsel_ir::CselIRDumpPass >();
    assert( value );

    std::string fn = "obj/" + std::string( value->name() ) + ".vhd";
    stream = fopen( fn.c_str(), "w" );

    value->iterate( Traversal::PREORDER, this );

    if( fclose( stream ) )
    {
        fprintf( stderr, "error: unable to close file stream\n" );
    }

    return false;
}

static const char* typeString( Value& value )
{
    // Type* type = value.type();
    // assert( type );

    // if( type->getIDKind() == Type::ID::BIT )
    // {
    //     std::string t = "i" + std::to_string( type->getBitsize() );
    //     return t.c_str();
    // }
    // else if( type->getIDKind() == Type::ID::STRUCTURE )
    // {
    //     assert( isa< Structure >( &value ) );
    //     return ( (Structure*)&value )->identifier()->name();
    // }
    // else
    // {
    //     assert( !"unimplemented or unsupported type to convert!" );
    // }

    assert( !" PPA: TODO!!! " );
    return 0;
}

void CselIRToLLPass::visit_prolog( Module& value, Context& )
{
    fprintf( stdout,
        "; ModuleID = '%s'\n"
        ";; begin of module: '%s'\n"
        "\n",
        value.name(), value.name() );
}
void CselIRToLLPass::visit_epilog( Module& value, Context& )
{
    fprintf( stdout,
        ";; end of module: '%s'\n"
        "\n",
        value.name() );
}

void CselIRToLLPass::visit_prolog( Function& value, Context& )
{
    fprintf( stdout,
        "define void @%s ;; Function\n"
        "( ",
        value.name() );
}
void CselIRToLLPass::visit_interlog( Function& value, Context& )
{
    fprintf( stdout,
        "\n)\n"
        "{\n"
        "begin:\n" );
}
void CselIRToLLPass::visit_epilog( Function& value, Context& )
{
    fprintf( stdout,
        "  ret void\n"
        "}\n"
        "\n" );
}

void CselIRToLLPass::visit_prolog( Intrinsic& value, Context& )
{
    fprintf( stdout,
        "define void @%s ;; Intrinsic\n"
        "( ",
        value.name() );
}
void CselIRToLLPass::visit_interlog( Intrinsic& value, Context& )
{
    fprintf( stdout, "\n)\n{\nbegin:\n" );
}
void CselIRToLLPass::visit_epilog( Intrinsic& value, Context& )
{
    fprintf( stdout,
        "  ret void\n"
        "}\n"
        "\n" );
}

void CselIRToLLPass::visit_prolog( Reference& value, Context& )
{
    fprintf( stdout, "%s %%%s%s",
        "i32" // value.type().name() // TODO: FIXME!!!
        ,
        value.identifier()->name(),
        ( value.callableUnit()->isLastParameter( &value ) ? "" : "\n, " ) );
}
void CselIRToLLPass::visit_epilog( Reference& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( Structure& value, Context& )
{
    if( value.elements().size() == 0 )
    {
        // all bit types can be represented in LLVM IR directly!
        return;
    }

    fprintf( stdout,
        ";; begin of structure: '%s'\n"
        "%%%s = type\n"
        "{ ",
        value.identifier()->name(), value.identifier()->name() );

    u16 cnt = 0;
    for( const Structure* s : value.elements() )
    {
        cnt++;

        fprintf( stdout, "%s%s ;; %s\n%s", typeString( *( (Value*)s ) ),
            s->elements().size() > 0 ? "*" : "", s->identifier()->name(),
            cnt < value.elements().size() ? ", " : "" );
    }

    fprintf( stdout,
        "}\n"
        ";; end of structure: '%s'\n"
        "\n",
        value.identifier()->name() );
}
void CselIRToLLPass::visit_epilog( Structure& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( Variable& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( Variable& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( Memory& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( Memory& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( ParallelScope& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( ParallelScope& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( SequentialScope& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( SequentialScope& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( TrivialStatement& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( TrivialStatement& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( BranchStatement& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_interlog( BranchStatement& value, Context& )
{
}
void CselIRToLLPass::visit_epilog( BranchStatement& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( LoopStatement& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_interlog( LoopStatement& value, Context& )
{
}
void CselIRToLLPass::visit_epilog( LoopStatement& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( CallInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( CallInstruction& value, Context& )
{
}

//
// IdCallInstruction
//

void CselIRToLLPass::visit_prolog( IdCallInstruction& value, Context& )
{
    assert( 0 );
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( IdCallInstruction& value, Context& )
{
}

//
// StreamInstruction
//

void CselIRToLLPass::visit_prolog( StreamInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( StreamInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( NopInstruction& value, Context& )
{
    assert( 0 );
    fprintf( stream, "    ;; nop\n" );
}
void CselIRToLLPass::visit_epilog( NopInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( AllocInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( AllocInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( IdInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( IdInstruction& value, Context& )
{
}

//
// CastInstruction
//

void CselIRToLLPass::visit_prolog( CastInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( CastInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( ExtractInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( ExtractInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( LoadInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( LoadInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( StoreInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( StoreInstruction& value, Context& )
{
}

//
// NotInstruction
//

void CselIRToLLPass::visit_prolog( NotInstruction& value, Context& )
{
    assert( 0 );
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( NotInstruction& value, Context& )
{
}

//
// AndInstruction
//

void CselIRToLLPass::visit_prolog( AndInstruction& value, Context& )
{
    assert( 0 );
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( AndInstruction& value, Context& )
{
}

//
// OrInstruction
//

void CselIRToLLPass::visit_prolog( OrInstruction& value, Context& )
{
    assert( 0 );
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( OrInstruction& value, Context& )
{
}

//
// XorInstruction
//

void CselIRToLLPass::visit_prolog( XorInstruction& value, Context& )
{
    assert( 0 );
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( XorInstruction& value, Context& )
{
}

//
// AddUnsignedInstruction
//

void CselIRToLLPass::visit_prolog( AddUnsignedInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( AddUnsignedInstruction& value, Context& )
{
}

//
// AddSignedInstruction
//

void CselIRToLLPass::visit_prolog( AddSignedInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( AddSignedInstruction& value, Context& )
{
}

//
// DivSignedInstruction
//

void CselIRToLLPass::visit_prolog( DivSignedInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( DivSignedInstruction& value, Context& )
{
}

//
// ModUnsignedInstruction
//

void CselIRToLLPass::visit_prolog( ModUnsignedInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( ModUnsignedInstruction& value, Context& )
{
}

//
// EquInstruction
//

void CselIRToLLPass::visit_prolog( EquInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( EquInstruction& value, Context& )
{
}

//
// NeqInstruction
//

void CselIRToLLPass::visit_prolog( NeqInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( NeqInstruction& value, Context& )
{
}

//
// ZeroExtendInstruction
//

void CselIRToLLPass::visit_prolog( ZeroExtendInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( ZeroExtendInstruction& value, Context& )
{
}

//
// TruncationInstruction
//

void CselIRToLLPass::visit_prolog( TruncationInstruction& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( TruncationInstruction& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( BitConstant& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( BitConstant& value, Context& )
{
}

void CselIRToLLPass::visit_prolog( StructureConstant& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( StructureConstant& value, Context& )
{
}

//
// StringConstant
//

void CselIRToLLPass::visit_prolog( StringConstant& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( StringConstant& value, Context& )
{
}

//
// Interconnect
//

void CselIRToLLPass::visit_prolog( Interconnect& value, Context& )
{
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( Interconnect& value, Context& )
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
