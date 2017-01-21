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

#include "CselIRToC11Pass.h"

#include "../csel-ir/src/analyze/CselIRDumpPass.h"

using namespace libcsel_ir;
using namespace libcsel_be;

char CselIRToC11Pass::id = 0;

static libpass::PassRegistration< CselIRToC11Pass > PASS(
    "CSEL IR to C11", "generates C11 code out of the CSEL IR", "el2c11", 0 );

static FILE* stream = stderr;

static Module* module = 0;

bool CselIRToC11Pass::run( libpass::PassResult& pr )
{
    Module* value = (Module*)pr.getResult< libcsel_ir::CselIRDumpPass >();
    assert( value );
    module = value;

    std::string fn = "obj/" + std::string( value->getName() ) + ".c";
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

    if( type->getID() == Type::BIT )
    {
        u16 bitsize = type->getSize();
        u16 bitsize_type = 8;

        while( bitsize > bitsize_type )
        {
            bitsize_type *= 2;
        }

        assert( bitsize_type <= 64 );

        std::string t = "uint" + std::to_string( bitsize_type ) + "_t";
        return libstdhl::Allocator::string( t );
    }
    else if( type->getID() == Type::STRUCTURE )
    {
        // Value* ty = type->getBound();
        // assert( isa< Structure >( ty ) );
        // std::string t = std::string( ( (Structure*)ty )->getName() );
        // return libstdhl::Allocator::string( t );
        assert( !" PPA: TODO!!! " );
        return 0;
    }
    else if( type->getID() == Type::VECTOR )
    {
        // Value* ty = type->getBound();
        // assert( isa< Memory >( ty ) );
        // std::string t
        //     = std::string( ( (Memory*)ty )->getStructure()->getName() ) +
        //     "*";
        // return libstdhl::Allocator::string( t );
        assert( !" PPA: TODO!!! " );
        return 0;
    }
    else if( type->getID() == Type::INTERCONNECT )
    {
        return libstdhl::Allocator::string( "uint64_t**" );
    }
    else if( type->getID() == Type::STRING )
    {
        return libstdhl::Allocator::string( "char*" );
    }
    else
    {
        assert( !"unimplemented or unsupported type to convert!" );
    }
}

static const char* indention( Value& value )
{
    std::string ind = "";
    u8 cnt = 0;
    Value* p = ( &value );
    while( p != 0 )
    {
        if( isa< Block >( p ) )
        {
            p = (Value*)( (Block*)p )->getParent();
        }
        else if( isa< Instruction >( p ) )
        {
            p = (Value*)( (Instruction*)p )->getStatement();
        }
        else
        {
            assert( 0 );
        }

        if( isa< CallableUnit >( p ) )
        {
            break;
        }

        cnt++;
        ind += "    ";
    }

    return libstdhl::Allocator::string( ind );
}

//
// Module
//

void CselIRToC11Pass::visit_prolog( Module& value, Context& )
{
    std::chrono::time_point< std::chrono::system_clock > stamp;
    stamp = std::chrono::system_clock::now();
    std::time_t timestamp = std::chrono::system_clock::to_time_t( stamp );

    fprintf( stream,
        "// Generated At: %s"
        "// Module: '%s'\n"
        "\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <stdint.h>\n"
        "#include <assert.h>\n"
        "\n",
        std::ctime( &timestamp ), value.getName() );
}
void CselIRToC11Pass::visit_epilog( Module& value, Context& )
{
}

//
// Function
//

