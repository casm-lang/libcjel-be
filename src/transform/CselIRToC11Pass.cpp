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
    Module* value = (Module*)pr.result< libcsel_ir::CselIRDumpPass >();
    assert( value );
    module = value;

    std::string fn = "obj/" + std::string( value->name() ) + ".c";
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
    Type& type = value.type();

    if( type.id() == Type::BIT )
    {
        u16 bitsize = type.bitsize();
        u16 bitsize_type = 8;

        while( bitsize > bitsize_type )
        {
            bitsize_type *= 2;
        }

        assert( bitsize_type <= 64 );

        std::string t = "uint" + std::to_string( bitsize_type ) + "_t";
        return libstdhl::Allocator::string( t );
    }
    else if( type.id() == Type::STRUCTURE )
    {
        // Value* ty = type.getBound();
        // assert( isa< Structure >( ty ) );
        // std::string t = std::string( ( (Structure*)ty )->name() );
        // return libstdhl::Allocator::string( t );
        assert( !" PPA: TODO!!! " );
        return 0;
    }
    else if( type.id() == Type::VECTOR )
    {
        // Value* ty = type.getBound();
        // assert( isa< Memory >( ty ) );
        // std::string t
        //     = std::string( ( (Memory*)ty )->structure()->name() ) +
        //     "*";
        // return libstdhl::Allocator::string( t );
        assert( !" PPA: TODO!!! " );
        return 0;
    }
    else if( type.id() == Type::INTERCONNECT )
    {
        return libstdhl::Allocator::string( "uint64_t**" );
    }
    else if( type.id() == Type::STRING )
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
            p = (Value*)( (Block*)p )->parent();
        }
        else if( isa< Instruction >( p ) )
        {
            p = (Value*)( (Instruction*)p )->statement();
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
        std::ctime( &timestamp ), value.name() );
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
        value.label(),
        ( strcmp( value.name(), "main" ) == 0 ? "int" : "void" ),
        value.name() );

    if( strcmp( value.name(), "main" ) == 0 )
    {
        fprintf( stream,
            "int    argc\n"
            ", char** argv%s",
            ( value.parameterLength() == 0 ? "" : "\n, " ) );
    }
}
void CselIRToC11Pass::visit_interlog( Function& value, Context& )
{
    fprintf( stream,
        "\n"
        ")\n" );

    if( value.linkage().size() > 0 )
    {
        fprintf( stream, "{ // linkage\n" );

        for( Value* linkage : value.linkage() )
        {
            assert( isa< Reference >( linkage ) );
            Reference* ref = (Reference*)linkage;

            Value* origin = ref->ref< Variable >();
            if( origin )
            {
                fprintf( stream,
                    "%s  %s_var = { 0 }; // linkage '%s'\n"
                    "%s* %s = &%s_var; // \n",
                    typeString( *ref ), ref->label(), ref->identifier()->name(),
                    typeString( *ref ), ref->label(), ref->label()
                    //, origin->label()
                    );
                // fprintf
                // ( stream
                // , "%s* %s = &%s; // linkage '%s'\n"
                // , typeString( *ref )
                // , ref->label()
                // , origin->label()
                // , ref->identifier()->name()
                // );
            }
            origin = ref->ref< Memory >();
            if( origin )
            {
                // Memory* mem = (Memory*)origin;

                // const char* mem_type = typeString( *mem );
                // std::string tmp = std::string( mem_type );
                // tmp[ tmp.size() - 1 ] = '\0';

                assert( !" PPA: TODO!!! " );

                // fprintf( stream,
                //     "%s %s = malloc( sizeof( %s ) * %u ); // linkage '%s'\n"
                //     "assert( %s );\n",
                //     mem_type, ref->label(), tmp.c_str(), mem->bitsize(),
                //     ref->identifier()->name(), ref->label() );

                // // fprintf
                // // ( stream
                // // , "%s = malloc( sizeof( %s ) * %u ); // linkage '%s'\n"
                // //   "assert( %s );\n"
                // //   "%s* %s = %s;\n"
                // // , mem->label()
                // // , typeString( *mem )
                // // , mem->bitsize()
                // // , ref->identifier()->name()
                // // , mem->label()
                // // , typeString( *mem )
                // // , ref->label()
                // // , mem->label()
                // // );
            }
            origin = ref->ref< Interconnect >();
            if( origin )
            {
                assert( !" PPA: TODO!!! " );

                // Interconnect* ic = (Interconnect*)origin;

                // // TODO: FIXME: PPA: HACK: needs better implemented directly
                // in
                // // the CSELBE model
                // Module* m = value.ref< Module >();
                // assert( m and m->has< Variable >() );

                // fprintf( stream,
                //     "uint64_t* %s[] = // interconnect '%s'\n"
                //     "{ ",
                //     ref->label(), ic->label() );
                // for( Value* v : m->get< Variable >() )
                // {
                //     fprintf( stream, "(uint64_t*)%s // '%s'\n%s",
                //         v->ref< Reference >()->label(), v->label(),
                //         m->get< Variable >().back() == v ? "" : ", " );
                // }
                // fprintf( stream, "};\n"
                //     //"uint64_t*** %s = (uint64_t***)&%s_var;\n"
                //     // , ref->label()
                //     // , ref->label()
                //     );
            }
            // assert( origin and " internal error! " );
        }
    }
}
void CselIRToC11Pass::visit_epilog( Function& value, Context& )
{
    if( value.linkage().size() > 0 )
    {
        for( Value* linkage : value.linkage() )
        {
            assert( isa< Reference >( linkage ) );
            Reference* ref = (Reference*)linkage;

            Value* origin = ref->ref< Memory >();
            if( origin )
            {
                fprintf( stream, "free( %s ); // linkage '%s'\n", ref->label(),
                    ref->identifier()->name() );
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
        value.label(), value.name() );

    assert( value.linkage().size() == 0 );
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

    fprintf( stream, "%s%s %s // %s %s%s", typeString( value ),
        ( ( value.type().id() == Type::STRUCTURE or value.isOutput() ) ? "*"
                                                                       : "" ),
        value.label(), value.identifier()->name(), kind,
        ( value.callableUnit()->isLastParameter( &value ) ? "" : "\n, " ) );
}
void CselIRToC11Pass::visit_epilog( Reference& value, Context& )
{
}

//
// Structure
//

void CselIRToC11Pass::visit_prolog( Structure& value, Context& )
{
    Module* m = value.ref< Module >();

    if( m->get< Structure >().front() == &value )
    {
        fprintf( stream, "// Structures\n" );
    }

    fprintf( stream,
        "typedef struct %s_t\n"
        "{ ",
        value.identifier()->name() );

    for( const Structure* s : value.elements() )
    {
        fprintf( stream, "%s%s %s\n; ", typeString( *( (Value*)s ) ),
            s->elements().size() > 0 ? "*" : "", s->identifier()->name() );
    }

    fprintf( stream,
        "}\n"
        "%s;\n"
        "\n",
        typeString( value ) );
}
void CselIRToC11Pass::visit_epilog( Structure& value, Context& )
{
}

//
// Variable
//

void CselIRToC11Pass::visit_prolog( Variable& value, Context& )
{
    static Value n( "", Type::TypeID(), libcsel_ir::Value::VALUE );

    Module* m = value.ref< Module >();

    if( m->get< Variable >().front() == &value )
    {
        fprintf( stream, "// Variables\n" );
    }

    // static u64 var_allocation = 0;

    fprintf( stream, "const %s %s = %lu; // '%s'\n", typeString( n ),
        value.label(), value.allocId().value(), value.ident() );

    // var_allocation++;

    // fprintf
    // ( stream
    // , "%s %s = { 0 }; // '%s'\n"
    // , typeString( *value.type().getBound() )
    // , value.label()
    // , value.ident()
    // );
}
void CselIRToC11Pass::visit_epilog( Variable& value, Context& )
{
    Module* m = value.ref< Module >();

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
    Module* m = value.ref< Module >();
    if( m->get< Memory >().front() == &value )
    {
        fprintf( stream, "// Memory\n" );
    }

    assert( !" PPA: TODO!!! " );

    // fprintf( stream, "//%s* %s = 0; // size = '%u'\n", typeString( value
    // ),
    //     value.label(), value.bitsize() );
}
void CselIRToC11Pass::visit_epilog( Memory& value, Context& )
{
    Module* m = value.ref< Module >();
    if( m->get< Memory >().back() == &value )
    {
        fprintf( stream, "\n" );
    }
}

static void non_trivial_statement( Scope& value )
{
    const Value* parent = value.parent();
    assert( parent );

    if( isa< BranchStatement >( parent ) )
    {
        BranchStatement* branch = (BranchStatement*)parent;

        Value* expr = (Value*)branch->instructions().back();
        assert( expr );
        // assert( isa< LogicalInstruction >( expr ) );
        assert( expr->type().id() == Type::BIT && expr->type().bitsize() == 1 );

        if( branch->scopes().front() == &value )
        {
            fprintf(
                stream, "%sif( %s )\n", indention( value ), expr->label() );
        }
        else if( branch->scopes().back() == &value )
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

    fprintf( stream, "%s{ // par '%s'\n", indention( value ), value.label() );
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

    fprintf( stream, "%s{ // seq '%s'\n", indention( value ), value.label() );
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
        indention( value ), value.label(), indention( value ) );
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
        indention( value ), value.label(), indention( value ) );
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
        indention( value ), value.label(), indention( value ),
        indention( value ) );
}
void CselIRToC11Pass::visit_interlog( LoopStatement& value, Context& )
{
    Value* expr = (Value*)value.instructions().back();
    assert( expr );
    assert( isa< LogicalInstruction >( expr ) );

    fprintf( stream,
        "%s    if( !%s )\n"
        "%s    {\n"
        "%s        break;\n"
        "%s    }\n"
        "%s    \n",
        indention( value ), expr->label(), indention( value ),
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
        ( isa< CastInstruction >( value.value( 0 ) ) )
            ? value.value( 0 )->label()
            : value.value( 0 )->name() );

    CallableUnit* cu = 0;
    if( isa< CallableUnit >( value.value( 0 ) ) )
    {
        cu = (CallableUnit*)value.value( 0 );
    }
    else if( isa< CastInstruction >( value.value( 0 ) ) )
    {
        CastInstruction* ci = (CastInstruction*)value.value( 0 );
        assert( isa< CallableUnit >( ci->lhs() ) );
        cu = (CallableUnit*)ci->lhs();

        assert( !" DEPRECATED: DO NOT USE CastInstruction with CallInstuction ANY LONGER!!! " );
    }

    u8 cnt = 0;

    for( auto v : value.values() )
    {
        if( cnt == 0 )
        {
            cnt++;
            continue;
        }

        const char* kind = ")";
        if( isa< Instruction >( v ) and cnt > cu->inParameters().size() )
        {
            kind = "*)&";
        }
        else if( isa< Instruction >( v ) and v->type().id() == Type::STRUCTURE )
        {
            kind = "*)&";
        }
        else if( isa< Reference >( v ) and v->type().id() == Type::STRUCTURE )
        {
            kind = "*)";
        }
        else if( isa< Constant >( v ) and v->type().id() == Type::STRUCTURE )
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
        // 	if( v->type().id() == Type::STRUCTURE ) // cnt <=
        // cu->inParameters().size() )
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

        fprintf( stream, "%s(%s%s%s", ( cnt > 1 ? ", " : "" ), typeString( *v ),
            kind, v->label() );
        cnt++;
    }

    fprintf( stream, " ); // call %lu\n", value.values().size() - 1 );
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
        indent, value.value( 1 )->label(), value.label(), indent );

    // TODO: FIXME: HACK: PPA: !!! should be dynamically fetched through
    // callable signature!!!
    // TODO: FIXME: HACK: PPA: IDEA: create implementation directly in 'CselIR'
    // !!!

    assert( isa< CallableUnit >( value.value( 0 ) ) );
    CallableUnit* cs = (CallableUnit*)value.value( 0 );

    u8 cnt = 0;
    std::string args = "";

    for( auto v : value.values() )
    {
        cnt++;

        if( cnt < 3 )
        {
            continue;
        }

        const char* kind = ")";
        if( isa< Instruction >( v ) and cnt > cs->inParameters().size() )
        {
            kind = "*)&";
        }
        else if( isa< Instruction >( v ) and v->type().id() == Type::STRUCTURE )
        {
            kind = "*)&";
        }
        else if( isa< Reference >( v ) and v->type().id() == Type::STRUCTURE )
        {
            kind = "*)";
        }
        else if( isa< Constant >( v ) and v->type().id() == Type::STRUCTURE )
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
        args += typeString( *v );
        args += kind;
        args += v->label();
    }

    Module* m = value.ref< Module >();

    for( Value* v : m->get< Function >() )
    {
        assert( v and isa< CallableUnit >( v ) );
        CallableUnit* cu = (CallableUnit*)v;

        if( cu->inParameters().size() != cs->inParameters().size() )
        {
            continue;
        }
        if( cu->outParameters().size() != cs->outParameters().size() )
        {
            continue;
        }

        // TODO: FIXME: PPA: HACK: more checks here regarding the indirect ID
        // call argument types!!!

        fprintf( stream, "%s    case %lu: { %s( %s ); break; }\n", indent,
            cu->allocId().value(), cu->name(), args.c_str() );
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
    assert( value.channel() == StreamInstruction::OUTPUT );

    const char* channel = "stdout";

    std::string fmt = "\"";
    std::string arg = "";
    for( Value* i : value.values() )
    {
        if( isa< Variable >( i ) )
        {
            fmt += "%p";
            arg += ", &" + arg += std::string( i->label() );
        }
        else if( isa< StringConstant >( i ) )
        {
            StringConstant* c = (StringConstant*)i;
            std::string tmp = std::string( c->value() );
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
        typeString( value ), value.label() );
}
void CselIRToC11Pass::visit_epilog( AllocInstruction& value, Context& )
{
}

//
// IdInstruction
//

void CselIRToC11Pass::visit_prolog( IdInstruction& value, Context& )
{
    const char* id = value.get()->label();

    if( isa< CallableUnit >( value.get() ) )
    {
        CallableUnit* c = (CallableUnit*)value.get();
        u64 id_num = c->allocId().value();
        id = libstdhl::Allocator::string( std::to_string( id_num ) );
    }

    fprintf( stream, "%s%s %s = (%s)%s;// id\n", indention( value ),
        typeString( value ), value.label(), typeString( value ), id );
}
void CselIRToC11Pass::visit_epilog( IdInstruction& value, Context& )
{
}

//
// CastInstruction
//

void CselIRToC11Pass::visit_prolog( CastInstruction& value, Context& )
{
    Value* kind = value.lhs();
    Value* src = value.rhs();

    if( isa< CallableUnit >( kind ) )
    {
        // TODO: FIXME: use 'kind' func signature to create better casts!
        fprintf( stream, "%svoid (*%s)() = (void (*)())%s;// cast Function\n",
            indention( value ), value.label(), src->label() );
    }
    else if( isa< Structure >( kind ) )
    {
        fprintf( stream
            //, "%s%s %s = *((%s*)%s%s);\n"
            ,
            "%s%s* %s = (%s*)(%s%s); // cast Structure\n", indention( value ),
            typeString( value ), value.label(), typeString( value ),
            ( isa< Reference >( src ) and src->type().id() == Type::STRUCTURE )
                ? "*"
                : "",
            src->label() );
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
    Value* base_ = value.lhs();
    Value* offset_ = value.rhs();

    if( isa< AllocInstruction >( base_ ) and isa< Structure >( offset_ ) )
    {
        Structure* offset = (Structure*)offset_;

        fprintf( stream,
            "%s%s* %s = &(%s.%s); // extract (T1) [instr+struct]\n",
            indention( value ), typeString( value ), value.label(),
            base_->label(), offset->name() );
        return;
    }

    assert( isa< Reference >( base_ ) or isa< CastInstruction >( base_ ) );
    Reference* base = (Reference*)base_;

    if( isa< Structure >( offset_ ) )
    {
        Structure* offset = (Structure*)offset_;
        if( isa< Reference >( base_ ) )
        {
            assert( offset->parent() == base->structure()
                    and " offset is not a element of base structure! " );
        }

        fprintf( stream, "%s%s* %s = &(%s->%s); // extract (T2) '%s'\n",
            indention( value ), typeString( value ), value.label(),
            base->label(), offset->name(), base->identifier()->name() );
    }
    else if( isa< Reference >( offset_ ) )
    {
        Reference* offset = (Reference*)offset_;
        assert( base->type().id() == Type::INTERCONNECT );

        fprintf( stream, "%s%s* %s = (%s*)(%s[%s]); // extract (T3) '%s'\n",
            indention( value ), typeString( value ), value.label(),
            typeString( value ), base->label(), offset->label(),
            base->identifier()->name() );
    }
    else if( isa< Instruction >( offset_ ) )
    {
        fprintf( stream,
            "%svoid* %s = (void*)(&%s[%s]); // extract (T4) '%s'\n",
            indention( value ), value.label(), base->label(), offset_->label(),
            base->identifier()->name() );
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
            typeString( value ), value.label(), addr_->label() );
    }
    else
    {
        assert( !"  unimplemented feature! " );
    }

    // assert( isa< ExtractInstruction >( value.get() ) );
    // ExtractInstruction* ext = (ExtractInstruction*)( value.get() );

    // assert( isa< Reference >( ext->lhs() ) );
    // Reference* ref = (Reference*)( ext->lhs() );

    // if( isa< Structure >( ext->rhs() ) )
    // {
    // 	Structure* str = (Structure*)( ext->rhs() );
    // 	assert( str->parent() == ref->structure() );

    // 	fprintf
    //     ( stream
    //     , "%s%s %s = %s->%s; // load '%s'\n"
    //     , indention( value )
    //     , typeString( value )
    //     , value.label()
    //     , ref->label()
    //     , str->name()
    //     , ref->identifier()->name()
    //     );
    // }
    // else if( isa< Reference >( ext->rhs() ) )
    // {
    // 	Reference* off = (Reference*)( ext->rhs() );

    // 	fprintf
    //     ( stream
    // 	, "%s(uint64_t*) %s = %s[%s]; // load '%s'\n" // TODO: FIXME: PPA: HACK:
    // !!!
    //     , indention( value )
    //     , value.label()
    //     , ref->label()
    //     , off->label()
    //     , ref->identifier()->name()
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
    Value* dst = value.rhs();
    Value* src = value.lhs();

    if( isa< ExtractInstruction >( dst ) )
    {
        fprintf( stream, "%s*%s = %s%s; // store '%s'\n", indention( value ),
            dst->label(), ( isa< ExtractInstruction >( src ) ? "*" : "" ),
            src->label(), value.label() );
    }
    else if( isa< Reference >( dst ) )
    {
        Reference* ref = (Reference*)( dst );
        assert( ref );

        fprintf( stream, "%s*%s = %s%s; // store '%s' ['%s']\n",
            indention( value ), ref->label(),
            ( isa< CastInstruction >( src ) ? "*" : "" ), src->label(),
            value.label(), ref->identifier()->name() );
    }
    else
    {
        // TODO;
        assert( !" unimplemented feature! " );
    }
}
void CselIRToC11Pass::visit_epilog( StoreInstruction& value, Context& )
{
}

static void instr( Instruction& value, const char* op )
{
    fprintf( stream, "%s%s %s = (%s %s %s);\n", indention( value ),
        typeString( value ), value.label(), value.value( 0 )->label(), op,
        value.value( 1 )->label() );
}

//
// NotInstruction
//

void CselIRToC11Pass::visit_prolog( NotInstruction& value, Context& )
{
    fprintf( stream, "%s%s %s = (~ %s);\n", indention( value ),
        typeString( value ), value.label(), value.get()->label() );
}
void CselIRToC11Pass::visit_epilog( NotInstruction& value, Context& )
{
}

//
// LnotInstruction
//

void CselIRToC11Pass::visit_prolog( LnotInstruction& value, Context& )
{
    assert( !" TODO! " );
}
void CselIRToC11Pass::visit_epilog( LnotInstruction& value, Context& )
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
    // TODO;
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
// AddUnsignedInstruction
//

void CselIRToC11Pass::visit_prolog( AddUnsignedInstruction& value, Context& )
{
    // TODO;
    assert( 0 );
}
void CselIRToC11Pass::visit_epilog( AddUnsignedInstruction& value, Context& )
{
}

//
// AddSignedInstruction
//

void CselIRToC11Pass::visit_prolog( AddSignedInstruction& value, Context& )
{
    // TODO;
    assert( 0 );
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
// EquInstruction
//

void CselIRToC11Pass::visit_prolog( EquInstruction& value, Context& )
{
    instr( value, "==" );
}
void CselIRToC11Pass::visit_epilog( EquInstruction& value, Context& )
{
}

//
// NeqInstruction
//

void CselIRToC11Pass::visit_prolog( NeqInstruction& value, Context& )
{
    instr( value, "!=" );
}
void CselIRToC11Pass::visit_epilog( NeqInstruction& value, Context& )
{
}

static void cast( Instruction& value, const char* comment )
{
    fprintf( stream, "%s%s %s = (%s)%s; // %s\n", indention( value ),
        typeString( value ), value.label(), typeString( value ),
        value.value( 0 )->label(), comment );
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
            typeString( value ), value.label(), value.get()->label(), "zext" );
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
            typeString( value ), value.label(), value.get()->label(), "trunc" );
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
    //     u1 last = sc->elements().back() == &value;

    //     fprintf( stream, "%lu%s", value.value()[ 0 ], last ? "" : ", " );
    // }
    // else
    // {
    //     fprintf( stream, "const %s %s = %lu;\n", typeString( value ),
    //         value.label(), value.value()[ 0 ] );
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
    //     stream, "const %s %s = { ", typeString( value ), value.label()
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
    //     u1 last = sc->elements().back() == &value;

    //     fprintf( stream, "\"%s\"%s", value.value(), last ? "" : ", " );
    // }
    // else
    // {
    //     fprintf( stream, "const %s %s = \"%s\";\n", typeString( value ),
    //         value.label(), value.value() );
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
