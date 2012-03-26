/*
 * unlock.c --
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

int tclvisa_unlock(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "session");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	/* Attempt to unlock instrument */
	status = viUnlock(session->session);
	storeLastError(session, status, interp);

	return status < 0 ? TCL_ERROR : TCL_OK;
}