void CselIRToC11Pass::visit_prolog( Function& value, Context& )
{
    fprintf( stream,
        "// Function '%s'\n"
        "%s %s\n"
        "( ",
        value.getLabel(),
        ( strcmp( value.getName(), "main" ) == 0 ? "int" : "void" ),
        value.getName() );

    if( strcmp( value.getName(), "main" ) == 0 )
    {
        fprintf( stream,
            "int    argc\n"
            ", char** argv%s",
            ( value.getParameterLength() == 0 ? "" : "\n, " ) );
    }
}
void CselIRToC11Pass::visit_interlog( Function& value, Context& )
{
    fprintf( stream,
        "\n"
        ")\n" );

    if( value.getLinkage().size() > 0 )
    {
        fprintf( stream, "{ // linkage\n" );

        for( Value* linkage : value.getLinkage() )
        {
            assert( isa< Reference >( linkage ) );
            Reference* ref = (Reference*)linkage;

            Value* origin = ref->getRef< Variable >();
            if( origin )
            {
                fprintf( stream,
                    "%s  %s_var = { 0 }; // linkage '%s'\n"
                    "%s* %s = &%s_var; // \n",
                    getTypeString( *ref ), ref->getLabel(),
                    ref->getIdentifier()->getName(), getTypeString( *ref ),
                    ref->getLabel(), ref->getLabel()
                    //, origin->getLabel()
                    );
                // fprintf
                // ( stream
                // , "%s* %s = &%s; // linkage '%s'\n"
                // , getTypeString( *ref )
                // , ref->getLabel()
                // , origin->getLabel()
                // , ref->getIdentifier()->getName()
                // );
            }
            origin = ref->getRef< Memory >();
            if( origin )
            {
                // Memory* mem = (Memory*)origin;

                // const char* mem_type = getTypeString( *mem );
                // std::string tmp = std::string( mem_type );
                // tmp[ tmp.size() - 1 ] = '\0';

                assert( !" PPA: TODO!!! " );

                // fprintf( stream,
                //     "%s %s = malloc( sizeof( %s ) * %u ); // linkage '%s'\n"
                //     "assert( %s );\n",
                //     mem_type, ref->getLabel(), tmp.c_str(), mem->getSize(),
                //     ref->getIdentifier()->getName(), ref->getLabel() );

                // // fprintf
                // // ( stream
                // // , "%s = malloc( sizeof( %s ) * %u ); // linkage '%s'\n"
                // //   "assert( %s );\n"
                // //   "%s* %s = %s;\n"
                // // , mem->getLabel()
                // // , getTypeString( *mem )
                // // , mem->getSize()
                // // , ref->getIdentifier()->getName()
                // // , mem->getLabel()
                // // , getTypeString( *mem )
                // // , ref->getLabel()
                // // , mem->getLabel()
                // // );
            }
            origin = ref->getRef< Interconnect >();
            if( origin )
            {
                assert( !" PPA: TODO!!! " );

                // Interconnect* ic = (Interconnect*)origin;

                // // TODO: FIXME: PPA: HACK: needs better implemented directly
                // in
                // // the CSELBE model
                // Module* m = value.getRef< Module >();
                // assert( m and m->has< Variable >() );

                // fprintf( stream,
                //     "uint64_t* %s[] = // interconnect '%s'\n"
                //     "{ ",
                //     ref->getLabel(), ic->getLabel() );
                // for( Value* v : m->get< Variable >() )
                // {
                //     fprintf( stream, "(uint64_t*)%s // '%s'\n%s",
                //         v->getRef< Reference >()->getLabel(), v->getLabel(),
                //         m->get< Variable >().back() == v ? "" : ", " );
                // }
                // fprintf( stream, "};\n"
                //     //"uint64_t*** %s = (uint64_t***)&%s_var;\n"
                //     // , ref->getLabel()
                //     // , ref->getLabel()
                //     );
            }
            // assert( origin and " internal error! " );
        }
    }
}
void CselIRToC11Pass::visit_epilog( Function& value, Context& )
{
    if( value.getLinkage().size() > 0 )
    {
        for( Value* linkage : value.getLinkage() )
        {
            assert( isa< Reference >( linkage ) );
            Reference* ref = (Reference*)linkage;

            Value* origin = ref->getRef< Memory >();
            if( origin )
            {
                fprintf( stream, "free( %s ); // linkage '%s'\n",
                    ref->getLabel(), ref->getIdentifier()->getName() );
            }
        }

        fprintf( stream, "}\n" );
    }

    fprintf( stream, "\n" );
}

//
// Intrinsic
//

void CselIRToC11Pass::visit_prolog( Intrinsic& value, Context& )
{
    fprintf( stream,
        "// Intrinsic '%s'\n"
        "static inline void %s\n"
        "( ",
        value.getLabel(), value.getName() );

    assert( value.getLinkage().size() == 0 );
}
void CselIRToC11Pass::visit_interlog( Intrinsic& value, Context& c )
{
    visit_interlog( reinterpret_cast< Function& >( value ), c );
}
void CselIRToC11Pass::visit_epilog( Intrinsic& value, Context& c )
{
    visit_epilog( reinterpret_cast< Function& >( value ), c );
}

//
// Reference
//

void CselIRToC11Pass::visit_prolog( Reference& value, Context& )
{
    const char* kind = "link";
    if( value.isInput() )
    {
        kind = "in";
    }
    else if( value.isOutput() )
    {
        kind = "out";
    }
    else
    {
        assert( 0 );
    }

    fprintf( stream, "%s%s %s // %s %s%s", getTypeString( value ),
        ( ( value.getType()->getID() == Type::STRUCTURE or value.isOutput() )
                ? "*"
                : "" ),
        value.getLabel(), value.getIdentifier()->getName(), kind,
        ( value.getCallableUnit()->isLastParameter( &value ) ? "" : "\n, " ) );
}
void CselIRToC11Pass::visit_epilog( Reference& value, Context& )
{
}

