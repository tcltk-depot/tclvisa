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
#include <windows.h>
#include "tclvisa_utils.h"

#define NAMESPACE "visa::"

/*
 * Forward declarations
 */
int tclvisa_open(const ClientData clientData, Tcl_Interp * const interp, const int objc, Tcl_Obj *CONST objv[]);
int tclvisa_open_default_rm(const ClientData clientData, Tcl_Interp * const interp, const int objc, Tcl_Obj *CONST objv[]);
int tclvisa_set_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);
int tclvisa_get_attribute(const ClientData clientData, Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[]);

int setVisaConstants(Tcl_Interp* const interp, const char* prefix);

#define addCommand(tcl_name, proc)	\
	Tcl_CreateObjCommand(interp, NAMESPACE tcl_name, proc, NULL, NULL)

int createTclvisaCommands(Tcl_Interp* const interp) {
	addCommand("open", tclvisa_open);
	addCommand("open-default-rm", tclvisa_open_default_rm);
	addCommand("set-attribute", tclvisa_set_attribute);
	addCommand("get-attribute", tclvisa_get_attribute);

	setVisaConstants(interp, NAMESPACE);

	return TCL_OK;
}

__declspec(dllexport) int Tclvisa_Init(Tcl_Interp* const interp) {
    /*
     * This may work with 8.0, but we are using strictly stubs here,
     * which requires 8.1.
     */
/*	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	} */
	if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL) {
		return TCL_ERROR;
    }
	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
		return TCL_ERROR;
    }
	return createTclvisaCommands(interp);
}

// DLL entry function (called on load, unload, ...)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {

	/* avoid "unused parameter" warning */
	UNREFERENCED_PARAMETER(hModule);	
	UNREFERENCED_PARAMETER(dwReason);	
	UNREFERENCED_PARAMETER(lpReserved);	

    return TRUE;
}
