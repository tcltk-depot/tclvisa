/*
 * get_last_error.c --
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

#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <visa.h>
#include <tcl.h>
#include "tclvisa_utils.h"
#include "visa_channel.h"
#include "visa_utils.h"

int tclvisa_get_last_error(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	Tcl_Obj* res;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "channel");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	res = Tcl_NewListObj(0, NULL);
	Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(session->lastError));
	if (session->lastErrorMsg) {
		char *buf, *s;

		buf = (char*) malloc(strlen(session->lastErrorMsg) + 1);
		strcpy(buf, session->lastErrorMsg);

		s = strtok(buf, "[]");
		Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj(s ? s : "", -1));

		for (s = strtok(NULL, "[]"); isspace(*s); ++s) {}
		Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj(s ? s : "", -1));

		free((void*) buf);
	} else {
		Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj("", -1));
		Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj("", -1));
	}
	Tcl_SetObjResult(interp, res);

	return TCL_OK;
}