//
// Structure
//

void CselIRToC11Pass::visit_prolog( Structure& value, Context& )
{
    Module* m = value.getRef< Module >();

    if( m->get< Structure >().front() == &value )
    {
        fprintf( stream, "// Structures\n" );
    }

    fprintf( stream,
        "typedef struct %s_t\n"
        "{ ",
        value.getIdentifier()->getName() );

    for( const Structure* s : value.getElements() )
    {
        fprintf( stream, "%s%s %s\n; ", getTypeString( *( (Value*)s ) ),
            s->getElements().size() > 0 ? "*" : "",
            s->getIdentifier()->getName() );
    }

    fprintf( stream,
        "}\n"
        "%s;\n"
        "\n",
        getTypeString( value ) );
}
void CselIRToC11Pass::visit_epilog( Structure& value, Context& )
{
}

//
// Variable
//

void CselIRToC11Pass::visit_prolog( Variable& value, Context& )
{
    static Value n( "", Type::getTypeID(), libcsel_ir::Value::VALUE );

    Module* m = value.getRef< Module >();

    if( m->get< Variable >().front() == &value )
    {
        fprintf( stream, "// Variables\n" );
    }

    // static u64 var_allocation = 0;

    fprintf( stream, "const %s %s = %lu; // '%s'\n", getTypeString( n ),
        value.getLabel(), value.getAllocationID()->getValue(),
        value.getIdent() );

    // var_allocation++;

    // fprintf
    // ( stream
    // , "%s %s = { 0 }; // '%s'\n"
    // , getTypeString( *value.getType()->getBound() )
    // , value.getLabel()
    // , value.getIdent()
    // );
}
void CselIRToC11Pass::visit_epilog( Variable& value, Context& )
{
    Module* m = value.getRef< Module >();

    if( m->get< Variable >().back() == &value )
    {
        fprintf( stream, "\n" );
    }
}

//
// Memory
//

void CselIRToC11Pass::visit_prolog( Memory& value, Context& )
{
    Module* m = value.getRef< Module >();
    if( m->get< Memory >().front() == &value )
    {
        fprintf( stream, "// Memory\n" );
    }

    assert( !" PPA: TODO!!! " );

    // fprintf( stream, "//%s* %s = 0; // size = '%u'\n", getTypeString( value
    // ),
    //     value.getLabel(), value.getSize() );
}
void CselIRToC11Pass::visit_epilog( Memory& value, Context& )
{
    Module* m = value.getRef< Module >();
    if( m->get< Memory >().back() == &value )
    {
        fprintf( stream, "\n" );
    }
}

static void non_trivial_statement( Scope& value )
{
    const Value* parent = value.getParent();
    assert( parent );

    if( isa< BranchStatement >( parent ) )
    {
        BranchStatement* branch = (BranchStatement*)parent;

        Value* expr = (Value*)branch->getInstructions().back();
        assert( expr );
        // assert( isa< LogicalInstruction >( expr ) );
        assert( expr->getType()->getID() == Type::BIT
                && expr->getType()->getSize() == 1 );

        if( branch->getScopes().front() == &value )
        {
            fprintf(
                stream, "%sif( %s )\n", indention( value ), expr->getLabel() );
        }
        else if( branch->getScopes().back() == &value )
        {
            fprintf( stream, "%selse\n", indention( value ) );
        }
    }
}

//
// ParallelScope
//

void CselIRToC11Pass::visit_prolog( ParallelScope& value, Context& )
{
    non_trivial_statement( value );

    fprintf(
        stream, "%s{ // par '%s'\n", indention( value ), value.getLabel() );
}
void CselIRToC11Pass::visit_epilog( ParallelScope& value, Context& )
{
    fprintf( stream, "%s}\n", indention( value ) );
}

//
// SequentialScope
//

void CselIRToC11Pass::visit_prolog( SequentialScope& value, Context& )
{
    non_trivial_statement( value );

    fprintf(
        stream, "%s{ // seq '%s'\n", indention( value ), value.getLabel() );
}
void CselIRToC11Pass::visit_epilog( SequentialScope& value, Context& c )
{
    visit_epilog( reinterpret_cast< ParallelScope& >( value ), c );
}

//
// TrivialStatement
//

