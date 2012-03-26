/*
 * read_to_file.c --
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
#include <stdio.h>
#include "visa_channel.h"
#include "visa_utils.h"
#include "tcl_utils.h"
#include "tclvisa_utils.h"

int read_to_file(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	ViUInt32 retCount;
	unsigned int count;
	const char* fileName;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "session fileName count");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	fileName = Tcl_GetString(objv[2]);

	if (Tcl_GetUIntFromObj(interp, objv[3], &count)) {
		return TCL_ERROR;
	}

	/* Attempt to read */
	status = viReadToFile(session->session, fileName, (ViUInt32) count, &retCount);

	/* Check status returned */
	if (status < 0 && VI_ERROR_TMO != status) {
		storeLastError(session, status, interp);
		return TCL_ERROR;
	} else {
		storeLastError(session, status, NULL);
		Tcl_SetObjResult(interp, Tcl_NewLongObj((long) retCount));
		return TCL_OK;
	}
}
