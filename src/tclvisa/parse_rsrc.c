/*
 * parse_rsrc.c --
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
#include "visa_channel.h"
#include "tclvisa_utils.h"
#include "tcl_utils.h"
#include "visa_utils.h"

int tclvisa_parse_rsrc(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData *rmSession;
	ViStatus status;
	ViUInt16 intfType, intfNum;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "RMsession rsrcName");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    rmSession = getVisaChannelFromObj(interp, objv[1]);
	if (rmSession == NULL) {
		return TCL_ERROR;
	}

	/* Attempt to parse resource address */
	status = viParseRsrc(rmSession->session, TclGetString(objv[2]), &intfType, &intfNum);

	/* Check status returned */
	if (status == VI_ERROR_INV_RSRC_NAME || status == VI_ERROR_RSRC_NFOUND) {
		storeLastError(rmSession, status = VI_SUCCESS, interp);
	} else if (status < 0) {
		storeLastError(rmSession, status, interp);
	} else {
		Tcl_Obj *res = Tcl_NewListObj(0, NULL);
		Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(intfType));
		Tcl_ListObjAppendElement(interp, res, Tcl_NewIntObj(intfNum));
		Tcl_SetObjResult(interp, res);
		storeLastError(rmSession, status, NULL);
	}

	return status < 0 ? TCL_ERROR : TCL_OK;
}
