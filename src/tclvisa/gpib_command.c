/*
 * gpib_command.c --
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
#include <string.h>
#include "visa_channel.h"
#include "visa_utils.h"
#include "tcl_utils.h"
#include "tclvisa_utils.h"

int tclvisa_gpib_command(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	ViUInt32 count, retCount;
	const char* buf;

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc < 3 || objc > 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "session buf ?count?");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	buf = TclGetString(objv[2]);

	if (objc > 3) {
		if (Tcl_GetUInt32FromObj(interp, objv[3], &count)) {
			return TCL_ERROR;
		}
	} else {
		const size_t sz = strlen(buf);
		count = sz > 0xffffffff ? 0xffffffff : (ViUInt32) sz;
	}

	/* Call VISA function */
	status = viGpibCommand(session->session, (ViBuf) buf, count, &retCount);
	storeLastError(session, status, interp);

	/* Check status returned */
	if (status >= 0) {
		/* Return number of bytes transmitted */
		Tcl_SetObjResult(interp, Tcl_NewLongObj((long) retCount));
		return TCL_OK;
	} else {
		return TCL_ERROR;
	}
}
