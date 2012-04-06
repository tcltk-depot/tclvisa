/*
 * gpib_pass_control.c --
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
#include "visa_channel.h"
#include "visa_utils.h"
#include "tcl_utils.h"
#include "tclvisa_utils.h"

int tclvisa_gpib_pass_control(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViUInt16 primAddr, secAddr;
	ViStatus status;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc < 3 || objc > 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "session primAddr ?secAddr?");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	if (Tcl_GetUInt16FromObj(interp, objv[2], &primAddr)) {
		return TCL_ERROR;
	}

	if (objc > 3) {
		if (Tcl_GetUInt16FromObj(interp, objv[3], &secAddr)) {
			return TCL_ERROR;
		}
	} else {
		secAddr = VI_NO_SEC_ADDR;
	}

	/* Call VISA function */
	status = viGpibPassControl(session->session, primAddr, secAddr);
	/* Check status returned */
	storeLastError(session, status, interp);

	return status < 0 ? TCL_ERROR : TCL_OK;
}
