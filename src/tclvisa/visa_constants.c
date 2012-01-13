/*
 * visa_constants.c --
 *
 * This file is part of tclvisa library.
 *
 * Copyright (c) 2011 Andrey V. Nakin <andrey.nakin@gmail.com>
 * All rights reserved.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include <tcl.h>
#include <visa.h>
#include <stdio.h>

static int addIntegerVar(Tcl_Interp* const interp, const char* prefix, const char* name, size_t nameOffset, int value) {
	char qualifiedName[64];

	sprintf(qualifiedName, "%s%s", prefix, name + nameOffset);
	return NULL == Tcl_SetVar2Ex(interp, qualifiedName, NULL, Tcl_NewIntObj(value), TCL_LEAVE_ERR_MSG)
		? TCL_ERROR : TCL_OK;
}

static int addStringVar(Tcl_Interp* const interp, const char* prefix, const char* name, size_t nameOffset, const char* value) {
	char qualifiedName[64];
	Tcl_Obj* v = Tcl_NewStringObj(value, 0);

	sprintf(qualifiedName, "%s%s", prefix, name + nameOffset);
	return NULL == Tcl_SetVar2Ex(interp, qualifiedName, NULL, v, TCL_LEAVE_ERR_MSG)
		? TCL_ERROR : TCL_OK;
}

#define addIntVar(name)	\
	if (TCL_OK != addIntegerVar(interp, prefix, #name, 3, name)/* || TCL_OK != addIntegerVar(interp, "", #name, 0, name)*/) goto error

int setVisaConstants(Tcl_Interp* const interp, const char* prefix, const char *version) {

#include "visa_constants.inc"

	if (TCL_OK != addStringVar(interp, "", "TCLVISAVERSION", 0, version)) {
		goto error;
	}

	return TCL_OK;

error:
	return TCL_ERROR;
}
