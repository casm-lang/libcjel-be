//
//  Copyright (c) 2015-2016 Philipp Paulweber
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

using namespace libcsel_be;

char CselIRToLLPass::id = 0;

static libpass::PassRegistration< CselIRToLLPass > PASS( "CSEL IR to LLVM IR",
    "generates LLVM IR code out of the CSEL IR", "el2ll", 0 );

static FILE* stream = stderr;

bool CselIRToLLPass::run( libpass::PassResult& pr )
{
    Module* value = (Module*)pr.getResult< libcsel_ir::CselIRDumpPass >();
    assert( value );

    string fn = "obj/" + string( value->getName() ) + ".vhd";
    stream = fopen( fn.c_str(), "w" );

    value->iterate( Traversal::PREORDER, this );

    if( fclose( stream ) )
    {
        fprintf( stderr, "error: unable to close file stream\n" );
    }

    return false;
}

static const char* getTypeString( Value& value )
{
    Type* type = value.getType();
    assert( type );

    if( type->getIDKind() == Type::ID::BIT )
    {
        string t = "i" + to_string( type->getBitsize() );
        return t.c_str();
    }
    else if( type->getIDKind() == Type::ID::STRUCTURE )
    {
        assert( Value::isa< Structure >( &value ) );
        return ( (Structure*)&value )->getIdentifier()->getName();
    }
    else
    {
        assert( !"unimplemented or unsupported type to convert!" );
    }
}

void CselIRToLLPass::visit_prolog( Module& value )
{
    fprintf( stdout,
        "; ModuleID = '%s'\n"
        ";; begin of module: '%s'\n"
        "\n",
        value.getName(), value.getName() );
}
void CselIRToLLPass::visit_epilog( Module& value )
{
    fprintf( stdout,
        ";; end of module: '%s'\n"
        "\n",
        value.getName() );
}

void CselIRToLLPass::visit_prolog( Function& value )
{
    fprintf( stdout,
        "define void @%s ;; Function\n"
        "( ",
        value.getName() );
}
void CselIRToLLPass::visit_interlog( Function& value )
{
    fprintf( stdout,
        "\n)\n"
        "{\n"
        "begin:\n" );
}
void CselIRToLLPass::visit_epilog( Function& value )
{
    fprintf( stdout,
        "  ret void\n"
        "}\n"
        "\n" );
}

void CselIRToLLPass::visit_prolog( Intrinsic& value )
{
    fprintf( stdout,
        "define void @%s ;; Intrinsic\n"
        "( ",
        value.getName() );
}
void CselIRToLLPass::visit_interlog( Intrinsic& value )
{
    fprintf( stdout, "\n)\n{\nbegin:\n" );
}
void CselIRToLLPass::visit_epilog( Intrinsic& value )
{
    fprintf( stdout,
        "  ret void\n"
        "}\n"
        "\n" );
}

void CselIRToLLPass::visit_prolog( Reference& value )
{
    fprintf( stdout, "%s %%%s%s",
        "i32" // value.getType()->getName() // TODO: FIXME!!!
        ,
        value.getIdentifier()->getName(),
        ( value.getCallableUnit()->isLastParameter( &value ) ? "" : "\n, " ) );
}
void CselIRToLLPass::visit_epilog( Reference& value )
{
}

void CselIRToLLPass::visit_prolog( Structure& value )
{
    if( value.getElements().size() == 0 )
    {
        // all bit types can be represented in LLVM IR directly!
        return;
    }

    fprintf( stdout,
        ";; begin of structure: '%s'\n"
        "%%%s = type\n"
        "{ ",
        value.getIdentifier()->getName(), value.getIdentifier()->getName() );

    u16 cnt = 0;
    for( const Structure* s : value.getElements() )
    {
        cnt++;

        fprintf( stdout, "%s%s ;; %s\n%s", getTypeString( *( (Value*)s ) ),
            s->getElements().size() > 0 ? "*" : "",
            s->getIdentifier()->getName(),
            cnt < value.getElements().size() ? ", " : "" );
    }

    fprintf( stdout,
        "}\n"
        ";; end of structure: '%s'\n"
        "\n",
        value.getIdentifier()->getName() );
}
void CselIRToLLPass::visit_epilog( Structure& value )
{
}

