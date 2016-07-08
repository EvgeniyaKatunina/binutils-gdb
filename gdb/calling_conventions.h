#ifndef CALLING_CONVENTIONS_H
#define CALLING_CONVENTIONS_H

#include "defs.h"
#include "gdb/symtab.h"
#include "gdb/common/ptid.h"

extern const char calling_convention_gcc[];
extern const char calling_convention_clang[];

extern char * calling_convention_mode;

struct calling_convention_info
{
  char * calling_convention;
  ptid_t inferior_ptid;
  int is_defined_by_user;
};

struct calling_convention_info calling_conventions_for_ptids[];

extern const char *const calling_convention_enums[];

struct calling_convention_info get_calling_convention_info_by_inferior_ptid (ptid_t inferior_ptid);

const char * arc_default_calling_convention (CORE_ADDR pc);

int is_calling_convention (const char * calling_convention);

int calling_convention_equal (const char * calling_convention1, const char * calling_convention2);
#endif
