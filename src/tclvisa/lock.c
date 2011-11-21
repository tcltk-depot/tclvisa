/*
 * lock.c --
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

int tclvisa_lock(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	ViAccessMode lockType = VI_EXCLUSIVE_LOCK;
	ViUInt32 timeout = VI_TMO_INFINITE;
	ViKeyId requestedKey = VI_NULL;
	ViChar* accesskey = VI_NULL;
	ViChar buf[VI_FIND_BUFLEN];

	UNREFERENCED_PARAMETER(clientData);	/* avoid "unused parameter" warning */

	/* Check number of arguments */
	if (objc < 2 || objc > 5) {
		Tcl_WrongNumArgs(interp, 1, objv, "session ?lockType? ?timeout? ?requestedKey?");
		return TCL_ERROR;
	}

	/* Convert first argument to valid Tcl channel reference */
    session = getVisaChannelFromObj(interp, objv[1]);
	if (session == NULL) {
		return TCL_ERROR;
	}

	if (objc > 2) {
		/* Lock type */
		long l;
		if (Tcl_GetLongFromObj(interp, objv[2], &l)) {
			return TCL_ERROR;
		}
		lockType = (ViAccessMode) l;
	}

	if (objc > 3) {
		/* Timeout */
		long l;
		if (Tcl_GetLongFromObj(interp, objv[3], &l)) {
			return TCL_ERROR;
		}
		timeout = (ViUInt32) l;
	}

	if (VI_EXCLUSIVE_LOCK != lockType) {
		/* Following options are only available for non-exclusive locks */

		if (objc > 4) {
			/* Requested key */
			long l;
			if (Tcl_GetLongFromObj(interp, objv[4], &l)) {
				return TCL_ERROR;
			}
			requestedKey = (ViKeyId) l;
		}

		accesskey = buf;
	}

	/* Attempt to lock instrument */
	status = viLock(session->session, lockType, timeout, requestedKey, accesskey);

	/* Check status returned */
	if (status < 0) {
		Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
	} else {
		if (accesskey) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj(accesskey, -1));
		} else {
			Tcl_ResetResult(interp);
		}
	}

	return status < 0 ? TCL_ERROR : TCL_OK;
}