void CselIRToC11Pass::visit_prolog( TrivialStatement& value, Context& )
{
    fprintf( stream,
        "%s// stmt '%s'\n"
        "%s{\n",
        indention( value ), value.getLabel(), indention( value ) );
}
void CselIRToC11Pass::visit_epilog( TrivialStatement& value, Context& )
{
    fprintf( stream, "%s}\n", indention( value ) );
}

//
// BranchStatement
//

void CselIRToC11Pass::visit_prolog( BranchStatement& value, Context& )
{
    fprintf( stream,
        "%s// branch '%s'\n"
        "%s{\n",
        indention( value ), value.getLabel(), indention( value ) );
}
void CselIRToC11Pass::visit_interlog( BranchStatement& value, Context& )
{
}
void CselIRToC11Pass::visit_epilog( BranchStatement& value, Context& c )
{
    visit_epilog( reinterpret_cast< TrivialStatement& >( value ), c );
}

//
// LoopStatement
//

void CselIRToC11Pass::visit_prolog( LoopStatement& value, Context& )
{
    fprintf( stream,
        "%s// loop '%s'\n"
        "%swhile(1)\n"
        "%s{\n",
        indention( value ), value.getLabel(), indention( value ),
        indention( value ) );
}
void CselIRToC11Pass::visit_interlog( LoopStatement& value, Context& )
{
    Value* expr = (Value*)value.getInstructions().back();
    assert( expr );
    assert( isa< LogicalInstruction >( expr ) );

    fprintf( stream,
        "%s    if( !%s )\n"
        "%s    {\n"
        "%s        break;\n"
        "%s    }\n"
        "%s    \n",
        indention( value ), expr->getLabel(), indention( value ),
        indention( value ), indention( value ), indention( value ) );
}
void CselIRToC11Pass::visit_epilog( LoopStatement& value, Context& c )
{
    visit_epilog( reinterpret_cast< TrivialStatement& >( value ), c );
}

//
// CallInstruction
//

void CselIRToC11Pass::visit_prolog( CallInstruction& value, Context& )
{
    fprintf( stream, "%s%s( ", indention( value ),
        ( isa< CastInstruction >( value.getValue( 0 ) ) )
            ? value.getValue( 0 )->getLabel()
            : value.getValue( 0 )->getName() );

    CallableUnit* cu = 0;
    if( isa< CallableUnit >( value.getValue( 0 ) ) )
    {
        cu = (CallableUnit*)value.getValue( 0 );
    }
    else if( isa< CastInstruction >( value.getValue( 0 ) ) )
    {
        CastInstruction* ci = (CastInstruction*)value.getValue( 0 );
        assert( isa< CallableUnit >( ci->getLHS() ) );
        cu = (CallableUnit*)ci->getLHS();

        assert( !" DEPRECATED: DO NOT USE CastInstruction with CallInstuction ANY LONGER!!! " );
    }

    u8 cnt = 0;

    for( auto v : value.getValues() )
    {
        if( cnt == 0 )
        {
            cnt++;
            continue;
        }

        const char* kind = ")";
        if( isa< Instruction >( v ) and cnt > cu->getInParameters().size() )
        {
            kind = "*)&";
        }
        else if( isa< Instruction >( v )
                 and v->getType()->getID() == Type::STRUCTURE )
        {
            kind = "*)&";
        }
        else if( isa< Reference >( v )
                 and v->getType()->getID() == Type::STRUCTURE )
        {
            kind = "*)";
        }
        else if( isa< Constant >( v )
                 and v->getType()->getID() == Type::STRUCTURE )
        {
            kind = "*)&";
        }
        else
        {
            printf(
                "\33[07mwarning:\33[0m unhandled 'kind' of a argument for call "
                "instr!\n" );
        }

        // if( isa< Reference >( v ) )
        // {
        // 	kind = "*)";
        // }
        // else if( isa< Instruction >( v ) or isa< Constant >( v
        // ) )
        // {
        // 	if( v->getType()->getID() == Type::STRUCTURE ) // cnt <=
        // cu->getInParameters().size() )
        // 	{
        // 		kind = "*)&";
        // 	}
        // 	else
        // 	{
        // 		assert(0);
        // 	}
        // }
        // else
        // {
        // 	assert(0);
        // }

        fprintf( stream, "%s(%s%s%s", ( cnt > 1 ? ", " : "" ),
            getTypeString( *v ), kind, v->getLabel() );
        cnt++;
    }

    fprintf( stream, " ); // call %lu\n", value.getValues().size() - 1 );
}
void CselIRToC11Pass::visit_epilog( CallInstruction& value, Context& )
{
}

//
// IdCallInstruction
//

