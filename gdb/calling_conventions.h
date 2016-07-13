#ifndef CALLING_CONVENTIONS_H
#define CALLING_CONVENTIONS_H

#include "defs.h"
#include "gdb/symtab.h"
#include "gdb/common/ptid.h"

extern const char calling_convention_gcc[];
extern const char calling_convention_clang[];

extern char * calling_convention_mode;

extern int is_calling_convention_defined_by_user;

extern const char *const calling_convention_enums[];

const char * arc_default_calling_convention (CORE_ADDR pc);

#endif
