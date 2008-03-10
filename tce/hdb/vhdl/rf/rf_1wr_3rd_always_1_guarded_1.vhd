---------------------------------------------------------------------------------
-- Title        : File for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
--
-- VHDL Architecture RF_lib.rf_1wr_3rd_always_1.symbol
--
-- Created:  10:55:31 11/22/05
--          by - tpitkane.tpitkane (elros)
--          at - 10:55:31 11/22/05
--
-- Generated by Mentor Graphics' HDL Designer(TM) 2004.1 (Build 41)
---------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 11/22/05      1.0     tpitkane   Created
-------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
LIBRARY work;
USE work.util.all;

ENTITY rf_1wr_3rd_always_1_guarded_1 IS
   GENERIC( 
      dataw   : integer := 32;
      rf_size : integer := 4
   );
   PORT( 
      clk      : IN     std_logic;
      glock    : IN     std_logic;
      r1load   : IN     std_logic;
      r1opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      r2load   : IN     std_logic;
      r2opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      r3load   : IN     std_logic;
      r3opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      rstx     : IN     std_logic;
      t1data   : IN     std_logic_vector ( dataw-1 DOWNTO 0 );
      t1load   : IN     std_logic;
      t1opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      r1data   : OUT    std_logic_vector ( dataw-1 DOWNTO 0 );
      r2data   : OUT    std_logic_vector ( dataw-1 DOWNTO 0 );
      r3data   : OUT    std_logic_vector ( dataw-1 DOWNTO 0 );
      guard    : OUT    std_logic_vector ( rf_size-1 DOWNTO 0)
   );

-- Declarations

END rf_1wr_3rd_always_1_guarded_1 ;
---------------------------------------------------------------------------------
-- Title        : File for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
--
-- VHDL Architecture RF_lib.rf_1wr_3rd_always_1.rtl
--
-- Created:  10:55:31 11/22/05
--          by - tpitkane.tpitkane (elros)
--          at - 10:55:31 11/22/05
--
-- Generated by Mentor Graphics' HDL Designer(TM) 2004.1 (Build 41)
---------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 11/22/05      1.0     tpitkane   Created
---------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
LIBRARY work;
USE work.util.all;
ARCHITECTURE rtl OF rf_1wr_3rd_always_1_guarded_1 IS

   -- Architecture declarations
   type   reg_type is array (natural range <>) of std_logic_vector(dataw-1 downto 0 );
   subtype rf_index is integer range 0 to rf_size-1;
   signal reg    : reg_type (rf_size-1 downto 0);

BEGIN

   -----------------------------------------------------------------
   Input : PROCESS (clk, rstx)
   -----------------------------------------------------------------

   -- Process declarations
   variable opc : integer;


   BEGIN
      -- Asynchronous Reset
      IF (rstx = '0') THEN
         -- Reset Actions
         for idx in (reg'length-1) downto 0 loop
             reg(idx) <= (others => '0');
         end loop;  -- idx

      ELSIF (clk'EVENT AND clk = '1') THEN
         IF glock = '0' THEN
            IF t1load = '1' THEN
               opc := conv_integer(unsigned(t1opcode));
               reg(opc) <= t1data;
            END IF;
         END IF;
      END IF;
   END PROCESS Input;

   -----------------------------------------------------------------
   --output : PROCESS (glock, r1load, r1opcode, r2load, r2opcode, r3load, r3opcode, reg, rstx)
   -----------------------------------------------------------------


   r1data <= reg(conv_integer(unsigned(r1opcode)));
   r2data <= reg(conv_integer(unsigned(r2opcode)));
   r3data <= reg(conv_integer(unsigned(r3opcode)));

   -----------------------------------------------------------------
   guard_out : PROCESS (reg)
   -----------------------------------------------------------------

   -- Process declarations
   variable guard_var : std_logic_vector(0 downto 0);


   BEGIN

         for i in rf_size-1 downto 0 loop
           if dataw > 1 then
             guard_var := 
               reg(i)(dataw-1 downto dataw-1) 
               or reg(i)(dataw-2 downto dataw-2);
             for j in dataw-2 downto 0 loop
               guard_var := reg(i)(j downto j) 
                            or guard_var;
             end loop;
           else
             guard_var(0 downto 0) := reg(i)(0 downto 0);
           end if;

           guard(i downto i) <= guard_var(0 downto 0);
         end loop;
   END PROCESS guard_out;

END rtl;
