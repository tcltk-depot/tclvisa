/*
 * tcl_utils.c --
 *
 * This file is part of tclvisa library.
 *
 * Copyright (c) 2011 Andrey V. Nakin <andrey.nakin@gmail.com>
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include "tcl_utils.h"

int Tcl_GetUIntFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr, unsigned int *uintPtr) {
	long l;

	if (Tcl_GetLongFromObj(interp, objPtr, &l)) {
		return TCL_ERROR;
	}

	if (l < 0) {
		if (interp) {
			Tcl_AppendResult(interp, "expected unsigned integer but got negative value", NULL);
		}
		return TCL_ERROR;
	}

	if (uintPtr) {
		*uintPtr = (unsigned int) l;
	}

	return TCL_OK;
}

