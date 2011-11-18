/*
 * set_attribute.c --
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

#include <visa.h>
#include <tcl.h>
#include "visa_channel.h"
#include "visa_utils.h"
#include "tclvisa_utils.h"

int tclvisa_set_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	int attr, value;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "session attr attrValue");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	/* Read attribute code */
	if (TCL_OK != Tcl_GetIntFromObj(interp, objv[2], &attr)) {
		return TCL_ERROR;
	}

	/* Read attribute value */
	if (TCL_OK != Tcl_GetIntFromObj(interp, objv[3], &value)) {
		return TCL_ERROR;
	}

	/* Attempt to set attribute */
	status = viSetAttribute(session->session, (ViAttr) attr, (ViAttrState) value);

	/* Check status returned */
	if (VI_SUCCESS != status) {
		Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
	} else {
		Tcl_ResetResult(interp);
	}
	
	return status < 0 ? TCL_ERROR : TCL_OK;
}
