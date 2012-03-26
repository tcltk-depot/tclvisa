/*
 * find.c --
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

int tclvisa_find(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	ViChar rsrcName[VI_FIND_BUFLEN];
	ViUInt32 retCount;
	ViFindList flist;
	Tcl_Obj *res;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "RMsession expr");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	/* Retrieve addresses found */
	res = Tcl_NewListObj(0, NULL);
	status = viFindRsrc(session->session, Tcl_GetString(objv[2]), &flist, &retCount, rsrcName);
	if (status >= 0) {
		while (status >= 0 && retCount--) {
			Tcl_ListObjAppendElement(interp, res, Tcl_NewStringObj(rsrcName, -1));
			status = viFindNext(flist, rsrcName);
		}

		viClose(flist);
	}

	/* Check status returned */
	if (status < 0 && VI_ERROR_RSRC_NFOUND != status) {
		storeLastError(session, status, interp);
		TclFreeObj(res);
		return TCL_ERROR;
	} else {
		storeLastError(session, VI_SUCCESS, NULL);
		Tcl_SetObjResult(interp, res);
		return TCL_OK;
	}
}
