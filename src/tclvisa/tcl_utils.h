/*
 * tcl_utils.h --
 *
 * This file is part of tclvisa library.
 *
 * Copyright (c) 2011 Andrey V. Nakin <andrey.nakin@gmail.com>
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef TCL_UTILS_H_34237856365464
#define TCL_UTILS_H_34237856365464

#include <tcl.h>

/*
 *----------------------------------------------------------------
 * Macro used by the Tcl core to get the string representation's byte array
 * pointer from a Tcl_Obj. This is an inline version of Tcl_GetString(). The
 * macro's expression result is the string rep's byte pointer which might be
 * NULL. The bytes referenced by this pointer must not be modified by the
 * caller. The ANSI C "prototype" for this macro is:
 *
 * MODULE_SCOPE char *	TclGetString(Tcl_Obj *objPtr);
 *----------------------------------------------------------------
 */
#ifndef TclGetString
#define TclGetString(objPtr) \
    ((objPtr)->bytes? (objPtr)->bytes : Tcl_GetString((objPtr)))
#endif

int Tcl_GetUIntFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr, unsigned int *uintPtr);

#endif /* TCL_UTILS_H_34237856365464 */
