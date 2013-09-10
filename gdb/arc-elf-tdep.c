/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.
   Copyright 2009-2013 Synopsys Inc.

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com> on behalf of
   Synopsys Inc.
   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com) on behalf of
   Synopsys Inc.

   Authors:
      Jeremy Bennett       <jeremy.bennett@embecosm.com>
      Soam Vasani          <soam.vasani@codito.com>
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com>
      Richard Stuckey      <richard.stuckey@arc.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module provides support for the ARC processor family's target     */
/*     dependencies which are specific to the arc-linux-uclibc configuration  */
/*     of the ARC gdb.                                                        */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <string.h>

/* gdb header files */
#include "defs.h"
#include "inferior.h"
#include "gdbcmd.h"
#include "regcache.h"
#include "reggroups.h"
#include "observer.h"
#include "objfiles.h"
#include "arch-utils.h"

/* ARC header files */
#include "arc-tdep.h"

#ifdef WITH_SIM
/* ARC simulator header files */
#include "sim/arc/arc-sim-registers.h"
#endif


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef struct
{
  struct gdbarch *gdbarch;
  struct ui_file *file;
  struct frame_info *frame;
} PrintData;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */
/*		   ARC specific GDB architectural functions		      */
/*									      */
/* Functions are listed in the order they are used in arc_elf_init_abi.       */
/* -------------------------------------------------------------------------- */

/*! Get breakpoint which is approriate for address at which it is to be set.

    For ARC ELF, breakpoint uses the 16-bit BRK_S instruction, which is 0x7fff
    (little endian) or 0xff7f (big endian).

    @todo Surely we should be able to use the same breakpoint instruction for
          both Linux and ELF?

    @param[in]     gdbarch  Current GDB architecture
    @param[in,out] pcptr    Pointer to the PC where we want to place a
                            breakpoint
    @param[out]    lenptr   Number of bytes used by the breakpoint.
    @return                 The byte sequence of a breakpoint instruction. */
static const unsigned char *
arc_elf_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR * pcptr,
			    int *lenptr)
{
  static const unsigned char breakpoint_instr_be[] = { 0x7f, 0xff };
  static const unsigned char breakpoint_instr_le[] = { 0xff, 0x7f };

  *lenptr = sizeof (breakpoint_instr_be);
  return (gdbarch_byte_order (gdbarch) == BFD_ENDIAN_BIG)
    ? breakpoint_instr_be
    : breakpoint_instr_le;

}	/* arc_elf_breakpoint_from_pc () */


#ifdef WITH_SIM
/*! Map GDB registers to ARC simulator registers

    The ARC CGEN based simulator has its own register numbering. This function
    provides the necessary mapping

    The simulator does not have a simple numbering. Rather registers are known
    by a class and a number.

    @param[in]  gdb_regnum  The GDB register number to map
    @param[out] sim_regnum  The corresponding simulator register
    @param[out] reg_class   The corresponding ARC register class */
static void
arc_elf_sim_map (int                gdb_regnum,
		 int*               sim_regnum,
		 ARC_RegisterClass *reg_class)
{
  if ((0 <= gdb_regnum) && (gdb_regnum <= ARC_LP_COUNT_REGNUM))
    {
      /* All core registers apart from reserved, LIMM and PCL have an
	 identity mapping. */
      *sim_regnum = gdb_regnum;
      *reg_class  = ARC_CORE_REGISTER;
    }
  else if (ARC_PC_REGNUM == gdb_regnum)
    {
      /* sim_regnum irrelevant. */
      *reg_class = ARC_PROGRAM_COUNTER;
    }
  else
    {
      *sim_regnum = arc_sim_aux_reg_map_lookup (gdb_regnum);

      if (*sim_regnum == -1)
	  *reg_class = ARC_UNKNOWN_REGISTER;
      else
	*reg_class  = ARC_AUX_REGISTER;
    }
}	/* arc_elf_sim_map () */
#endif


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/*! Function to determine the OSABI variant.

    Every target variant must define this appropriately.

    @return  The OSABI variant. */
enum gdb_osabi
arc_get_osabi (void)
{
  return GDB_OSABI_LINUX;

}	/* arc_get_osabi () */


/*! Function to initialize for this target variant.

    Every target variant must define this appropriately.

    @param[in,out] gdbarch  The gdbarch we are initializing. */
void
arc_gdbarch_osabi_init (struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Fill in target-dependent info in ARC-private structure. */
  tdep->is_sigtramp = NULL;
  tdep->sigcontext_addr = NULL;
  tdep->sc_reg_offset = NULL;
  tdep->sc_num_regs = 0;

  /* Set up target dependent GDB architecture entries. */
  set_gdbarch_breakpoint_from_pc (gdbarch, arc_elf_breakpoint_from_pc);

#ifdef WITH_SIM
  /* Provide the built-in simulator with a function that it can use to map
     from gdb register numbers to h/w register numbers */
  arc_set_register_mapping (&arc_elf_sim_map);
#endif

}	/* arc_gdbarch_osabi_init () */
