/*
 * visa_utils.c --
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

#include "visa_utils.h"

const char* visaErrorMessage(ViStatus status) {
	switch (status) {
	case VI_ERROR_ALLOC:
		return "[VI_ERROR_ALLOC] Insufficient system resources to open a session.";
	case VI_ERROR_ASRL_FRAMING:
		return "[VI_ERROR_ASRL_FRAMING] A framing error occurred during transfer.";
	case VI_ERROR_ASRL_OVERRUN:
		return "[VI_ERROR_ASRL_OVERRUN] An overrun error occurred during transfer. A character was not read from the hardware before the next character arrived.";
	case VI_ERROR_ASRL_PARITY:
		return "[VI_ERROR_ASRL_PARITY] A parity error occurred during transfer.";
	case VI_ERROR_ATTR_READONLY:
		return "[VI_ERROR_ATTR_READONLY] The specified attribute is read-only.";
	case VI_ERROR_BERR:
		return "[VI_ERROR_BERR] Bus error occurred during transfer.";
	case VI_ERROR_CLOSING_FAILED:
		return "[VI_ERROR_CLOSING_FAILED] Unable to deallocate the previously allocated data structures corresponding to this session or object reference.";
	case VI_ERROR_CONN_LOST:
		return "[VI_ERROR_CONN_LOST] The I/O connection for the given session has been lost.";
	case VI_ERROR_FILE_ACCESS:
		return "An error occurred while trying to open the specified file. Possible reasons include an invalid path or lack of access rights.";
	case VI_ERROR_FILE_IO:
		return "An error occurred while accessing the specified file.";
	case VI_ERROR_INP_PROT_VIOL:
		return "[VI_ERROR_INP_PROT_VIOL] Device reported an input protocol error during transfer.";
	case VI_ERROR_INTF_NUM_NCONFIG:
		return "[VI_ERROR_INTF_NUM_NCONFIG] The interface type is valid, but the specified interface number is not configured.";
	case VI_ERROR_INV_ACC_MODE:
		return "[VI_ERROR_INV_ACC_MODE] Invalid access mode.";
	case VI_ERROR_INV_EXPR:
		return "[VI_ERROR_INV_EXPR] Invalid expression specified for search.";
	case VI_ERROR_INV_MASK:
		return "[VI_ERROR_INV_MASK] The specified mask does not specify a valid flush operation on read/write resource.";
	case VI_ERROR_INV_OBJECT:
		return "[VI_ERROR_INV_OBJECT] The given session reference is invalid.";
	case VI_ERROR_INV_RSRC_NAME:
		return "[VI_ERROR_INV_RSRC_NAME] Invalid resource reference specified. Parsing error.";
	case VI_ERROR_INV_SETUP:
		return "[VI_ERROR_INV_SETUP] Some implementation-specific configuration file is corrupt or does not exist.";
	case VI_ERROR_IO:
		return "[VI_ERROR_IO] Could not perform read/write operation because of I/O error.";
	case VI_ERROR_LIBRARY_NFOUND:
		return "[VI_ERROR_LIBRARY_NFOUND] A code library required by VISA could not be located or loaded.";
	case VI_ERROR_NCIC:
		return "[VI_ERROR_NCIC] The interface associated with the given vi is not currently the controller in charge.";
	case VI_ERROR_NLISTENERS:
		return "[VI_ERROR_NLISTENERS] No-listeners condition is detected (both NRFD and NDAC are unasserted).";
	case VI_ERROR_NSUP_ATTR:
		return "[VI_ERROR_NSUP_ATTR] The specified attribute is not defined by the referenced object.";
	case VI_ERROR_NSUP_ATTR_STATE:
		return "[VI_ERROR_NSUP_ATTR_STATE] The specified state of the attribute is not valid, or is not supported as defined by the object.";
	case VI_ERROR_NSUP_OPER:
		return "[VI_ERROR_NSUP_OPER] The given session does not support this operation. This operation is supported only by a Resource Manager session.";
	case VI_ERROR_OUTP_PROT_VIOL:
		return "[VI_ERROR_OUTP_PROT_VIOL] Device reported an output protocol error during transfer.";
	case VI_ERROR_RAW_RD_PROT_VIOL:
		return "[VI_ERROR_RAW_RD_PROT_VIOL] Violation of raw read protocol occurred during transfer.";
	case VI_ERROR_RAW_WR_PROT_VIOL:
		return "[VI_ERROR_RAW_WR_PROT_VIOL] Violation of raw write protocol occurred during transfer.";
	case VI_ERROR_RSRC_BUSY:
		return "[VI_ERROR_RSRC_BUSY] The resource is valid, but VISA cannot currently access it.";
	case VI_ERROR_RSRC_LOCKED:
		return "[VI_ERROR_RSRC_LOCKED] Specified operation could not be performed because the resource identified by vi has been locked for this kind of access.";
	case VI_ERROR_RSRC_NFOUND:
		return "[VI_ERROR_RSRC_NFOUND] Insufficient location information or resource not present in the system.";
	case VI_ERROR_SYSTEM_ERROR:
		return "[VI_ERROR_SYSTEM_ERROR] The VISA system failed to initialize.";
	case VI_ERROR_TMO:
		return "[VI_ERROR_TMO] The read/write operation was aborted because timeout expired while operation was in progress.";
	case VI_WARN_NSUP_ATTR_STATE:
		return "[VI_WARN_NSUP_ATTR_STATE] Although the specified attribute state is valid, it is not supported by this implementation.";
	default:
		return "Unknown VISA error.";
	}
}

const char* tclvisaErrorMessage(int error) {
	switch (error) {
	case TCLVISA_ERROR_NULL_DATA:
		return "Channel data passed are null";
	case TCLVISA_ERROR_BAD_CHANNEL:
		return "Argument passed is not a valid VISA channel";
	default:
		return "Unknown Tclvisa error.";
	}
}
