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

#include "CselIRToVHDLPass.h"

#include "../csel-ir/src/analyze/CselIRDumpPass.h"

using namespace libcsel_ir;
using namespace libcsel_be;

char CselIRToVHDLPass::id = 0;

static libpass::PassRegistration< CselIRToVHDLPass > PASS(
    "CSEL IR to VHDL", "generates VDHL code out of the CSEL IR", "el2vhdl", 0 );

static FILE* stream = stderr;

static Module* module = 0;

static u1 instruction_implementation = false;

static void emit_conversion_functions( void );

bool CselIRToVHDLPass::run( libpass::PassResult& pr )
{
    Module* value = (Module*)pr.getResult< libcsel_ir::CselIRDumpPass >();
    assert( value );
    module = value;

    std::string fn = "obj/" + std::string( value->getName() ) + ".vhd";
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
    // Type* type = value.getType();
    // assert( type );

    // if( type->getID() == Type::BIT
    //     or isa< ExtractInstruction >( &value ) )
    // {
    //     if( type->getSize() == 1 )
    //     {
    //         return "std_logic";
    //     }
    //     else if( type->getSize() > 1 )
    //     {
    //         std::string t = "std_logic_vector( "
    //                         + std::to_string( type->getSize() - 1 )
    //                         + " downto 0 )";
    //         return libstdhl::Allocator::string( t );
    //     }
    //     else
    //     {
    //         if( type->getID() == Type::INTERCONNECT )
    //         {
    //             Value* bind = type->getBound();
    //             assert( isa< Interconnect >( bind ) );
    //             Interconnect* ict = (Interconnect*)bind;

    //             std::string t = "std_logic_vector( "
    //                             + std::to_string( ict->getSizeMax() - 1 )
    //                             + " downto 0 )";
    //             return libstdhl::Allocator::string( t );
    //         }

    //         assert( !"invalid type bit size" );
    //     }
    // }
    // else if( type->getID() == Type::ID::STRUCTURE )
    // {
    //     Value* ty = type->getBound();
    //     assert( isa< Structure >( ty ) );
    //     std::string t
    //         = "struct_" + std::string( ( (Structure*)ty )->getName() );
    //     return libstdhl::Allocator::string( t );
    // }
    // else if( type->getID() == Type::INTERCONNECT )
    // {
    //     assert( 0 );
    //     // return libstdhl::Allocator::string( "Interconnect" );
    // }
    // else if( type->getID() == Type::MEMORY )
    // {
    //     assert( 0 );
    //     // return libstdhl::Allocator::string( "Interconnect" );
    // }
    // else
    // {
    //     assert( !"unimplemented or unsupported type to convert!" );
    // }

    assert( !" PPA: TODO!!! " );
    return 0;
}

static void emit_statement_wires( Statement& value )
{
    for( Value* instr : value.getInstructions() )
    {
        if( isa< AllocInstruction >( instr ) )
        {
            fprintf( stream, "  signal     %s : %s; -- %s\n", instr->getLabel(),
                getTypeString( *instr ), instr->getName() );
        }
        else if( isa< IdInstruction >( instr ) )
        {
            IdInstruction* id_instr = (IdInstruction*)instr;
            Value* id_kind = id_instr->get();
            const char* id = 0;

            if( isa< CallableUnit >( id_kind ) )
            {
                std::bitset< 48 > v(
                    ( (CallableUnit*)id_kind )->getAllocationID()->getValue() );
                id = v.to_string().c_str();
            }
            else if( isa< Variable >( id_instr->get() ) )
            {
                std::bitset< 48 > v(
                    ( (Variable*)id_kind )->getAllocationID()->getValue() );
                id = v.to_string().c_str();
            }
            else
            {
                assert( !" unsupported feature! " );
            }

            fprintf( stream, "  constant   %s : %s := \"%s\"; -- %s\n",
                instr->getLabel(), getTypeString( *instr ), id,
                instr->getName() );
        }
        else if( isa< IdCallInstruction >( instr ) )
        {
            IdCallInstruction* idcall = (IdCallInstruction*)instr;
            assert( isa< CallableUnit >( idcall->getValue( 0 ) ) );
            CallableUnit* cs = (CallableUnit*)idcall->getValue( 0 );

            fprintf( stream, "  signal sig_%s : std_logic := '0'; -- %s\n",
                instr->getLabel(), instr->getName() );

            Module* m = idcall->getRef< Module >();

            for( Value* v : m->get< Function >() )
            {
                assert( v and isa< CallableUnit >( v ) );
                CallableUnit* cu = (CallableUnit*)v;

                // TODO: FIXME: PPA: HACK: more checks here regarding the
                // indirect ID call argument types!!!
                if( cu->getInParameters().size()
                    != cs->getInParameters().size() )
                {
                    continue;
                }
                if( cu->getOutParameters().size()
                    != cs->getOutParameters().size() )
                {
                    continue;
                }

                fprintf( stream,
                    "  signal sig_%s_req_%s : std_logic := '0'; -- '%s'\n"
                    "  signal sig_%s_ack_%s : std_logic := '0';\n",
                    instr->getLabel(), cu->getLabel(), cu->getName(),
                    instr->getLabel(), cu->getLabel() );
            }
        }
        else
        {
            fprintf( stream, "  signal sig_%s : std_logic := '0'; -- %s\n",
                instr->getLabel(), instr->getName() );

            if( not isa< NopInstruction >( instr )
                and not isa< CallInstruction >( instr )
                and not isa< IdCallInstruction >( instr )
                and not isa< StoreInstruction >( instr ) )
            {
                fprintf( stream, "  signal     %s : %s; -- %s\n",
                    instr->getLabel(), getTypeString( *instr ),
                    instr->getName() );
            }
        }
    }
}

static void emit_wire_req_ack( Value* context )
{
    fprintf( stream,
        "  signal req_%s : std_logic := '0'; -- '%s'\n"
        "  signal ack_%s : std_logic := '0';\n",
        context->getLabel(), context->getName(), context->getLabel() );

    if( isa< Statement >( context ) )
    {
        fprintf( stream, "  signal sig_%s : std_logic := '0';\n",
            context->getLabel() );

        emit_statement_wires( *( (Statement*)context ) );
    }

    if( isa< Scope >( context ) )
    {
        for( auto block : ( (Scope*)context )->getBlocks() )
        {
            emit_wire_req_ack( block );
        }
    }
    else if( isa< BranchStatement >( context ) )
    {
        for( auto inner_block : ( (BranchStatement*)context )->getScopes() )
        {
            emit_wire_req_ack( inner_block );
        }
    }
    else if( isa< LoopStatement >( context ) )
    {
        for( auto inner_block : ( (LoopStatement*)context )->getScopes() )
        {
            emit_wire_req_ack( inner_block );
        }
    }
}

static void emit_wire( Value& value )
{
    assert( isa< CallableUnit >( &value ) );
    CallableUnit* cunit = ( (CallableUnit*)( &value ) );

    for( auto link : cunit->getLinkage() )
    {
        Reference* linkage = (Reference*)link;

        if( linkage->getType()->getID() == Type::INTERCONNECT )
        {
            fprintf( stream,
                "signal ict_req_%s  : std_logic := '0'; -- interconnect '%s'\n"
                "signal ict_ack_%s  : std_logic := '0';\n"
                "signal ict_addr_%s : std_logic_vector( 47 downto 0 ) := ( "
                "others => '0' );\n"
                "signal ict_data_%s : std_logic_vector( 64 downto 0 ) := ( "
                "others => '0' );\n",
                linkage->getLabel(), linkage->getLabel(), linkage->getLabel(),
                linkage->getLabel(), linkage->getLabel()
                //, mem->getStructure()->getType()->getSize() - 1
                );

            continue;
        }
        // else if( linkage->getType()->getID() == Type::MEMORY )
        // {
        //     Value* bind = linkage->getType()->getBound();
        //     assert( isa< Memory >( bind ) );
        //     Memory* mem = (Memory*)bind;

        //     fprintf( stream,
        //         "signal mem_req_%s  : std_logic := '0'; -- memory '%s'\n"
        //         "signal mem_ack_%s  : std_logic := '0';\n"
        //         "signal mem_mode_%s : std_logic := '0';\n"
        //         "signal mem_addr_%s : std_logic_vector( 47 downto 0 ) := ( "
        //         "others => '0' );\n"
        //         "signal mem_data_%s : std_logic_vector( %u downto 0 ) := ( "
        //         "others => '0' );\n",
        //         linkage->getLabel(), linkage->getLabel(),
        //         linkage->getLabel(),
        //         linkage->getLabel(), linkage->getLabel(),
        //         linkage->getLabel(),
        //         mem->getStructure()->getType()->getSize() - 1 );

        //     continue;
        // }
        else
        {
            assert( !" PPA: TODO!!! " );
        }

        fprintf( stream,
            "  signal %s : %s := ( (others => '0'), '0' ); -- := %s; -- "
            "linkage '%s'\n",
            linkage->getLabel(), getTypeString( *linkage ),
            linkage->getRef< Variable >()->getExpression()->getLabel(),
            linkage->getIdentifier()->getName() );
    }

    Block* context = cunit->getContext();
    assert( context );

    if( isa< Scope >( context ) )
    {
        emit_wire_req_ack( context );
    }
}

