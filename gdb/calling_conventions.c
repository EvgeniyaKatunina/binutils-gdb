#include "gdb/calling_conventions.h"
#include "gdb/common/common-utils.h"

char * calling_convention_mode = calling_convention_clang;
const char calling_convention_gcc[] = "gcc";
const char calling_convention_clang[] = "clang";
int is_calling_convention_defined_by_user = FALSE;

const char *const calling_convention_enums[] =
{
  calling_convention_gcc,
  calling_convention_clang,
  NULL
};

const char *
arc_default_calling_convention (CORE_ADDR pc)
{
  CORE_ADDR func_addr;
  struct compunit_symtab *cust = find_pc_compunit_symtab (func_addr);
  if (find_pc_partial_function (pc, NULL, &func_addr, NULL))
    {
      const char * producer = COMPUNIT_PRODUCER (cust);
      if (cust != NULL && producer != NULL)
	{
	  if (startswith (producer, "GNU "))
	    {
	      return calling_convention_gcc;
	    }
	  if (startswith (producer, "clang "))
	    {
	      return calling_convention_clang;
	    }
	}
    }
  return calling_convention_clang;
}

