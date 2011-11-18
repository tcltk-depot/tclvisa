#include <visa.h>
#include <tcl.h>
#include "tclvisa_utils.h"
#include "visa_channel.h"
#include "visa_utils.h"

int tclvisa_get_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]) {
	VisaChannelData* session;
	ViStatus status;
	int attr;
	ViInt64 value;

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

	/* Attempt to get attribute */
	status = viGetAttribute(session->session, (ViAttr) attr, &value);

	/* Check status returned */
	if (VI_SUCCESS != status) {
		Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
	} else {
		Tcl_SetObjResult(interp, Tcl_NewLongObj((long) value));
	}

	return status < 0 ? TCL_ERROR : TCL_OK;
}