void CselIRToC11Pass::visit_prolog( IdCallInstruction& value, Context& )
{
    const char* indent = indention( value );

    fprintf( stream,
        "%sswitch( %s ) // idcall '%s'\n"
        "%s{\n",
        indent, value.getValue( 1 )->getLabel(), value.getLabel(), indent );

    // TODO: FIXME: HACK: PPA: !!! should be dynamically fetched through
    // callable signature!!!
    // TODO: FIXME: HACK: PPA: IDEA: create implementation directly in 'CselIR'
    // !!!

    assert( isa< CallableUnit >( value.getValue( 0 ) ) );
    CallableUnit* cs = (CallableUnit*)value.getValue( 0 );

    u8 cnt = 0;
    std::string args = "";

    for( auto v : value.getValues() )
    {
        cnt++;

        if( cnt < 3 )
        {
            continue;
        }

        const char* kind = ")";
        if( isa< Instruction >( v ) and cnt > cs->getInParameters().size() )
        {
            kind = "*)&";
        }
        else if( isa< Instruction >( v )
                 and v->getType()->getID() == Type::STRUCTURE )
        {
            kind = "*)&";
        }
        else if( isa< Reference >( v )
                 and v->getType()->getID() == Type::STRUCTURE )
        {
            kind = "*)";
        }
        else if( isa< Constant >( v )
                 and v->getType()->getID() == Type::STRUCTURE )
        {
            kind = "*)&";
        }
        else
        {
            printf(
                "\33[07mwarning:\33[0m unhandled 'kind' of a argument for call "
                "instr!\n" );
        }
        args += ( cnt > 3 ? ", " : "" );
        args += "(";
        args += getTypeString( *v );
        args += kind;
        args += v->getLabel();
    }

    Module* m = value.getRef< Module >();

    for( Value* v : m->get< Function >() )
    {
        assert( v and isa< CallableUnit >( v ) );
        CallableUnit* cu = (CallableUnit*)v;

        if( cu->getInParameters().size() != cs->getInParameters().size() )
        {
            continue;
        }
        if( cu->getOutParameters().size() != cs->getOutParameters().size() )
        {
            continue;
        }

        // TODO: FIXME: PPA: HACK: more checks here regarding the indirect ID
        // call argument types!!!

        fprintf( stream, "%s    case %lu: { %s( %s ); break; }\n", indent,
            cu->getAllocationID()->getValue(), cu->getName(), args.c_str() );
    }

    fprintf( stream,
        "%s    default: assert( 0 ); break;\n"
        "%s};\n",
        indent, indent );
}
void CselIRToC11Pass::visit_epilog( IdCallInstruction& value, Context& )
{
}

//
// StreamInstruction
//

static void replace(
    std::string& str, const std::string& search, const std::string& replace )
{
    size_t pos = 0;
    while( ( pos = str.find( search, pos ) ) != std::string::npos )
    {
        str.replace( pos, search.length(), replace );
        pos += replace.length();
    }
}

void CselIRToC11Pass::visit_prolog( StreamInstruction& value, Context& )
{
    assert( value.getChannel() == StreamInstruction::OUTPUT );

    const char* channel = "stdout";

    std::string fmt = "\"";
    std::string arg = "";
    for( Value* i : value.getValues() )
    {
        if( isa< Variable >( i ) )
        {
            fmt += "%p";
            arg += ", &" + arg += std::string( i->getLabel() );
        }
        else if( isa< StringConstant >( i ) )
        {
            StringConstant* c = (StringConstant*)i;
            std::string tmp = std::string( c->getValue() );
            replace( tmp, "\n", "\\n" );
            fmt += tmp;
        }

        else
        {
            assert( !" unimplemented Value to 'stream'! " );
        }
    }
    fmt += "\"";

    fprintf( stream, "%sfprintf( %s, %s%s );\n", indention( value ), channel,
        fmt.c_str(), arg.c_str() );
}
void CselIRToC11Pass::visit_epilog( StreamInstruction& value, Context& )
{
}

//
// NopInstruction
//

void CselIRToC11Pass::visit_prolog( NopInstruction& value, Context& )
{
    fprintf( stream, "%s// nop\n", indention( value ) );
}
void CselIRToC11Pass::visit_epilog( NopInstruction& value, Context& )
{
}

//
// AllocInstruction
//

void CselIRToC11Pass::visit_prolog( AllocInstruction& value, Context& )
{
    fprintf( stream, "%s%s %s;// alloc\n", indention( value ),
        getTypeString( value ), value.getLabel() );
}
void CselIRToC11Pass::visit_epilog( AllocInstruction& value, Context& )
{
}

//
// IdInstruction
//