static void emit_conversion_functions( void )
{
    fprintf( stream,
        "\n"
        "-- Instruction Implementations\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "package Instruction is\n"
        "  function to_slv( s : std_logic ) return std_logic_vector;\n"
        "  function to_slv( v : std_logic_vector ) return std_logic_vector;\n"
        "  function to_slb( v : in std_logic_vector) return std_logic;\n"
        "end;\n"
        "package body Instruction is\n"
        "  function to_slv( s : std_logic ) return std_logic_vector\n"
        "    is\n"
        "      variable v: std_logic_vector( 0 downto 0 );\n"
        "    begin\n"
        "      v(0) := s;\n"
        "      return v;\n"
        "  end;\n"
        "  function to_slv( v : std_logic_vector ) return std_logic_vector\n"
        "    is\n"
        "    begin\n"
        "      return v;\n"
        "  end;\n"
        "  function to_slb( v: in std_logic_vector ) return std_ulogic\n"
        "    is\n"
        "    begin\n"
        "      assert v'length = 1\n"
        "      report \"output port must be a 'std_logic'!\"\n"
        "      severity FAILURE;\n"
        "      return v(v'LEFT);\n"
        "  end;\n"
        "end;\n"
        "\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity handshake is\n"
        "  port\n"
        "  ( req      : in  std_logic\n"
        "  ; req_next : out std_logic\n"
        "  ; ack_next : in  std_logic\n"
        "  );\n"
        "end handshake;\n"
        "architecture \\@handshake@\\ of handshake is\n"
        "  signal sig_req      : std_logic := '0';\n"
        "  signal sig_req_next : std_logic := '0';\n"
        "  signal sig_ack_next : std_logic := '0';\n"
        "begin\n"
        "  sig_req      <=     req;\n"
        "      req_next <= sig_req_next;\n"
        "  sig_ack_next <=     ack_next;\n"
        "  \n"
        "  process( sig_req, sig_ack_next ) is\n"
        "  begin\n"
        "    if sig_req = '0' and sig_ack_next = '1' then\n"
        "      sig_req_next <= transport '0' after 25 ps;\n"
        "    else\n"
        "      if rising_edge( sig_req ) then\n"
        "        if sig_ack_next = '0' then\n"
        "          sig_req_next <= transport '1' after 25 ps;\n"
        "        else\n"
        "          sig_req_next <= transport '0' after 25 ps;\n"
        "        end if;\n"
        "      end if;\n"
        "    end if;\n"
        "  end process;\n"
        "end \\@handshake@\\;\n"
        "\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity handshake_branch is\n"
        "  port\n"
        "  ( req       : in  std_logic\n"
        "  ; ack       : out std_logic\n"
        "  ; req_true  : out std_logic\n"
        "  ; ack_true  : in  std_logic\n"
        "  ; req_false : out std_logic\n"
        "  ; ack_false : in  std_logic\n"
        "  ; condition : in  std_logic\n"
        "  );\n"
        "end handshake_branch;\n"
        "architecture \\@handshake_branch@\\ of handshake_branch is\n"
        "  signal sig_req       : std_logic := '0';\n"
        "  signal sig_ack       : std_logic := '0';\n"
        "  signal sig_req_true  : std_logic := '0';\n"
        "  signal sig_ack_true  : std_logic := '0';\n"
        "  signal sig_req_false : std_logic := '0';\n"
        "  signal sig_ack_false : std_logic := '0';\n"
        "begin\n"
        "  sig_req       <=     req;\n"
        "      ack       <= sig_ack;\n"
        "      req_true  <= sig_req_true;\n"
        "  sig_ack_true  <=     ack_true;\n"
        "      req_false <= sig_req_false;\n"
        "  sig_ack_false <=     ack_false;\n"
        "  \n"
        "  process( sig_req, sig_ack_true, sig_ack_false, condition ) is\n"
        "  begin\n"
        "    if sig_req = '0' and ( sig_ack_true = '1' or sig_ack_false = '1' "
        ") then\n"
        "      sig_req_true  <= transport '0' after 25 ps;\n"
        "      sig_req_false <= transport '0' after 25 ps;\n"
        "    else\n"
        "      if rising_edge( sig_req ) then\n"
        "        if condition = '1' then\n"
        "          if sig_ack_true = '0' then\n"
        "            sig_req_true <= transport '1' after 25 ps;\n"
        "          else\n"
        "            sig_req_true <= transport '0' after 25 ps;\n"
        "          end if;\n"
        "        else\n"
        "          if sig_ack_false = '0' then\n"
        "            sig_req_false <= transport '1' after 25 ps;\n"
        "          else\n"
        "            sig_req_false <= transport '0' after 25 ps;\n"
        "          end if;\n"
        "        end if;\n"
        "      end if;\n"
        "    end if;\n"
        "  end process;\n"
        "  sig_ack <= transport ( sig_ack_true or sig_ack_false ) after 25 "
        "ps;\n"
        "end \\@handshake_branch@\\;\n"
        "\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity handshake_branch_true is\n"
        "  port\n"
        "  ( req       : in  std_logic\n"
        "  ; ack       : out std_logic\n"
        "  ; req_true  : out std_logic\n"
        "  ; ack_true  : in  std_logic\n"
        "  ; condition : in  std_logic\n"
        "  );\n"
        "end handshake_branch_true;\n"
        "architecture \\@handshake_branch_true@\\ of handshake_branch_true is\n"
        "  signal sig_req       : std_logic := '0';\n"
        "  signal sig_ack       : std_logic := '0';\n"
        "  signal sig_req_true  : std_logic := '0';\n"
        "  signal sig_ack_true  : std_logic := '0';\n"
        "  signal sig_req_false : std_logic := '0';\n"
        "  signal sig_ack_false : std_logic := '0';\n"
        "begin\n"
        "  sig_req       <=     req;\n"
        "      ack       <= sig_ack;\n"
        "      req_true  <= sig_req_true;\n"
        "  sig_ack_true  <=     ack_true;\n"
        "  sig_ack_false <= sig_req_false;\n"
        "  \n"
        "  process( sig_req, sig_req_true, sig_ack_true, sig_ack_false, "
        "condition ) is\n"
        "  begin\n"
        "    if sig_req = '0' and ( sig_ack_true = '1' or sig_ack_false = '1' "
        ") then\n"
        "      sig_req_true  <= transport '0' after 25 ps;\n"
        "      sig_req_false <= transport '0' after 25 ps;\n"
        "    else\n"
        "      if rising_edge( sig_req ) then\n"
        "        if condition = '1' then\n"
        "          if sig_ack_true = '0' then\n"
        "            sig_req_true <= transport '1' after 25 ps;\n"
        "          else\n"
        "            sig_req_true <= transport '0' after 25 ps;\n"
        "          end if;\n"
        "        else\n"
        "          if sig_ack_false = '0' then\n"
        "            sig_req_false <= transport '1' after 25 ps;\n"
        "          else\n"
        "            sig_req_false <= transport '0' after 25 ps;\n"
        "          end if;\n"
        "        end if;\n"
        "      end if;\n"
        "    end if;\n"
        "  end process;\n"
        "  sig_ack <= transport ( sig_ack_true or sig_ack_false ) after 25 "
        "ps;\n"
        "end \\@handshake_branch_true@\\;\n"
        "\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity handshake_loop is\n"
        "  port\n"
        "  ( req       : in  std_logic\n"
        "  ; ack       : out std_logic\n"
        "  ; req_cond  : out std_logic\n"
        "  ; ack_cond  : in  std_logic\n"
        "  ; condition : in  std_logic\n"
        "  ; req_body  : out std_logic\n"
        "  ; ack_body  : in  std_logic\n"
        "  );\n"
        "end handshake_loop;\n"
        "architecture \\@handshake_loop@\\ of handshake_loop is\n"
        "  signal sig_req            : std_logic := '0';\n"
        "  signal sig_ack            : std_logic := '0';\n"
        "  signal sig_req_cond       : std_logic := '0';\n"
        "  signal sig_ack_cond       : std_logic := '0';\n"
        "  signal sig_req_body       : std_logic := '0';\n"
        "  signal sig_ack_body       : std_logic := '0';\n"
        "  signal sig_req_cond_init  : std_logic := '0';\n"
        "  signal sig_req_cond_body  : std_logic := '0';\n"
        "begin\n"
        "  sig_req      <=     req;\n"
        "      ack      <= sig_ack;\n"
        "      req_cond <= sig_req_cond;\n"
        "  sig_ack_cond <=     ack_cond;\n"
        "      req_body <= sig_req_body;\n"
        "  sig_ack_body <=     ack_body;\n"
        "  \n"
        "  process( sig_req, sig_req_body, sig_ack_body, condition ) is\n"
        "  begin\n"
        "    if sig_req_body = '1' then\n"
        "      sig_req_cond_init <= transport '0' after 25 ps;\n"
        "    else\n"
        "      if rising_edge( sig_req ) then\n"
        "	sig_req_cond_init <= transport '1' after 25 ps;\n"
        "      end if;\n"
        "    end if;\n"
        "  end process;\n"
        "  \n"
        "  sig_req_cond <= ( sig_req_cond_init or sig_req_cond_body ) and "
        "sig_req;\n"
        "  \n"
        "  process( sig_ack_cond, sig_req, sig_ack_body, condition ) is\n"
        "  begin\n"
        "    if sig_ack_body = '1' or sig_req = '0' then\n"
        "      sig_req_body <= transport '0' after 25 ps;\n"
        "      sig_ack      <= transport '0' after 25 ps;\n"
        "    else\n"
        "      if rising_edge( sig_ack_cond ) then\n"
        "	if condition = '1' then\n"
        "	  sig_req_body <= transport '1' after 25 ps;\n"
        "        else\n"
        "          sig_req_body <= transport '0' after 25 ps;\n"
        "          sig_ack      <= transport '1' after 25 ps;\n"
        "        end if;\n"
        "      end if;\n"
        "    end if;\n"
        "  end process;\n"
        "  \n"
        "  process( sig_ack_body, sig_req_body ) is\n"
        "  begin\n"
        "    if sig_req_body = '1' then\n"
        "      sig_req_cond_body <= transport '0' after 25 ps;\n"
        "    else\n"
        "      if falling_edge( sig_ack_body ) then\n"
        "	sig_req_cond_body <= transport '1' after 25 ps;\n"
        "      end if;\n"
        "    end if;\n"
        "  end process;\n"
        "end \\@handshake_loop@\\;\n" );
}