void CselIRToLLPass::visit_prolog( Variable& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( Variable& value )
{
}

void CselIRToLLPass::visit_prolog( Memory& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( Memory& value )
{
}

void CselIRToLLPass::visit_prolog( ParallelScope& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( ParallelScope& value )
{
}

void CselIRToLLPass::visit_prolog( SequentialScope& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( SequentialScope& value )
{
}

void CselIRToLLPass::visit_prolog( TrivialStatement& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( TrivialStatement& value )
{
}

void CselIRToLLPass::visit_prolog( BranchStatement& value )
{
    TODO;
}
void CselIRToLLPass::visit_interlog( BranchStatement& value )
{
}
void CselIRToLLPass::visit_epilog( BranchStatement& value )
{
}

void CselIRToLLPass::visit_prolog( LoopStatement& value )
{
    TODO;
}
void CselIRToLLPass::visit_interlog( LoopStatement& value )
{
}
void CselIRToLLPass::visit_epilog( LoopStatement& value )
{
}

void CselIRToLLPass::visit_prolog( CallInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( CallInstruction& value )
{
}

//
// IdCallInstruction
//

void CselIRToLLPass::visit_prolog( IdCallInstruction& value )
{
    TODO;
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( IdCallInstruction& value )
{
}

//
// StreamInstruction
//

void CselIRToLLPass::visit_prolog( StreamInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( StreamInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( NopInstruction& value )
{
    TODO;
    fprintf( stream, "    ;; nop\n" );
}
void CselIRToLLPass::visit_epilog( NopInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( AllocInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( AllocInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( IdInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( IdInstruction& value )
{
}

//
// CastInstruction
//

void CselIRToLLPass::visit_prolog( CastInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( CastInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( ExtractInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( ExtractInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( LoadInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( LoadInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( StoreInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( StoreInstruction& value )
{
}

//
// NotInstruction
//

void CselIRToLLPass::visit_prolog( NotInstruction& value )
{
    TODO;
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( NotInstruction& value )
{
}

//
// AndInstruction
//

void CselIRToLLPass::visit_prolog( AndInstruction& value )
{
    TODO;
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( AndInstruction& value )
{
}

//
// OrInstruction
//

void CselIRToLLPass::visit_prolog( OrInstruction& value )
{
    TODO;
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( OrInstruction& value )
{
}

//
// XorInstruction
//

void CselIRToLLPass::visit_prolog( XorInstruction& value )
{
    TODO;
    assert( 0 );
}
void CselIRToLLPass::visit_epilog( XorInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( AddSignedInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( AddSignedInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( DivSignedInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( DivSignedInstruction& value )
{
}

//
// ModUnsignedInstruction
//

void CselIRToLLPass::visit_prolog( ModUnsignedInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( ModUnsignedInstruction& value )
{
}

//
// EquUnsignedInstruction
//

void CselIRToLLPass::visit_prolog( EquUnsignedInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( EquUnsignedInstruction& value )
{
}

//
// NeqUnsignedInstruction
//

void CselIRToLLPass::visit_prolog( NeqUnsignedInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( NeqUnsignedInstruction& value )
{
}

//
// ZeroExtendInstruction
//

void CselIRToLLPass::visit_prolog( ZeroExtendInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( ZeroExtendInstruction& value )
{
}

//
// TruncationInstruction
//

void CselIRToLLPass::visit_prolog( TruncationInstruction& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( TruncationInstruction& value )
{
}

void CselIRToLLPass::visit_prolog( BitConstant& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( BitConstant& value )
{
}

void CselIRToLLPass::visit_prolog( StructureConstant& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( StructureConstant& value )
{
}

//
// StringConstant
//

void CselIRToLLPass::visit_prolog( StringConstant& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( StringConstant& value )
{
}

//
// Interconnect
//

void CselIRToLLPass::visit_prolog( Interconnect& value )
{
    TODO;
}
void CselIRToLLPass::visit_epilog( Interconnect& value )
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
