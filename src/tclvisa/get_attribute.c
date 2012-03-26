/*
 * get_attribute.c --
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
#include "tclvisa_utils.h"
#include "visa_channel.h"
#include "visa_utils.h"

int tclvisa_get_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	int attr;
	ViInt64 value = 0;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "session attr");
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

	/* This attribute is processed specially */
	if (VI_ATTR_TMO_VALUE == attr) {
		ViUInt32 timeout;
		int res = getVisaTimeout(interp, session, &timeout);
		if (TCL_OK == res) {
			Tcl_SetObjResult(interp, Tcl_NewLongObj(timeout));
		}
		return res;
	}

	/* Attempt to get attribute */
	status = viGetAttribute(session->session, (ViAttr) attr, &value);

	/* Check status returned */
	if (VI_SUCCESS != status) {
		storeLastError(session, status, interp);
	} else {
		storeLastError(session, status, NULL);
		Tcl_SetObjResult(interp, Tcl_NewLongObj((long) value));
	}

	return status < 0 ? TCL_ERROR : TCL_OK;
}