//
// Module
//

void CselIRToVHDLPass::visit_prolog( Module& value, Context& c )
{
    instruction_implementation = false;

    std::chrono::time_point< std::chrono::system_clock > stamp;
    stamp = std::chrono::system_clock::now();
    std::time_t timestamp = std::chrono::system_clock::to_time_t( stamp );

    fprintf( stream,
        "-- Generated At: %s"
        "-- Module: '%s'\n"
        "\n",
        std::ctime( &timestamp ), value.getName() );

    instruction_implementation = true;

    emit_conversion_functions();

    for( Value* v : ( *Value::getSymbols() )[ ".instruction" ] )
    {
        if( isa< IdInstruction >( v ) or isa< CallInstruction >( v )
            or isa< IdCallInstruction >( v ) )
        {
            continue;
        }
        this->dispatch( Visitor::Stage::PROLOG, *v, c );
    }

    // module->iterate( Traversal::PREORDER, [this]( Value& v ) {

    // } );

    instruction_implementation = false;
}
void CselIRToVHDLPass::visit_epilog( Module& value, Context& )
{
    fprintf( stream, "-- end of module: '%s'\n\n", value.getName() );
}

//
// Function
//

void CselIRToVHDLPass::visit_prolog( Function& value, Context& )
{
    fprintf( stream,
        "-- Function '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "use work.Structure.all;\n"
        "use work.Constants.all;\n"
        "use work.Variables.all;\n"
        "use work.Instruction.all;\n"
        "entity %s is port\n"
        "( req : in  std_logic\n"
        "; ack : out std_logic%s",
        value.getLabel(), value.getName(),
        value.getParameterLength() == 0 ? "" : "\n; " );
}
void CselIRToVHDLPass::visit_interlog( Function& value, Context& )
{
    std::string tmp( "\n;" );

    if( value.getInParameters().size() == 0
        and value.getOutParameters().size() == 0 )
    {
        tmp = "";
    }

    fprintf( stream,
        "\n"
        ");\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n",
        value.getName(), value.getName(), value.getName() );

    emit_wire( value );

    fprintf( stream, "begin\n" );

    for( auto link : value.getLinkage() )
    {
        Reference* linkage = (Reference*)link;

        if( linkage->getType()->getID() == Type::INTERCONNECT )
        {
            assert( !" PPA: TODO!!! " );

            // Value* bind = linkage->getType()->getBound();
            // assert( isa< Interconnect >( bind ) );
            // Interconnect* ict = (Interconnect*)bind;

            // fprintf( stream,
            //     "  ict_%s: entity work.%s port map( ict_req_%s, ict_ack_%s, "
            //     "ict_addr_%s, ict_data_%s",
            //     linkage->getLabel(), ict->getLabel(), linkage->getLabel(),
            //     linkage->getLabel(), linkage->getLabel(), linkage->getLabel()
            //     );

            // for( auto w : value.getLinkage() )
            // {
            //     if( w->getType()->getID() == Type::MEMORY
            //         or w->getType()->getID() == Type::INTERCONNECT )
            //     {
            //         continue;
            //     }
            //     fprintf( stream, ", %s", w->getLabel() );
            // }

            // fprintf( stream, " );\n" );
        }
        // else if( linkage->getType()->getID() == Type::MEMORY )
        // {
        //     Value* bind = linkage->getType()->getBound();
        //     assert( isa< Memory >( bind ) );
        //     Memory* mem = (Memory*)bind;

        //     fprintf( stream,
        //         "  mem_%s: entity work.memory generic map ( 48, 114 ) port "
        //         "map( mem_req_%s, mem_ack_%s, mem_mode_%s, mem_addr_%s, "
        //         "mem_data_%s );\n",
        //         linkage->getLabel(), linkage->getLabel(),
        //         linkage->getLabel(),
        //         linkage->getLabel(), linkage->getLabel(), linkage->getLabel()
        //         );
        // }
        else
        {
            assert( !" PPA: TODO!!! " );
        }
    }

    fprintf( stream,
        "  hscu_%s : entity work.handshake port map ( req, req_%s, ack_%s );\n"
        "  ack <= transport ack_%s after 25 ps;\n",
        value.getLabel(), value.getContext()->getLabel(),
        value.getContext()->getLabel(), value.getContext()->getLabel() );
}
void CselIRToVHDLPass::visit_epilog( Function& value, Context& )
{
    fprintf( stream,
        "end \\@%s@\\;\n"
        "\n",
        value.getName() );
}

//
// Intrinsic
//

void CselIRToVHDLPass::visit_prolog( Intrinsic& value, Context& )
{
    fprintf( stream,
        "-- Intrinsic '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "use work.Structure.all;\n"
        "use work.Constants.all;\n"
        "use work.Variables.all;\n"
        "use work.Instruction.all;\n"
        "entity %s is port\n"
        "( req : in  std_logic\n"
        "; ack : out std_logic%s",
        value.getLabel(), value.getName(),
        value.getParameterLength() == 0 ? "" : "\n; " );
}
void CselIRToVHDLPass::visit_interlog( Intrinsic& value, Context& c )
{
    visit_interlog( reinterpret_cast< Function& >( value ), c );
}
void CselIRToVHDLPass::visit_epilog( Intrinsic& value, Context& c )
{
    visit_epilog( reinterpret_cast< Function& >( value ), c );
}

//
// Reference
//

void CselIRToVHDLPass::visit_prolog( Reference& value, Context& )
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

    if( value.getType() and value.getType()->getID() == Type::INTERCONNECT )
    {
        fprintf( stream,
            "ict_req_%s  : out std_logic -- interconnect '%s'\n; "
            "ict_ack_%s  : in  std_logic\n; "
            "ict_addr_%s : out std_logic_vector( 47 downto 0 )\n; "
            "ict_data_%s : in  std_logic_vector( 64 downto 0 )%s",
            value.getLabel(), value.getLabel(), value.getLabel(),
            value.getLabel(), value.getLabel(),
            ( value.getCallableUnit()->isLastParameter( &value ) ? ""
                                                                 : "\n; " ) );
    }
    // else if( value.getType() and value.getType()->getID() == Type::MEMORY )
    // {
    //     Value* bind = value.getType()->getBound();
    //     assert( isa< Memory >( bind ) );
    //     Memory* mem = (Memory*)bind;

    //     fprintf( stream,
    //         "mem_req_%s  : out std_logic -- memory '%s'\n; "
    //         "mem_ack_%s  : in  std_logic\n; "
    //         "mem_mode_%s : out std_logic\n; "
    //         "mem_addr_%s : out std_logic_vector( 47 downto 0 )\n; "
    //         "mem_data_%s : inout std_logic_vector( %u downto 0 )%s",
    //         value.getLabel(), value.getLabel(), value.getLabel(),
    //         value.getLabel(), value.getLabel(), value.getLabel(),
    //         mem->getStructure()->getType()->getSize() - 1,
    //         ( value.getCallableUnit()->isLastParameter( &value ) ? ""
    //                                                              : "\n; " )
    //                                                              );
    // }
    else
    {
        fprintf( stream, "%s : %s %s -- %s %s%s", value.getLabel(), kind,
            getTypeString( value ), value.getIdentifier()->getName(), kind,
            ( value.getCallableUnit()->isLastParameter( &value ) ? ""
                                                                 : "\n; " ) );
    }
    assert( !" PPA: TODO!!! " );
}
void CselIRToVHDLPass::visit_epilog( Reference& value, Context& )
{
}

