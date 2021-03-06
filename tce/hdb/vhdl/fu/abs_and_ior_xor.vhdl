-- Copyright (c) 2002-2009 Tampere University of Technology.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation
-- the rights to use, copy, modify, merge, publish, distribute, sublicense,
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
-- DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------
-- Title      : Logical unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : and_ior_xor_abs.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2010-04-23
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Logical functional unit for TTA
--              -logic architecture as a separate component
--              -Supports SVTL pipelining discipline
--              opcode  0 and
--                      1 ior
--                      2 xor
--                      3 abs
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-06-24  1.0      sertamo Created
-------------------------------------------------------------------------------


library IEEE;--, DW01;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use DW01.DW01_components.all;

entity abs_and_ior_xor_arith is
  generic (
    dataw : integer := 32);
  port(
    T1    : in  std_logic_vector(dataw-1 downto 0);
    T1opc : in  std_logic_vector(1 downto 0);
    O1    : in  std_logic_vector(dataw-1 downto 0);
    R1    : out std_logic_vector(dataw-1 downto 0));
end abs_and_ior_xor_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for logic unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of abs_and_ior_xor_arith is

begin
  sel : process(T1, O1, T1OPC)
    variable opc : integer;
  begin
    opc := conv_integer(unsigned(T1OPC));
    case opc is
      when 0 =>
        if signed(T1) < conv_signed(0,T1'length) then
          R1 <= conv_std_logic_vector(conv_signed(0,R1'length) - signed(T1), R1'length);
        else
          R1 <= T1;
        end if;
      when 1      =>
        R1 <= T1 and O1;
      when 2      =>
        R1 <= T1 or O1;
      when others =>
        R1 <= T1 xor O1;                
    end case;
  end process sel;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit and_ior_xor latency 1
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_abs_and_ior_xor_always_1 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_abs_and_ior_xor_always_1;

architecture rtl of fu_abs_and_ior_xor_always_1 is
  
  component abs_and_ior_xor_arith
    generic (
      dataw : integer);
    port(
      T1    : in  std_logic_vector(dataw-1 downto 0);
      T1opc : in  std_logic_vector(1 downto 0);
      O1    : in  std_logic_vector(dataw-1 downto 0);
      R1    : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal T1opc_reg : std_logic_vector(1 downto 0);
  signal control   : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : abs_and_ior_xor_arith
    generic map (
      dataw => dataw)      
    port map(
      T1    => t1reg,
      T1opc => T1opc_reg,
      O1    => o1reg,
      R1    => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      T1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      o1temp    <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg     <= o1data;
            o1temp    <= o1data;
            t1reg     <= t1data;
            t1opc_reg <= t1opcode;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg     <= o1temp;
            t1reg     <= t1data;
            t1opc_reg <= t1opcode;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  output: process (r1)
  begin  -- process output
    if DATAW > BUSW then
      r1data(BUSW-1 downto 0) <= r1(BUSW-1 downto 0);
    else
      r1data <= sxt(r1,BUSW);
    end if;
  end process output;
  
end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for unit and_ior_xor latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_abs_and_ior_xor_always_2 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_abs_and_ior_xor_always_2;

architecture rtl of fu_abs_and_ior_xor_always_2 is
  
  component abs_and_ior_xor_arith
    port(
      T1    : in  std_logic_vector(dataw-1 downto 0);
      T1opc : in  std_logic_vector(1 downto 0);
      O1    : in  std_logic_vector(dataw-1 downto 0);
      R1    : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  signal T1opc_reg : std_logic_vector(1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : abs_and_ior_xor_arith
    port map(
      T1    => t1reg,
      T1opc => T1opc_reg,
      O1    => o1reg,
      R1    => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      T1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      r1reg     <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg     <= o1data;
            t1reg     <= t1data;
            t1opc_reg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg     <= t1data;
            t1opc_reg <= t1opcode;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;

      end if;
    end if;
  end process regs;

  output: process (r1reg)
  begin  -- process output
    if DATAW > BUSW then
      r1data(BUSW-1 downto 0) <= r1reg(BUSW-1 downto 0);
    else
      r1data <= sxt(r1reg,BUSW);
    end if;
  end process output;

  --r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;

