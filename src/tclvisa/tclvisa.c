/*
 * tclvisa.c --
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

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "tclvisa"
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "0.1.1"
#endif

#include <tcl.h>
#ifdef _WINDOWS
#include <windows.h>
#endif
#include "tclvisa_utils.h"

#define NAMESPACE "visa::"

/*
 * Forward declarations
 */
int tclvisa_open(const ClientData clientData, Tcl_Interp * const interp, const int objc, Tcl_Obj *const objv[]);
int tclvisa_open_default_rm(const ClientData clientData, Tcl_Interp * const interp, const int objc, Tcl_Obj *const objv[]);
int tclvisa_set_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_get_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_clear(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_lock(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_unlock(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_find(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int write_from_file(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int read_to_file(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_parse_rsrc(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_get_last_error(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_assert_trigger(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_assert_intr_signal(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_assert_util_signal(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_gpib_command(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_gpib_control_atn(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_gpib_control_ren(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_gpib_pass_control(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_gpib_send_ifc(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);

int setVisaConstants(Tcl_Interp* const interp, const char* prefix, const char *version);

#define addCommand(tcl_name, proc)	\
	if (NULL == Tcl_CreateObjCommand(interp, NAMESPACE tcl_name, proc, NULL, NULL))	\
		goto error

int createTclvisaCommands(Tcl_Interp* const interp) {
	addCommand("open", tclvisa_open);
	addCommand("open-default-rm", tclvisa_open_default_rm);
	addCommand("set-attribute", tclvisa_set_attribute);
	addCommand("get-attribute", tclvisa_get_attribute);
	addCommand("clear", tclvisa_clear);
	addCommand("lock", tclvisa_lock);
	addCommand("unlock", tclvisa_unlock);
	addCommand("find", tclvisa_find);
	addCommand("write-from-file", write_from_file);
	addCommand("read-to-file", read_to_file);
	addCommand("parse-rsrc", tclvisa_parse_rsrc);
	addCommand("last-error", tclvisa_get_last_error);
	addCommand("assert-trigger", tclvisa_assert_trigger);
	addCommand("assert-intr-signal", tclvisa_assert_intr_signal);
	addCommand("assert-util-signal", tclvisa_assert_util_signal);
	addCommand("gpib-command", tclvisa_gpib_command);
	addCommand("gpib-control-atn", tclvisa_gpib_control_atn);
	addCommand("gpib-control-ren", tclvisa_gpib_control_ren);
	addCommand("gpib-pass-control", tclvisa_gpib_pass_control);
	addCommand("gpib-send-ifc", tclvisa_gpib_send_ifc);

	if (TCL_OK != setVisaConstants(interp, NAMESPACE, PACKAGE_VERSION)) {
		goto error;
	}

	return TCL_OK;

error:
	return TCL_ERROR;
}

#ifdef _WINDOWS
__declspec(dllexport)
#endif
int tclvisa_Init(Tcl_Interp* const interp) {
    /*
     * This may work with 8.0, but we are using strictly stubs here,
     * which requires 8.1.
     */
	if (
#ifdef USE_TCL_STUBS
		Tcl_InitStubs(interp, "9-10", 0)
#else
		Tcl_PkgRequire(interp, "Tcl", "9-10", 0)
#endif
			== NULL) {
		return TCL_ERROR;
    }
	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
		return TCL_ERROR;
    }
	return createTclvisaCommands(interp);
}

#ifdef _WINDLL

// DLL entry function (called on load, unload, ...)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {

	/* avoid "unused parameter" warning */
	UNREFERENCED_PARAMETER(hModule);	
	UNREFERENCED_PARAMETER(dwReason);	
	UNREFERENCED_PARAMETER(lpReserved);	

    return TRUE;
}

#endif	//	_WINDLL
