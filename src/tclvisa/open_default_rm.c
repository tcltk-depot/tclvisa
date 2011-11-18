/*
 * open_default_rm.c --
 *
 * This file is part of tclvisa library.
 *
 * Copyright (c) 2011 Andrey V. Nakin <andrey.nakin@gmail.com>
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include <tcl.h>
#include <visa.h>
#include "visa_utils.h"
#include "visa_channel.h"
#include "tclvisa_utils.h"

int tclvisa_open_default_rm(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	ViSession session;
	ViStatus status;
	Tcl_Channel channel;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 1) {
		Tcl_WrongNumArgs(interp, 1, objv, "<no arguments>");
		return TCL_ERROR;
	}

	/* Attempt to open resource manager */
	status = viOpenDefaultRM(&session);

	if (status < 0) {
		Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
		return TCL_ERROR;
	}

	/* Create Tcl channel backed by VISA session */
	channel = createVisaChannel(interp, session);
	if (NULL == channel) {
		return TCL_ERROR;
	}

	/* Return channel string representation as a procedure result */
	Tcl_AppendResult(interp, Tcl_GetChannelName(channel), NULL);
	return TCL_OK;
}