void CselIRToC11Pass::visit_prolog( IdInstruction& value, Context& )
{
    const char* id = value.get()->getLabel();

    if( isa< CallableUnit >( value.get() ) )
    {
        CallableUnit* c = (CallableUnit*)value.get();
        u64 id_num = c->getAllocationID()->getValue();
        id = libstdhl::Allocator::string( std::to_string( id_num ) );
    }

    fprintf( stream, "%s%s %s = (%s)%s;// id\n", indention( value ),
        getTypeString( value ), value.getLabel(), getTypeString( value ), id );
}
void CselIRToC11Pass::visit_epilog( IdInstruction& value, Context& )
{
}

//
// CastInstruction
//

void CselIRToC11Pass::visit_prolog( CastInstruction& value, Context& )
{
    Value* kind = value.getLHS();
    Value* src = value.getRHS();

    if( isa< CallableUnit >( kind ) )
    {
        // TODO: FIXME: use 'kind' func signature to create better casts!
        fprintf( stream, "%svoid (*%s)() = (void (*)())%s;// cast Function\n",
            indention( value ), value.getLabel(), src->getLabel() );
    }
    else if( isa< Structure >( kind ) )
    {
        fprintf( stream
            //, "%s%s %s = *((%s*)%s%s);\n"
            ,
            "%s%s* %s = (%s*)(%s%s); // cast Structure\n", indention( value ),
            getTypeString( value ), value.getLabel(), getTypeString( value ),
            ( isa< Reference >( src )
                and src->getType()->getID() == Type::STRUCTURE )
                ? "*"
                : "",
            src->getLabel() );
    }
    else
    {
        assert( !" unsupported feature !!! " );
    }
}
void CselIRToC11Pass::visit_epilog( CastInstruction& value, Context& )
{
}

//
// ExtractInstruction
//

void CselIRToC11Pass::visit_prolog( ExtractInstruction& value, Context& )
{
    Value* base_ = value.getLHS();
    Value* offset_ = value.getRHS();

    if( isa< AllocInstruction >( base_ ) and isa< Structure >( offset_ ) )
    {
        Structure* offset = (Structure*)offset_;

        fprintf( stream,
            "%s%s* %s = &(%s.%s); // extract (T1) [instr+struct]\n",
            indention( value ), getTypeString( value ), value.getLabel(),
            base_->getLabel(), offset->getName() );
        return;
    }

    assert( isa< Reference >( base_ ) or isa< CastInstruction >( base_ ) );
    Reference* base = (Reference*)base_;

    if( isa< Structure >( offset_ ) )
    {
        Structure* offset = (Structure*)offset_;
        if( isa< Reference >( base_ ) )
        {
            assert( offset->getParent() == base->getStructure()
                    and " offset is not a element of base structure! " );
        }

        fprintf( stream, "%s%s* %s = &(%s->%s); // extract (T2) '%s'\n",
            indention( value ), getTypeString( value ), value.getLabel(),
            base->getLabel(), offset->getName(),
            base->getIdentifier()->getName() );
    }
    else if( isa< Reference >( offset_ ) )
    {
        Reference* offset = (Reference*)offset_;
        assert( base->getType()->getID() == Type::INTERCONNECT );

        fprintf( stream, "%s%s* %s = (%s*)(%s[%s]); // extract (T3) '%s'\n",
            indention( value ), getTypeString( value ), value.getLabel(),
            getTypeString( value ), base->getLabel(), offset->getLabel(),
            base->getIdentifier()->getName() );
    }
    else if( isa< Instruction >( offset_ ) )
    {
        fprintf( stream,
            "%svoid* %s = (void*)(&%s[%s]); // extract (T4) '%s'\n",
            indention( value ), value.getLabel(), base->getLabel(),
            offset_->getLabel(), base->getIdentifier()->getName() );
    }
    else
    {
        assert( !" unsupported/unimplemented feature! " );
    }
}
void CselIRToC11Pass::visit_epilog( ExtractInstruction& value, Context& )
{
}

//
// LoadInstruction
//

