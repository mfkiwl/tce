-------------------------------------------------------------------------------
-- Title      : Sign Extension unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : zxhw.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2003-08-28
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: sign extension functional unit
--              sign extends half-word into a word
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Entity declaration for zxhw unit internals
----------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_zxhw is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_zxhw;


architecture rtl_0_stage_gated_clock of fu_zxhw is
  
  signal t1reg : std_logic_vector(dataw/2-1 downto 0);
  
begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then
        
        if t1load = '1' then
          t1reg <= t1data(dataw/2-1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  r1data <= EXT(t1reg, r1data'length);
  
end rtl_0_stage_gated_clock;




-------------------------------------------------------------------------------
-- The entities and architectures employing new naming conventions start here
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_zxhw_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock      : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_zxhw_always_1;


architecture rtl of fu_zxhw_always_1 is
  
  signal t1reg : std_logic_vector(dataw/2-1 downto 0);
  
begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then
        
        if t1load = '1' then
          t1reg <= t1data(dataw/2-1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  r1data <= EXT(t1reg, r1data'length);
  
end rtl;