//
// Structure
//

void CselIRToVHDLPass::visit_prolog( Structure& value, Context& )
{
    Module* m = value.getRef< Module >();

    if( m->get< Structure >().front() == &value )
    {
        fprintf( stream,
            "-- Structures\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "package Structure is\n" );
    }

    fprintf( stream,
        "type %s is\n"
        "record\n",
        getTypeString( value ) );

    for( const Structure* s : value.getElements() )
    {
        fprintf( stream, "  %s : %s;\n", s->getIdentifier()->getName(),
            getTypeString( *( (Value*)s ) ) );
    }

    fprintf( stream, "end record;\n" );

    if( m->get< Structure >().back() == &value )
    {
        fprintf( stream, "end;\n" );
    }

    fprintf( stream, "\n" );
}
void CselIRToVHDLPass::visit_epilog( Structure& value, Context& )
{
}

//
// Variable
//

void CselIRToVHDLPass::visit_prolog( Variable& value, Context& )
{
    static Value n( "", Type::getTypeID(), libcsel_ir::Value::VALUE );

    Module* m = value.getRef< Module >();

    if( m->get< Variable >().front() == &value )
    {
        fprintf( stream,
            "-- Variables\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "use work.Structure.all;\n"
            "use work.Constants.all;\n"
            "package Variables is\n" );
    }

    std::bitset< 48 > v( value.getAllocationID()->getValue() );

    fprintf( stream, "  constant %s_id : %s := \"%s\";\n", value.getLabel(),
        getTypeString( n ), v.to_string().c_str() );
}
void CselIRToVHDLPass::visit_epilog( Variable& value, Context& )
{
    Module* m = value.getRef< Module >();

    if( m->get< Variable >().back() == &value )
    {
        fprintf( stream,
            "end;\n"
            "\n" );
    }
}

//
// Memory
//

void CselIRToVHDLPass::visit_prolog( Memory& value, Context& )
{
    static u1 used = false;
    assert( used == false );
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Memory '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "use work.Structure.all;\n"
        "use work.Constants.all;\n"
        "use work.Variables.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( ADDR_WIDTH : integer\n"
        "  ; DATA_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in     std_logic\n"
        "  ; ack : out    std_logic\n"
        "  ; mode : in    std_logic -- '0' read, '1' write\n"
        "  ; addr : in    std_logic_vector( ADDR_WIDTH-1 downto 0 )\n"
        "  ; data : inout std_logic_vector( DATA_WIDTH-1 downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "  type RAM is array( 0 to %u-1 ) of std_logic_vector( DATA_WIDTH-1 "
        "downto 0 );\n"
        "  signal mem : RAM := ( others => ( others => '0' ) );\n"
        "  signal data_out : std_logic_vector( DATA_WIDTH-1 downto 0 );\n"
        "begin\n"
        "  process( req ) is\n"
        "  begin\n"
        "    if rising_edge( req ) then\n"
        "      if( req = '1' and mode = '1' ) then\n"
        "        mem( to_integer(unsigned(addr)) ) <= data;\n"
        "      end if;\n"
        "      if( req = '1' and mode = '0' ) then\n"
        "        data_out <= mem( to_integer(unsigned(addr)) );\n"
        "      end if;\n"
        "      ack <= transport '1' after 50 ps;\n"
        "    end if;\n"
        "  end process;\n"
        "  \n"
        "  data <= data_out when ( req = '1' and mode = '0' ) else ( others => "
        "'Z' );\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, value.getLength(), name );
}
void CselIRToVHDLPass::visit_epilog( Memory& value, Context& )
{
}

//
// ParallelScope
//

void CselIRToVHDLPass::visit_prolog( ParallelScope& value, Context& c )
{
    visit_epilog( reinterpret_cast< SequentialScope& >( value ), c );
    return;

    fprintf( stream,
        "\n"
        "  -- par '%s' begin\n",
        value.getLabel() );

    for( auto block : value.getBlocks() )
    {
        fprintf( stream,
            "  para_%s : entity work.handshake port map ( req_%s, req_%s, "
            "ack_%s );\n",
            block->getLabel(), value.getLabel(), block->getLabel(),
            block->getLabel() );
    }

    std::string cond = "";

    for( auto block : value.getBlocks() )
    {
        if( value.getBlocks().front() != block )
        {
            cond += " and ";
        }
        cond += "ack_";
        cond += block->getLabel();
    }

    fprintf( stream, "  ack_%s <= transport ( %s ) after 25 ps;\n",
        value.getLabel(), cond.c_str() );
}
void CselIRToVHDLPass::visit_epilog( ParallelScope& value, Context& c )
{
    visit_epilog( reinterpret_cast< SequentialScope& >( value ), c );
    return;

    fprintf( stream, "  -- par '%s' end\n", value.getLabel() );
}

//
// SequentialScope
//

void CselIRToVHDLPass::visit_prolog( SequentialScope& value, Context& )
{
    fprintf( stream,
        "\n"
        "  -- seq '%s' begin\n",
        value.getLabel() );

    Value* last = &value;

    for( auto block : value.getBlocks() )
    {
        const char* last_kind = "ack";
        if( last == &value )
        {
            last_kind = "req";
        }

        fprintf( stream,
            "  sequ_%s : entity work.handshake port map ( %s_%s, req_%s, "
            "ack_%s );\n",
            block->getLabel(), last_kind, last->getLabel(), block->getLabel(),
            block->getLabel() );

        last = block;
    }

    fprintf( stream, "  ack_%s <= transport ack_%s after 25 ps;\n",
        value.getLabel(), value.getBlocks().back()->getLabel() );
}
void CselIRToVHDLPass::visit_epilog( SequentialScope& value, Context& )
{
    fprintf( stream, "  -- seq '%s' end\n", value.getLabel() );
}

//
// TrivialStatement
//

void CselIRToVHDLPass::visit_prolog( TrivialStatement& value, Context& )
{
    fprintf( stream,
        "\n"
        "  -- trivial statement '%s' begin\n",
        value.getLabel() );

    Value* first = value.getInstructions().front();
    while( isa< AllocInstruction >( first ) or isa< IdInstruction >( first ) )
    {
        first = first->getNext();
    }

    fprintf( stream, "  sig_%s <= transport req_%s after 25 ps;\n",
        first->getLabel(), value.getLabel() );
}
void CselIRToVHDLPass::visit_epilog( TrivialStatement& value, Context& )
{
    fprintf( stream,
        "  ack_%s <= transport sig_%s after 25 ps;\n"
        "  -- trivial statement '%s' end\n",
        value.getLabel(), value.getLabel(), value.getLabel() );
}

//
// BranchStatement
//

void CselIRToVHDLPass::visit_prolog( BranchStatement& value, Context& )
{
    fprintf( stream,
        "\n"
        "  -- branch statement '%s' begin\n",
        value.getLabel() );

    // emit_statement_wires( value );

    Value* first = value.getInstructions().front();
    while( isa< AllocInstruction >( first ) or isa< IdInstruction >( first ) )
    {
        first = first->getNext();
    }

    fprintf( stream, "  sig_%s <= transport req_%s after 25 ps;\n",
        first->getLabel(), value.getLabel() );
}
void CselIRToVHDLPass::visit_interlog( BranchStatement& value, Context& )
{
    Value* instr = value.getInstructions().back();
    assert( instr );

    if( value.getScopes().size() < 2 )
    {
        fprintf( stream,
            "    brat_%s : entity work.handshake_branch_true "
            "port map ( sig_%s, ack_%s, req_%s, ack_%s, %s );\n"
            "  -- branch statement '%s' branching;\n",
            value.getLabel(), value.getLabel(), value.getLabel(),
            value.getScopes()[ 0 ]->getLabel(),
            value.getScopes()[ 0 ]->getLabel(), instr->getLabel(),
            value.getLabel() );
    }
    else
    {
        fprintf( stream,
            "    bran_%s : entity work.handshake_branch "
            "port map ( sig_%s, ack_%s, req_%s, ack_%s, req_%s, ack_%s, %s );\n"
            "  -- branch statement '%s' branching;\n",
            value.getLabel(), value.getLabel(), value.getLabel(),
            value.getScopes()[ 0 ]->getLabel(),
            value.getScopes()[ 0 ]->getLabel(),
            value.getScopes()[ 1 ]->getLabel(),
            value.getScopes()[ 1 ]->getLabel(), instr->getLabel(),
            value.getLabel() );
    }
}
void CselIRToVHDLPass::visit_epilog( BranchStatement& value, Context& )
{
    fprintf( stream, "  -- branch statement '%s' end\n", value.getLabel() );
}

//
// LoopStatement
//

void CselIRToVHDLPass::visit_prolog( LoopStatement& value, Context& )
{
    fprintf( stream,
        "\n"
        "  -- loop statement '%s' begin\n",
        value.getLabel() );

    // emit_statement_wires( value );

    Value* first = value.getInstructions().front();
    while( isa< AllocInstruction >( first ) or isa< IdInstruction >( first ) )
    {
        first = first->getNext();
    }

    Value* instr = value.getInstructions().back();
    assert( instr );

    fprintf( stream,
        "    loop_%s : entity work.handshake_loop "
        "port map ( req_%s, ack_%s, sig_%s, sig_%s, %s, req_%s, ack_%s );\n",
        value.getLabel(), value.getLabel(), value.getLabel(), first->getLabel(),
        value.getLabel(), instr->getLabel(), value.getScopes()[ 0 ]->getLabel(),
        value.getScopes()[ 0 ]->getLabel() );
}
void CselIRToVHDLPass::visit_interlog( LoopStatement& value, Context& )
{
    fprintf(
        stream, "  -- loop statement '%s' branching;\n", value.getLabel() );
}
void CselIRToVHDLPass::visit_epilog( LoopStatement& value, Context& )
{
    fprintf( stream, "  -- loop statement '%s' end\n", value.getLabel() );
}

//
// CallInstruction
//

void CselIRToVHDLPass::visit_prolog( CallInstruction& value, Context& )
{
    Value* next = value.getNext();
    for( ;; )
    {
        if( next == 0 )
        {
            next = value.getStatement();
            break;
        }

        if( not isa< AllocInstruction >( next )
            and not isa< IdInstruction >( next ) )
        {
            break;
        }
        next = next->getNext();
    }

    fprintf( stream, "    call_%s : entity work.%s port map( sig_%s, sig_%s",
        value.getLabel(), value.getValue( 0 )->getName(), value.getLabel(),
        next->getLabel() );

    u1 first = true;
    for( auto v : value.getValues() )
    {
        if( first )
        {
            first = false;
            continue;
        }

        if( isa< Reference >( v ) and v->getType()->getID() == Type::VECTOR )
        {
            fprintf( stream,
                ", mem_req_%s"
                ", mem_ack_%s"
                ", mem_mode_%s"
                ", mem_addr_%s"
                ", mem_data_%s",
                v->getLabel(), v->getLabel(), v->getLabel(), v->getLabel(),
                v->getLabel() );
        }
        else if( isa< Reference >( v )
                 and v->getType()->getID() == Type::INTERCONNECT )
        {
            fprintf( stream,
                ", ict_req_%s"
                ", ict_ack_%s"
                ", ict_addr_%s"
                ", ict_data_%s",
                v->getLabel(), v->getLabel(), v->getLabel(), v->getLabel() );
        }
        else
        {
            fprintf( stream, ", %s", v->getLabel() );
        }
    }

    fprintf( stream, " ); -- call %lu\n", value.getValues().size() - 1 );
}
void CselIRToVHDLPass::visit_epilog( CallInstruction& value, Context& )
{
}

//
// IdCallInstruction
//

void CselIRToVHDLPass::visit_prolog( IdCallInstruction& value, Context& )
{
    fprintf( stream,
        "    \n"
        "    -- id call\n"
        "    -- %s = %s %s()\n"
        "    -- sig_%s <= TODO;\n",
        value.getLabel(), value.getValue( 0 )->getLabel(),
        value.getValue( 1 )->getLabel(),
        value.getNext() != 0 ? value.getNext()->getLabel()
                             : value.getStatement()->getLabel() );

    fprintf( stream,
        "    idca_%s : process( sig_%s ) is\n"
        "    begin\n"
        "      case %s is\n",
        value.getLabel(), value.getLabel(), value.getValue( 1 )->getLabel() );

    // TODO: FIXME: HACK: PPA: !!! should be dynamically fetched through
    // callable signature!!!
    // TODO: FIXME: HACK: PPA: IDEA: create implementation directly in 'CselIR'
    // !!!

    assert( isa< CallableUnit >( value.getValue( 0 ) ) );
    CallableUnit* cs = (CallableUnit*)value.getValue( 0 );

    Module* m = value.getRef< Module >();

    std::vector< CallableUnit* > selection;
    for( Value* v : m->get< Function >() )
    {
        assert( v and isa< CallableUnit >( v ) );
        CallableUnit* cu = (CallableUnit*)v;
        selection.push_back( cu );

        // TODO: FIXME: PPA: HACK: more checks here regarding the indirect ID
        // call argument types!!!
        if( cu->getInParameters().size() != cs->getInParameters().size() )
        {
            continue;
        }
        if( cu->getOutParameters().size() != cs->getOutParameters().size() )
        {
            continue;
        }

        std::bitset< 48 > bits( cu->getAllocationID()->getValue() );

        fprintf( stream,
            "        when \"%s\" =>\n"
            "          sig_%s_req_%s <= transport sig_%s after 25 ps;\n",
            bits.to_string().c_str(), value.getLabel(), cu->getLabel(),
            value.getLabel() );
    }

    fprintf( stream,
        "        when others =>\n"
        "          null; -- assert false report \"unspecified function to "
        "call\" severity FAILURE;\n"
        "      end case;\n"
        "    end process;\n" );

    std::string args = "";

    assert( selection.size() > 0
            and " at least one function signature has to match the id call! " );
    u32 cnt = 0;
    for( auto v : value.getValues() )
    {
        cnt++;
        if( cnt < 3 )
        {
            continue;
        }

        if( isa< Reference >( v ) and v->getType()->getID() == Type::VECTOR )
        {
            args += ", mem_req_" + std::string( v->getLabel() );
            args += ", mem_ack_" + std::string( v->getLabel() );
            args += ", mem_mode_" + std::string( v->getLabel() );
            args += ", mem_addr_" + std::string( v->getLabel() );
            args += ", mem_data_" + std::string( v->getLabel() );
        }
        else if( isa< Reference >( v )
                 and v->getType()->getID() == Type::INTERCONNECT )
        {
            args += ", ict_req_" + std::string( v->getLabel() );
            args += ", ict_ack_" + std::string( v->getLabel() );
            args += ", ict_addr_" + std::string( v->getLabel() );
            args += ", ict_data_" + std::string( v->getLabel() );
        }
        else
        {
            args += ", " + std::string( v->getLabel() );
        }
    }

    for( Value* v : m->get< Function >() )
    {
        assert( v and isa< CallableUnit >( v ) );
        CallableUnit* cu = (CallableUnit*)v;

        // TODO: FIXME: PPA: HACK: more checks here regarding the indirect ID
        // call argument types!!!
        if( cu->getInParameters().size() != cs->getInParameters().size() )
        {
            continue;
        }
        if( cu->getOutParameters().size() != cs->getOutParameters().size() )
        {
            continue;
        }

        fprintf( stream,
            "   idca_%s : entity work.%s port map ( sig_%s_req_%s, "
            "sig_%s_ack_%s%s );\n",
            cu->getLabel(), cu->getName(), value.getLabel(), cu->getLabel(),
            value.getLabel(), cu->getLabel(), args.c_str() );
    }

    fprintf( stream, "\n" );
}
void CselIRToVHDLPass::visit_epilog( IdCallInstruction& value, Context& )
{
}

//
// StreamInstruction
//

void CselIRToVHDLPass::visit_prolog( StreamInstruction& value, Context& )
{
    assert(0);
}
void CselIRToVHDLPass::visit_epilog( StreamInstruction& value, Context& )
{
}

//
// NopInstruction
//

void CselIRToVHDLPass::visit_prolog( NopInstruction& value, Context& )
{
    fprintf( stream,
        " -- inst_%s: %s\n"
        "    sig_%s <= transport sig_%s after 100 ps;\n",
        value.getLabel(), &value.getName()[ 1 ],
        value.getNext() != 0 ? value.getNext()->getLabel()
                             : value.getStatement()->getLabel(),
        value.getLabel() );
}
void CselIRToVHDLPass::visit_epilog( NopInstruction& value, Context& )
{
}

//
// AllocInstruction
//

void CselIRToVHDLPass::visit_prolog( AllocInstruction& value, Context& )
{
}
void CselIRToVHDLPass::visit_epilog( AllocInstruction& value, Context& )
{
}

static void instr_generic_port( Instruction& value,
    const std::vector< Value* >& args = {}, const char* specializer = 0,
    u32 overwrite_pos = 0, const char* overwrite_str = 0,
    u32 overwrite_pos2 = 0, const char* overwrite_str2 = 0 )
{
    const char* name = &value.getName()[ 1 ];

    fprintf( stream, "    inst_%s : entity work.%s%s%s", value.getLabel(), name,
        ( specializer ? "_" : "" ), ( specializer ? specializer : "" ) );

    for( u32 c = 0; c < args.size(); c++ )
    {
        fprintf( stream, "%s %lu%s", ( c == 0 ? " generic map(" : "," ),
            args[ c ]->getType()->getSize(),
            ( c == ( args.size() - 1 ) ? " )" : "" ) );
    }

    fprintf( stream, " port map( sig_%s, sig_%s", value.getLabel(),
        value.getNext() != 0 ? value.getNext()->getLabel()
                             : value.getStatement()->getLabel() );

    u32 c = 0;
    for( Value* v : value.getValues() )
    {
        c++;
        if( isa< CastInstruction >( &value ) and c == 1 )
        {
            continue;
        }

        if( isa< Reference >( v ) and v->getType()->getID() == Type::VECTOR )
        {
            fprintf( stream,
                ", mem_req_%s"
                ", mem_ack_%s"
                ", mem_mode_%s"
                ", mem_addr_%s"
                ", mem_data_%s",
                v->getLabel(), v->getLabel(), v->getLabel(), v->getLabel(),
                v->getLabel() );
        }
        else if( isa< Reference >( v )
                 and v->getType()->getID() == Type::INTERCONNECT )
        {
            fprintf( stream,
                ", ict_req_%s"
                ", ict_ack_%s"
                ", ict_addr_%s"
                ", ict_data_%s",
                v->getLabel(), v->getLabel(), v->getLabel(), v->getLabel() );
        }
        else
        {
            const char* lbl = v->getLabel();

            if( c == overwrite_pos and overwrite_str )
            {
                lbl = overwrite_str;
            }
            if( c == overwrite_pos2 and overwrite_str2 )
            {
                lbl = overwrite_str2;
            }

            const char* pre = "";
            const char* post = "";

            if( v->getType()->getSize() == 1 )
            {
                if( isa< StoreInstruction >( &value ) and c == 2 )
                {
                    pre = "to_slb( dst ) => ";
                }
                else
                {
                    pre = "to_slv( ";
                    post = " )";
                }
            }

            fprintf( stream, ", %s%s%s", pre, lbl, post );
        }
    }

    if( not value.getType() )
    {
        fprintf( stream, " ); -- %s\n", name );
        return;
    }

    const char* pre = "";

    if( value.getType()->getSize() == 1 )
    {
        if( isa< LoadInstruction >( &value ) or isa< AndInstruction >( &value )
            or isa< XorInstruction >( &value )
            or isa< OrInstruction >( &value )
            or isa< NotInstruction >( &value ) )
        {
            pre = "to_slb( t ) => ";
        }
        else if( not isa< LoadInstruction >( &value )
                 and not isa< EquUnsignedInstruction >( &value )
                 and not isa< NeqUnsignedInstruction >( &value ) )
        {
            assert( 0 );
        }
    }

    fprintf( stream, ", %s%s ); -- %s\n", pre, value.getLabel(), name );
}

//
// IdInstruction
//

void CselIRToVHDLPass::visit_prolog( IdInstruction& value, Context& )
{
}
void CselIRToVHDLPass::visit_epilog( IdInstruction& value, Context& )
{
}

//
// CastInstruction
//

void CselIRToVHDLPass::visit_prolog( CastInstruction& value, Context& )
{
    Value* kind = value.getLHS();
    Value* src = value.getRHS();

    if( isa< ExtractInstruction >( src ) and isa< Structure >( kind )
        // and src->getType()->getID() == Type::STRUCTURE
        )
    {
        const char* type_name = getTypeString( value );

        if( not instruction_implementation )
        {
            instr_generic_port( value, {}, type_name );
            return;
        }

        static std::unordered_set< std::string > used;
        if( used.count( std::string( type_name ) ) > 0 )
        {
            return;
        }
        used.insert( std::string( type_name ) );

        const char* name = &value.getName()[ 1 ];
        fprintf( stream,
            "-- Instruction '%s'\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "use work.Structure.all;\n"
            "use work.Constants.all;\n"
            "use work.Variables.all;\n"
            "entity %s_%s is\n"
            "  port\n"
            "  ( req : in  std_logic\n"
            "  ; ack : out std_logic\n"
            "  ; src : in  %s\n"
            "  ; dst : out %s\n"
            "  );\n"
            "end %s_%s;\n"
            "architecture \\@%s_%s@\\ of %s_%s is\n"
            "begin\n"
            "  process( req, src ) is\n"
            "  begin\n"
            "    if rising_edge( req ) then\n",
            name, name, type_name, getTypeString( *src ),
            getTypeString( value ), name, type_name, name, type_name, name,
            type_name );

        assert( isa< Structure >( kind ) );
        Structure* sty = (Structure*)kind;
        u64 bs = sty->getType()->getSize() - 1;

        for( Value* v : sty->getElements() )
        {
            if( v->getType()->getSize() > 1 )
            {
                fprintf( stream, "      dst.%s <= src( %lu downto %lu );\n",
                    v->getName(), bs, bs - v->getType()->getSize() + 1 );
            }
            else
            {
                fprintf(
                    stream, "      dst.%s <= src( %lu );\n", v->getName(), bs );
            }

            bs = bs - v->getType()->getSize();
        }

        fprintf( stream,
            "      ack <= transport '1' after 50 ps;\n"
            "    end if;\n"
            "  end process;\n"
            "end \\@%s_%s@\\;\n"
            "\n",
            name, type_name );
    }
    else
    {
        if( not instruction_implementation )
        {
            instr_generic_port( value );

            assert( 0 );
            return;
        }

        static u1 used = false;
        if( used )
        {
            return;
        }
        used = true;

        const char* name = &value.getName()[ 1 ];
        fprintf( stream,
            "-- Instruction '%s'\n"
            "-- TODO\n"
            "\n",
            name );
    }
}
void CselIRToVHDLPass::visit_epilog( CastInstruction& value, Context& )
{
}

//
// ExtractInstruction
//

void CselIRToVHDLPass::visit_prolog( ExtractInstruction& value, Context& )
{
    Value* base = value.getLHS();
    Value* offset = value.getRHS();

    if( isa< Reference >( base ) and isa< Instruction >( offset )
        and base->getType()->getID() == Type::VECTOR
        and offset->getType()->getID() == Type::BIT )
    {
        if( not instruction_implementation )
        {
            // Value* bind = base->getType()->getBound();
            // assert( isa< Memory >( bind ) );
            // Memory* mem = (Memory*)bind;

            // instr_generic_port(
            //     value, { offset, mem->getStructure() }, "memory" );

            assert( !" PPA: TODO!!! " );

            return;
        }

        static u1 bitbit = false;
        if( bitbit )
        {
            return;
        }
        bitbit = true;

        const char* name = &value.getName()[ 1 ];
        fprintf( stream,
            "-- Instruction '%s'\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "entity %s_memory is\n"
            "  generic\n"
            "  ( ADDR_WIDTH : integer\n"
            "  ; DATA_WIDTH : integer\n"
            "  );\n"
            "  port\n"
            "  ( req : in  std_logic\n"
            "  ; ack : out std_logic\n"
            "  ; mem_req  : out   std_logic -- memory\n"
            "  ; mem_ack  : in    std_logic\n"
            "  ; mem_mode : out   std_logic\n"
            "  ; mem_addr : out   std_logic_vector( (ADDR_WIDTH-1) downto 0 )\n"
            "  ; mem_data : inout std_logic_vector( (DATA_WIDTH-1) downto 0 )\n"
            "  ; src      : in    std_logic_vector( (ADDR_WIDTH-1) downto 0 )\n"
            "  ; dst      : out   std_logic_vector( (DATA_WIDTH-1) downto 0 )\n"
            "  );\n"
            "end %s_memory;\n"
            "architecture \\@%s_memory@\\ of %s_memory is\n"
            "begin\n"
            "  process( req, src ) is\n"
            "  begin\n"
            "    if rising_edge( req ) then\n"
            "      mem_mode <= '0'; -- read\n"
            "      mem_addr <= src;\n"
            "      mem_req  <= transport '1' after 50 ps;\n"
            "    end if;\n"
            "  end process;\n"
            "  \n"
            "  process( mem_ack, mem_data ) is\n"
            "  begin\n"
            "    if rising_edge( mem_ack ) then\n"
            "      dst <= mem_data;\n"
            "      ack <= transport '1' after 50 ps;\n"
            "    end if;\n"
            "  end process;\n"
            "end \\@%s_memory@\\;\n"
            "\n",
            name, name, name, name, name, name );
    }
    else if( isa< Reference >( base )
             and base->getType()->getID() == Type::INTERCONNECT
             and offset->getType()->getID() == Type::BIT )
    {
        if( not instruction_implementation )
        {
            // Value* bind = base->getType()->getBound();
            // assert( isa< Interconnect >( bind ) );
            // Interconnect* ict = (Interconnect*)bind;

            // static Value addr( "", &TypeId, libcsel_ir::Value::VALUE );
            // static Value data( "", new Type( Type::BIT, ict->getSizeMax(),
            //                            Type::STATE::LOCKED ),
            //     libcsel_ir::Value::VALUE );

            // instr_generic_port( value, { &addr, &data }, "interconnect" );

            assert( !" PPA: TODO!!! " );

            return;
        }

        static u1 used_ict = false;
        if( used_ict )
        {
            return;
        }
        used_ict = true;

        const char* name = &value.getName()[ 1 ];
        fprintf( stream,
            "-- Instruction '%s'\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "entity %s_interconnect is\n"
            "  generic\n"
            "  ( ADDR_WIDTH : integer\n"
            "  ; DATA_WIDTH : integer\n"
            "  );\n"
            "  port\n"
            "  ( req : in  std_logic\n"
            "  ; ack : out std_logic\n"
            "  ; ict_req  : out   std_logic -- interconnect\n"
            "  ; ict_ack  : in    std_logic\n"
            "  ; ict_addr : out   std_logic_vector( (ADDR_WIDTH-1) downto 0 )\n"
            "  ; ict_data : in    std_logic_vector( (DATA_WIDTH-1) downto 0 )\n"
            "  ; src      : in    std_logic_vector( (ADDR_WIDTH-1) downto 0 )\n"
            "  ; dst      : out   std_logic_vector( (DATA_WIDTH-1) downto 0 )\n"
            "  );\n"
            "end %s_interconnect;\n"
            "architecture \\@%s_interconnect@\\ of %s_interconnect is\n"
            "begin\n"
            "  process( req, src ) is\n"
            "  begin\n"
            "    if rising_edge( req ) then\n"
            "      ict_addr <= src;\n"
            "      ict_req  <= transport '1' after 50 ps;\n"
            "    end if;\n"
            "  end process;\n"
            "  \n"
            "  process( ict_ack, ict_data ) is\n"
            "  begin\n"
            "    if rising_edge( ict_ack ) then\n"
            "      dst <= ict_data;\n"
            "      ack <= transport '1' after 50 ps;\n"
            "    end if;\n"
            "  end process;\n"
            "end \\@%s_interconnect@\\;\n"
            "\n",
            name, name, name, name, name, name );
    }
    // else if
    // ( ( isa< CastInstruction >( base ) or isa< Reference >(
    // base ) )
    // and isa< Structure >( offset )
    // and base->getType()->getID() == Type::STRUCTURE
    // )
    // {
    // 	if( not instruction_implementation )
    // 	{
    // 	    fprintf
    // 	    ( stream
    // 	    , "    sig_%s <= transport sig_%s after 25 ps; -- instr_%s: extract
    // omitted due to direct access!\n"
    // 	    , value.getNext()->getLabel()
    // 	    , value.getLabel()
    // 	    , value.getLabel()
    // 	    );
    // 	}
    // }
    else if( ( isa< Instruction >( base ) or isa< Reference >( base ) )
             and isa< Structure >( offset )
             and base->getType()->getID() == Type::STRUCTURE )
    {
        if( not instruction_implementation )
        {
            fprintf( stream,
                "    sig_%s <= transport sig_%s after 25 ps; -- instr_%s: "
                "extract omitted due to direct access!\n",
                value.getNext()->getLabel(), value.getLabel(),
                value.getLabel() );
        }
    }
    else
    {
        if( not instruction_implementation )
        {
            fprintf( stream, " -- inst_%s: extract TODO\n", value.getLabel() );
        }
        // assert( !" unsupported/unimplemented extract feature! " );
        assert( 0 );
    }
}
void CselIRToVHDLPass::visit_epilog( ExtractInstruction& value, Context& )
{
}

//
// LoadInstruction
//

void CselIRToVHDLPass::visit_prolog( LoadInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        Value* src = value.get();

        if( isa< CastInstruction >( src ) )
        {
            fprintf( stream,
                " -- load_%s: -- %s '%s'\n"
                "    sig_%s <= sig_%s;\n"
                "    %s <= %s;\n",
                value.getLabel(), &value.getName()[ 1 ], src->getLabel(),
                value.getNext()->getLabel(), value.getLabel(), value.getLabel(),
                src->getLabel() );

            // instr_generic_port( value, { &value } );
        }
        else if( isa< ExtractInstruction >( src ) )
        {
            ExtractInstruction* ext_src = (ExtractInstruction*)src;
            Value* base_src = ext_src->getLHS();
            Value* offset_src = ext_src->getRHS();

            std::string tmp_src = std::string( base_src->getLabel() ) + "."
                                  + std::string( offset_src->getName() );

            instr_generic_port( value, { &value }, 0, 1, tmp_src.c_str() );

            // fprintf
            // ( stream
            // , " -- load_%s: -- %s '%s'\n"
            //   "    sig_%s <= sig_%s;\n"
            //   "    %s <= %s;\n"
            // , value.getLabel()
            // , &value.getName()[1]
            // , src->getLabel()
            // , value.getNext()->getLabel()
            // , value.getLabel()
            // , value.getLabel()
            // , tmp_src.c_str()
            // );
        }
        else
        {
            fprintf( stream, " -- load_%s: %s -- %s TODO\n", value.getLabel(),
                value.get()->getLabel(), &value.getName()[ 1 ] );
            assert( 0 );
        }
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= a;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( LoadInstruction& value, Context& )
{
}

//
// StoreInstruction
//

void CselIRToVHDLPass::visit_prolog( StoreInstruction& value, Context& )
{
    i16 store_bitsize = -1;

    Value* src = value.getLHS();
    Value* dst = value.getRHS();

    store_bitsize = src->getType()->getSize();

    if( not instruction_implementation )
    {
        if( isa< Instruction >( src ) and isa< Reference >( dst )
            and src->getType()->getID() == Type::BIT
            and dst->getType()->getID() == Type::BIT )
        {
            instr_generic_port( value, { src } );
        }
        else if( isa< ExtractInstruction >( dst ) )
        {
            ExtractInstruction* ext = (ExtractInstruction*)dst;
            Value* base = ext->getLHS();
            Value* offset = ext->getRHS();

            if( ( isa< CastInstruction >( base ) or isa< Reference >( base ) )
                and isa< Structure >( offset )
                and base->getType()->getID() == Type::STRUCTURE )
            {
                std::string tmp_dst = std::string( base->getLabel() ) + "."
                                      + std::string( offset->getName() );

                if( isa< ExtractInstruction >( src ) )
                {
                    ExtractInstruction* ext_src = (ExtractInstruction*)src;
                    Value* base_src = ext_src->getLHS();
                    Value* offset_src = ext_src->getRHS();

                    std::string tmp_src
                        = std::string( base_src->getLabel() ) + "."
                          + std::string( offset_src->getName() );

                    instr_generic_port( value, { dst }, 0, 1, tmp_src.c_str(),
                        2, tmp_dst.c_str() );
                }
                else
                {
                    instr_generic_port( value, { dst }, 0, 2, tmp_dst.c_str() );
                }
            }
            else
            {
                assert( !" internal error! " );
            }
        }
        else
        {
            fprintf( stream, " -- inst_%s: store\n", value.getLabel() );
        }
        return;
    }

    if( store_bitsize < 0 )
    {
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; src : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; dst : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  -- dst <= src;\n"
        "  process( req ) is\n"
        "  begin\n"
        "    if rising_edge( req ) then\n"
        "      dst <= src;\n"
        "    end if;\n"
        "  end process;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( StoreInstruction& value, Context& )
{
}

//
// NotInstruction
//

void CselIRToVHDLPass::visit_prolog( NotInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= not a;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( NotInstruction& value, Context& )
{
}

//
// AndInstruction
//

void CselIRToVHDLPass::visit_prolog( AndInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t <= a and b;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( AndInstruction& value, Context& )
{
}

//
// OrInstruction
//

void CselIRToVHDLPass::visit_prolog( OrInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t <= a or b;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( OrInstruction& value, Context& )
{
}

//
// XorInstruction
//

void CselIRToVHDLPass::visit_prolog( XorInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t <= a xor b;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( XorInstruction& value, Context& )
{
}

//
// AddSignedInstruction
//

void CselIRToVHDLPass::visit_prolog( AddSignedInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= std_logic_vector( signed( a ) + signed( b ) );\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( AddSignedInstruction& value, Context& )
{
}

//
// DivSignedInstruction
//

void CselIRToVHDLPass::visit_prolog( DivSignedInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= std_logic_vector( signed( a ) / signed( b ) );\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( DivSignedInstruction& value, Context& )
{
}

//
// ModUnsignedInstruction
//

void CselIRToVHDLPass::visit_prolog( ModUnsignedInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= std_logic_vector( unsigned( a ) mod unsigned( b ) );\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( ModUnsignedInstruction& value, Context& )
{
}

//
// EquUnsignedInstruction
//

void CselIRToVHDLPass::visit_prolog( EquUnsignedInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { value.getLHS() } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= '1' when ( unsigned( a ) = unsigned( b ) ) else '0';\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( EquUnsignedInstruction& value, Context& )
{
}

//
// NeqUnsignedInstruction
//

void CselIRToVHDLPass::visit_prolog( NeqUnsignedInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; b   : in  std_logic_vector( (BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t   <= '0' when ( unsigned( a ) = unsigned( b ) ) else '1';\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( NeqUnsignedInstruction& value, Context& )
{
}

//
// ZeroExtendInstruction
//

void CselIRToVHDLPass::visit_prolog( ZeroExtendInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { value.get(), &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( FROM_BIT_WIDTH : integer\n"
        "  ; TO_BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (FROM_BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector(   (TO_BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "  constant padding : std_logic_vector( "
        "(TO_BIT_WIDTH-FROM_BIT_WIDTH-1) downto 0 ) := ( others => '0');\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t <= padding & a;\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );

    // u16 bs = value.getType()->getSize() -
    // value.get()->getType()->getSize();
    // std::bitset< 256 > v( 0 );
    // const char* bits = &(v.to_string().c_str()[ 256 - bs ]);
    // const char* fmt = ( bs > 1 ? "\"" : "'" );

    // fprintf
    // ( stream
    // , "      %s := %s%s%s & %s; -- zext\n"
    // , value.getLabel()
    // , fmt
    // , bits
    // , fmt
    // , value.get()->getLabel()
    // );
}
void CselIRToVHDLPass::visit_epilog( ZeroExtendInstruction& value, Context& )
{
}

//
// TruncationInstruction
//

void CselIRToVHDLPass::visit_prolog( TruncationInstruction& value, Context& )
{
    if( not instruction_implementation )
    {
        instr_generic_port( value, { value.get(), &value } );
        return;
    }

    static u1 used = false;
    if( used )
    {
        return;
    }
    used = true;

    const char* name = &value.getName()[ 1 ];
    fprintf( stream,
        "-- Instruction '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "entity %s is\n"
        "  generic\n"
        "  ( FROM_BIT_WIDTH : integer\n"
        "  ; TO_BIT_WIDTH : integer\n"
        "  );\n"
        "  port\n"
        "  ( req : in  std_logic\n"
        "  ; ack : out std_logic\n"
        "  ; a   : in  std_logic_vector( (FROM_BIT_WIDTH-1) downto 0 )\n"
        "  ; t   : out std_logic_vector(   (TO_BIT_WIDTH-1) downto 0 )\n"
        "  );\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  ack <= transport req after 100 ps;\n"
        "  t <= a( TO_BIT_WIDTH-1 downto 0 );\n"
        "end \\@%s@\\;\n"
        "\n",
        name, name, name, name, name, name );
}
void CselIRToVHDLPass::visit_epilog( TruncationInstruction& value, Context& )
{
}

//
// BitConstant
//

void CselIRToVHDLPass::visit_prolog( BitConstant& value, Context& )
{
    if( module->get< Constant >().front() == &value )
    {
        fprintf( stream,
            "-- Constants\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "use work.Structure.all;\n"
            "package Constants is\n" );
    }

    assert( !" PPA: TODO!!! " );

    // u16 bs = value.getType()->getSize();
    // std::bitset< 256 > v( value.getValue() );
    // const char* bits = &( v.to_string().c_str()[ 256 - bs ] );
    // const char* fmt = ( bs > 1 ? "\"" : "'" );

    // StructureConstant* sc = 0;
    // if( value.isBound() )
    // {
    //     sc = value.getBound();
    //     u1 last = sc->getElements().back() == &value;

    //     fprintf( stream, "%s%s%s%s", fmt, bits, fmt, last ? "" : ", " );
    // }
    // else
    // {
    //     fprintf( stream, "  constant %s : %s := %s%s%s;\n", value.getLabel(),
    //         getTypeString( value ), fmt, bits, fmt );
    // }
}
void CselIRToVHDLPass::visit_epilog( BitConstant& value, Context& )
{
    if( module->get< Constant >().back() == &value )
    {
        fprintf( stream,
            "end;\n"
            "\n" );
    }
}

//
// StructureConstant
//

void CselIRToVHDLPass::visit_prolog( StructureConstant& value, Context& )
{
    if( module->get< Constant >().front() == &value )
    {
        fprintf( stream,
            "-- Constants\n"
            "library IEEE;\n"
            "use IEEE.std_logic_1164.all;\n"
            "use IEEE.numeric_std.all;\n"
            "use work.Structure.all;\n"
            "package Constants is\n" );
    }

    fprintf( stream, "  constant %s : %s := ( ", value.getLabel(),
        getTypeString( value ) );
}
void CselIRToVHDLPass::visit_epilog( StructureConstant& value, Context& )
{
    fprintf( stream, ");\n" );

    if( module->get< Constant >().back() == &value )
    {
        fprintf( stream,
            "end;\n"
            "\n" );
    }
}

//
// StringConstant
//

void CselIRToVHDLPass::visit_prolog( StringConstant& value, Context& )
{
    assert(0);
}
void CselIRToVHDLPass::visit_epilog( StringConstant& value, Context& )
{
}

//
// Interconnect
//

void CselIRToVHDLPass::visit_prolog( Interconnect& value, Context& )
{
    static Value n( "", Type::getTypeID(), libcsel_ir::Value::VALUE );

    fprintf( stream,
        "-- Interconnect '%s'\n"
        "library IEEE;\n"
        "use IEEE.std_logic_1164.all;\n"
        "use IEEE.numeric_std.all;\n"
        "use work.Structure.all;\n"
        "use work.Constants.all;\n"
        "use work.Variables.all;\n"
        "entity %s is port\n"
        "( req  : in  std_logic\n"
        "; ack  : out std_logic\n"
        "; addr : in  %s\n"
        "; data : out std_logic_vector( %lu downto 0 )\n",
        value.getLabel(), value.getLabel(), getTypeString( n ),
        value.getBitsizeMax() - 1 );

    for( auto v : value.getObjects() )
    {
        fprintf( stream, "; %s : in %s\n", v->getLabel(), getTypeString( *v ) );
    }

    fprintf( stream,
        ");\n"
        "end %s;\n"
        "architecture \\@%s@\\ of %s is\n"
        "begin\n"
        "  process( req, addr ) is\n"
        "  begin\n"
        "    case addr is\n",
        value.getLabel(), value.getLabel(), value.getLabel() );

    for( auto v : value.getObjects() )
    {
        assert( isa< Variable >( v ) );

        assert( !" PPA: TODO!!! " );

        // Variable* var = (Variable*)v;

        // Value* ty = var->getType()->getBound();
        // assert( isa< Structure >( ty ) );
        // Structure* sty = (Structure*)ty;

        // std::bitset< 48 > bits( var->getAllocationID()->getValue() );

        // // TODO: FIXME: PPA: HACK: this needs to be generic in the future!!!
        // fprintf( stream,
        //     "      when \"%s\" =>\n"
        //     "        data <= ",
        //     bits.to_string().c_str() );

        // u64 padding = 0;
        // for( auto e : sty->getElements() )
        // {
        //     padding += e->getType()->getSize();
        // }

        // u1 first = true;

        // if( padding < 65 )
        // {
        //     std::bitset< 256 > bits( 0 );

        //     fprintf( stream, "\"%s\"",
        //         &bits.to_string().c_str()[ 256 - ( 65 - padding ) ] );

        //     first = false;
        // }

        // for( auto e : sty->getElements() )
        // {
        //     fprintf( stream, "%s%s.%s", ( first ? "" : " & " ),
        //     var->getLabel(),
        //         e->getName() );

        //     if( first )
        //     {
        //         first = false;
        //     }
        // }

        // fprintf( stream, ";\n" );
    }

    fprintf( stream,
        "      when others =>\n"
        "        data <= ( others => 'U' );\n"
        "        null; --assert false report \"unspecified address to "
        "interconnect\" severity FAILURE;\n"
        "    end case;\n"
        "  end process;\n"
        "  ack <= transport req after 100 ps;\n"
        "end \\@%s@\\;\n"
        "\n",
        value.getLabel() );
}
void CselIRToVHDLPass::visit_epilog( Interconnect& value, Context& )
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