void CselIRToC11Pass::visit_prolog( LoadInstruction& value, Context& )
{
    Value* addr_ = value.get();

    if( isa< ExtractInstruction >( addr_ ) or isa< CastInstruction >( addr_ ) )
    {
        fprintf( stream, "%s%s %s = *%s; // load\n", indention( value ),
            getTypeString( value ), value.getLabel(), addr_->getLabel() );
    }
    else
    {
        assert( !"  unimplemented feature! " );
    }

    // assert( isa< ExtractInstruction >( value.get() ) );
    // ExtractInstruction* ext = (ExtractInstruction*)( value.get() );

    // assert( isa< Reference >( ext->getLHS() ) );
    // Reference* ref = (Reference*)( ext->getLHS() );

    // if( isa< Structure >( ext->getRHS() ) )
    // {
    // 	Structure* str = (Structure*)( ext->getRHS() );
    // 	assert( str->getParent() == ref->getStructure() );

    // 	fprintf
    //     ( stream
    //     , "%s%s %s = %s->%s; // load '%s'\n"
    //     , indention( value )
    //     , getTypeString( value )
    //     , value.getLabel()
    //     , ref->getLabel()
    //     , str->getName()
    //     , ref->getIdentifier()->getName()
    //     );
    // }
    // else if( isa< Reference >( ext->getRHS() ) )
    // {
    // 	Reference* off = (Reference*)( ext->getRHS() );

    // 	fprintf
    //     ( stream
    // 	, "%s(uint64_t*) %s = %s[%s]; // load '%s'\n" // TODO: FIXME: PPA: HACK:
    // !!!
    //     , indention( value )
    //     , value.getLabel()
    //     , ref->getLabel()
    //     , off->getLabel()
    //     , ref->getIdentifier()->getName()
    //     );
    // }
}
void CselIRToC11Pass::visit_epilog( LoadInstruction& value, Context& )
{
}

//
// StoreInstruction
//

void CselIRToC11Pass::visit_prolog( StoreInstruction& value, Context& )
{
    Value* dst = value.getRHS();
    Value* src = value.getLHS();

    if( isa< ExtractInstruction >( dst ) )
    {
        fprintf( stream, "%s*%s = %s%s; // store '%s'\n", indention( value ),
            dst->getLabel(), ( isa< ExtractInstruction >( src ) ? "*" : "" ),
            src->getLabel(), value.getLabel() );
    }
    else if( isa< Reference >( dst ) )
    {
        Reference* ref = (Reference*)( dst );
        assert( ref );

        fprintf( stream, "%s*%s = %s%s; // store '%s' ['%s']\n",
            indention( value ), ref->getLabel(),
            ( isa< CastInstruction >( src ) ? "*" : "" ), src->getLabel(),
            value.getLabel(), ref->getIdentifier()->getName() );
    }
    else
    {
        TODO;
        assert( !" unimplemented feature! " );
    }
}
void CselIRToC11Pass::visit_epilog( StoreInstruction& value, Context& )
{
}

static void instr( BinaryInstruction& value, const char* op )
{
    fprintf( stream, "%s%s %s = (%s %s %s);\n", indention( value ),
        getTypeString( value ), value.getLabel(), value.getLHS()->getLabel(),
        op, value.getRHS()->getLabel() );
}

//
// NotInstruction
//

void CselIRToC11Pass::visit_prolog( NotInstruction& value, Context& )
{
    fprintf( stream, "%s%s %s = (~ %s);\n", indention( value ),
        getTypeString( value ), value.getLabel(), value.get()->getLabel() );
}
void CselIRToC11Pass::visit_epilog( NotInstruction& value, Context& )
{
}

//
// AndInstruction
//

void CselIRToC11Pass::visit_prolog( AndInstruction& value, Context& )
{
    instr( value, "&" );
}
void CselIRToC11Pass::visit_epilog( AndInstruction& value, Context& )
{
}

//
// OrInstruction
//

void CselIRToC11Pass::visit_prolog( OrInstruction& value, Context& )
{
    TODO;
    assert( 0 );
}
void CselIRToC11Pass::visit_epilog( OrInstruction& value, Context& )
{
}

//
// XorInstruction
//

void CselIRToC11Pass::visit_prolog( XorInstruction& value, Context& )
{
    instr( value, "^" );
}
void CselIRToC11Pass::visit_epilog( XorInstruction& value, Context& )
{
}

//
// AddSignedInstruction
//

void CselIRToC11Pass::visit_prolog( AddSignedInstruction& value, Context& )
{
    instr( value, "+" );
}
void CselIRToC11Pass::visit_epilog( AddSignedInstruction& value, Context& )
{
}

//
// DivSignedInstruction
//

void CselIRToC11Pass::visit_prolog( DivSignedInstruction& value, Context& )
{
    instr( value, "/" );
}
void CselIRToC11Pass::visit_epilog( DivSignedInstruction& value, Context& )
{
}

//
// ModUnsignedInstruction
//

void CselIRToC11Pass::visit_prolog( ModUnsignedInstruction& value, Context& )
{
    instr( value, "%" );
}
void CselIRToC11Pass::visit_epilog( ModUnsignedInstruction& value, Context& )
{
}

//
// EquUnsignedInstruction
//

