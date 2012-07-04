/*
 * assert_intr_signal.c --
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

int tclvisa_assert_intr_signal(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	ViUInt16 mode;
	ViUInt32 statusID = 0;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc < 3 || objc > 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "session mode ?statusID?");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	if (Tcl_GetUInt16FromObj(interp, objv[2], &mode)) {
		return TCL_ERROR;
	}

	if (objc > 3) {
		if (Tcl_GetUInt32FromObj(interp, objv[3], &statusID)) {
			return TCL_ERROR;
		}
	}

	/* Call VISA function */
	status = viAssertIntrSignal(session->session, mode, statusID);
	/* Check status returned */
	storeLastError(session, status, interp);

	return status < 0 ? TCL_ERROR : TCL_OK;
}