void CselIRToC11Pass::visit_prolog( EquUnsignedInstruction& value, Context& )
{
    instr( value, "==" );
}
void CselIRToC11Pass::visit_epilog( EquUnsignedInstruction& value, Context& )
{
}

//
// NeqUnsignedInstruction
//

void CselIRToC11Pass::visit_prolog( NeqUnsignedInstruction& value, Context& )
{
    instr( value, "!=" );
}
void CselIRToC11Pass::visit_epilog( NeqUnsignedInstruction& value, Context& )
{
}

static void cast( UnaryInstruction& value, const char* comment )
{
    fprintf( stream, "%s%s %s = (%s)%s; // %s\n", indention( value ),
        getTypeString( value ), value.getLabel(), getTypeString( value ),
        value.get()->getLabel(), comment );
}

//
// ZeroExtendInstruction
//

void CselIRToC11Pass::visit_prolog( ZeroExtendInstruction& value, Context& )
{
    if( not isa< ExtractInstruction >( value.get() ) )
    {
        cast( value, "zext" );
    }
    else
    {
        fprintf( stream, "%s%s* %s = %s; // %s\n", indention( value ),
            getTypeString( value ), value.getLabel(), value.get()->getLabel(),
            "zext" );
    }
}
void CselIRToC11Pass::visit_epilog( ZeroExtendInstruction& value, Context& )
{
}

//
// TruncationInstruction
//

void CselIRToC11Pass::visit_prolog( TruncationInstruction& value, Context& )
{
    // if( not isa< LoadInstruction >( value.get() ) )
    // {
    // 	cast( value, "trunc" );
    // }
    // else
    {
        fprintf( stream, "%s%s %s = %s; // %s\n", indention( value ),
            getTypeString( value ), value.getLabel(), value.get()->getLabel(),
            "trunc" );
    }
}
void CselIRToC11Pass::visit_epilog( TruncationInstruction& value, Context& )
{
}

//
// BitConstant
//

void CselIRToC11Pass::visit_prolog( BitConstant& value, Context& )
{
    if( module->get< Constant >().front() == &value )
    {
        fprintf( stream, "// Constant\n" );
    }

    assert( !" PPA: TODO!!! " );

    // StructureConstant* sc = 0;
    // if( value.isBound() )
    // {
    //     sc = value.getBound();
    //     u1 last = sc->getElements().back() == &value;

    //     fprintf( stream, "%lu%s", value.getValue()[ 0 ], last ? "" : ", " );
    // }
    // else
    // {
    //     fprintf( stream, "const %s %s = %lu;\n", getTypeString( value ),
    //         value.getLabel(), value.getValue()[ 0 ] );
    // }
}
void CselIRToC11Pass::visit_epilog( BitConstant& value, Context& )
{
    // if( not value.isBound() )
    // {
    //     if( module->get< Constant >().back() == &value )
    //     {
    //         fprintf( stream, "\n" );
    //     }
    // }
}

//
// StructureConstant
//

void CselIRToC11Pass::visit_prolog( StructureConstant& value, Context& )
{
    assert( !" PPA: TODO!!! " );
    // if( module->get< Constant >().front() == &value )
    // {
    //     fprintf( stream, "// Constant\n" );
    // }

    // fprintf(
    //     stream, "const %s %s = { ", getTypeString( value ), value.getLabel()
    //     );
}
void CselIRToC11Pass::visit_epilog( StructureConstant& value, Context& )
{
    assert( !" PPA: TODO!!! " );
    // fprintf( stream, " };\n" );

    // if( module->get< Constant >().back() == &value )
    // {
    //     fprintf( stream, "\n" );
    // }
}

//
// StringConstant
//

void CselIRToC11Pass::visit_prolog( StringConstant& value, Context& )
{
    assert( !" PPA: TODO!!! " );

    // StructureConstant* sc = 0;
    // if( value.isBound() )
    // {
    //     sc = value.getBound();
    //     u1 last = sc->getElements().back() == &value;

    //     fprintf( stream, "\"%s\"%s", value.getValue(), last ? "" : ", " );
    // }
    // else
    // {
    //     fprintf( stream, "const %s %s = \"%s\";\n", getTypeString( value ),
    //         value.getLabel(), value.getValue() );
    // }
}
void CselIRToC11Pass::visit_epilog( StringConstant& value, Context& )
{
}

//
// Interconnect
//

void CselIRToC11Pass::visit_prolog( Interconnect& value, Context& )
{
    // TODO: FIXME: PPA: different implementation in the future here!!!
}
void CselIRToC11Pass::visit_epilog( Interconnect& value, Context& )
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
