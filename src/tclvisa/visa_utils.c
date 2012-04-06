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
#ifdef VI_ERROR_ALLOC
	case VI_ERROR_ALLOC:
		return "[VI_ERROR_ALLOC] Insufficient system resources to open a session.";
#endif
#ifdef VI_ERROR_ASRL_FRAMING
	case VI_ERROR_ASRL_FRAMING:
		return "[VI_ERROR_ASRL_FRAMING] A framing error occurred during transfer.";
#endif
#ifdef VI_ERROR_ASRL_OVERRUN
	case VI_ERROR_ASRL_OVERRUN:
		return "[VI_ERROR_ASRL_OVERRUN] An overrun error occurred during transfer. A character was not read from the hardware before the next character arrived.";
#endif
#ifdef VI_ERROR_ASRL_PARITY
	case VI_ERROR_ASRL_PARITY:
		return "[VI_ERROR_ASRL_PARITY] A parity error occurred during transfer.";
#endif
#ifdef VI_ERROR_ATTR_READONLY
	case VI_ERROR_ATTR_READONLY:
		return "[VI_ERROR_ATTR_READONLY] The specified attribute is read-only.";
#endif
#ifdef VI_ERROR_BERR
	case VI_ERROR_BERR:
		return "[VI_ERROR_BERR] Bus error occurred during transfer.";
#endif
#ifdef VI_ERROR_CLOSING_FAILED
	case VI_ERROR_CLOSING_FAILED:
		return "[VI_ERROR_CLOSING_FAILED] Unable to deallocate the previously allocated data structures corresponding to this session or object reference.";
#endif
#ifdef VI_ERROR_CONN_LOST
	case VI_ERROR_CONN_LOST:
		return "[VI_ERROR_CONN_LOST] The I/O connection for the given session has been lost.";
#endif
#ifdef VI_ERROR_FILE_ACCESS
	case VI_ERROR_FILE_ACCESS:
		return "An error occurred while trying to open the specified file. Possible reasons include an invalid path or lack of access rights.";
#endif
#ifdef VI_ERROR_FILE_IO
	case VI_ERROR_FILE_IO:
		return "An error occurred while accessing the specified file.";
#endif
#ifdef VI_ERROR_INP_PROT_VIOL
	case VI_ERROR_INP_PROT_VIOL:
		return "[VI_ERROR_INP_PROT_VIOL] Device reported an input protocol error during transfer.";
#endif
#ifdef VI_ERROR_INTF_NUM_NCONFIG
	case VI_ERROR_INTF_NUM_NCONFIG:
		return "[VI_ERROR_INTF_NUM_NCONFIG] The interface type is valid, but the specified interface number is not configured.";
#endif
#ifdef VI_ERROR_INV_ACC_MODE
	case VI_ERROR_INV_ACC_MODE:
		return "[VI_ERROR_INV_ACC_MODE] Invalid access mode.";
#endif
#ifdef VI_ERROR_INV_EXPR
	case VI_ERROR_INV_EXPR:
		return "[VI_ERROR_INV_EXPR] Invalid expression specified for search.";
#endif
#ifdef VI_ERROR_INV_MASK
	case VI_ERROR_INV_MASK:
		return "[VI_ERROR_INV_MASK] The specified mask does not specify a valid flush operation on read/write resource.";
#endif
#ifdef VI_ERROR_INV_OBJECT
	case VI_ERROR_INV_OBJECT:
		return "[VI_ERROR_INV_OBJECT] The given session reference is invalid.";
#endif
#ifdef VI_ERROR_INV_RSRC_NAME
	case VI_ERROR_INV_RSRC_NAME:
		return "[VI_ERROR_INV_RSRC_NAME] Invalid resource reference specified. Parsing error.";
#endif
#ifdef VI_ERROR_INV_SETUP
	case VI_ERROR_INV_SETUP:
		return "[VI_ERROR_INV_SETUP] Some implementation-specific configuration file is corrupt or does not exist.";
#endif
#ifdef VI_ERROR_IO
	case VI_ERROR_IO:
		return "[VI_ERROR_IO] Could not perform read/write operation because of I/O error.";
#endif
#ifdef VI_ERROR_LIBRARY_NFOUND
	case VI_ERROR_LIBRARY_NFOUND:
		return "[VI_ERROR_LIBRARY_NFOUND] A code library required by VISA could not be located or loaded.";
#endif
#ifdef VI_ERROR_NCIC
	case VI_ERROR_NCIC:
		return "[VI_ERROR_NCIC] The interface associated with the given vi is not currently the controller in charge.";
#endif
#ifdef VI_ERROR_NLISTENERS
	case VI_ERROR_NLISTENERS:
		return "[VI_ERROR_NLISTENERS] No-listeners condition is detected (both NRFD and NDAC are unasserted).";
#endif
#ifdef VI_ERROR_NSUP_ATTR
	case VI_ERROR_NSUP_ATTR:
		return "[VI_ERROR_NSUP_ATTR] The specified attribute is not defined by the referenced object.";
#endif
#ifdef VI_ERROR_NSUP_ATTR_STATE
	case VI_ERROR_NSUP_ATTR_STATE:
		return "[VI_ERROR_NSUP_ATTR_STATE] The specified state of the attribute is not valid, or is not supported as defined by the object.";
#endif
#ifdef VI_ERROR_NSUP_OPER
	case VI_ERROR_NSUP_OPER:
		return "[VI_ERROR_NSUP_OPER] The given session does not support this operation.";
#endif
#ifdef VI_ERROR_OUTP_PROT_VIOL
	case VI_ERROR_OUTP_PROT_VIOL:
		return "[VI_ERROR_OUTP_PROT_VIOL] Device reported an output protocol error during transfer.";
#endif
#ifdef VI_ERROR_RAW_RD_PROT_VIOL
	case VI_ERROR_RAW_RD_PROT_VIOL:
		return "[VI_ERROR_RAW_RD_PROT_VIOL] Violation of raw read protocol occurred during transfer.";
#endif
#ifdef VI_ERROR_RAW_WR_PROT_VIOL
	case VI_ERROR_RAW_WR_PROT_VIOL:
		return "[VI_ERROR_RAW_WR_PROT_VIOL] Violation of raw write protocol occurred during transfer.";
#endif
#ifdef VI_ERROR_RSRC_BUSY
	case VI_ERROR_RSRC_BUSY:
		return "[VI_ERROR_RSRC_BUSY] The resource is valid, but VISA cannot currently access it.";
#endif
#ifdef VI_ERROR_RSRC_LOCKED
	case VI_ERROR_RSRC_LOCKED:
		return "[VI_ERROR_RSRC_LOCKED] Specified operation could not be performed because the resource identified by vi has been locked for this kind of access.";
#endif
#ifdef VI_ERROR_RSRC_NFOUND
	case VI_ERROR_RSRC_NFOUND:
		return "[VI_ERROR_RSRC_NFOUND] Insufficient location information or resource not present in the system.";
#endif
#ifdef VI_ERROR_SYSTEM_ERROR
	case VI_ERROR_SYSTEM_ERROR:
		return "[VI_ERROR_SYSTEM_ERROR] The VISA system failed to initialize.";
#endif
#ifdef VI_ERROR_TMO
	case VI_ERROR_TMO:
		return "[VI_ERROR_TMO] The read/write operation was aborted because timeout expired while operation was in progress.";
#endif
#ifdef VI_WARN_NSUP_ATTR_STATE
	case VI_WARN_NSUP_ATTR_STATE:
		return "[VI_WARN_NSUP_ATTR_STATE] Although the specified attribute state is valid, it is not supported by this implementation.";
#endif
#ifdef VI_ERROR_QUEUE_ERROR
	case VI_ERROR_QUEUE_ERROR:
		return "[VI_ERROR_QUEUE_ERROR] Unable to queue move operation.";
#endif
#ifdef VI_ERROR_INV_FMT
	case VI_ERROR_INV_FMT:
		return "[VI_ERROR_INV_FMT] A format specifier in the format string is invalid.";
#endif
#ifdef VI_ERROR_NSUP_FMT
	case VI_ERROR_NSUP_FMT:
		return "[VI_ERROR_NSUP_FMT] A format specifier in the format string is not supported.";
#endif
#ifdef VI_ERROR_LINE_IN_USE
	case VI_ERROR_LINE_IN_USE:
		return "[VI_ERROR_LINE_IN_USE] The specified trigger line is currently in use.";
#endif
#ifdef VI_ERROR_NSUP_MODE
	case VI_ERROR_NSUP_MODE:
		return "[VI_ERROR_NSUP_MODE] The specified mode is not supported by this VISA implementation.";
#endif
#ifdef VI_ERROR_SRQ_NOCCURRED
	case VI_ERROR_SRQ_NOCCURRED:
		return "[VI_ERROR_SRQ_NOCCURRED] Service request has not been received for the session.";
#endif
#ifdef VI_ERROR_INV_SPACE
	case VI_ERROR_INV_SPACE:
		return "[VI_ERROR_INV_SPACE] Invalid address space specified.";
#endif
#ifdef VI_ERROR_INV_OFFSET
	case VI_ERROR_INV_OFFSET:
		return "[VI_ERROR_INV_OFFSET] Invalid offset specified.";
#endif
#ifdef VI_ERROR_INV_WIDTH
	case VI_ERROR_INV_WIDTH:
		return "[VI_ERROR_INV_WIDTH] Invalid source or destination width specified.";
#endif
#ifdef VI_ERROR_NSUP_OFFSET
	case VI_ERROR_NSUP_OFFSET:
		return "[VI_ERROR_NSUP_OFFSET] Specified offset is not accessible from this hardware.";
#endif
#ifdef VI_ERROR_NSUP_VAR_WIDTH
	case VI_ERROR_NSUP_VAR_WIDTH:
		return "[VI_ERROR_NSUP_VAR_WIDTH] Cannot support source and destination widths that are different.";
#endif
#ifdef VI_ERROR_WINDOW_NMAPPED
	case VI_ERROR_WINDOW_NMAPPED:
		return "[VI_ERROR_WINDOW_NMAPPED] The specified session is not currently mapped.";
#endif
#ifdef VI_ERROR_RESP_PENDING
	case VI_ERROR_RESP_PENDING:
		return "[VI_ERROR_RESP_PENDING] A previous response is still pending, causing a multiple query error.";
#endif
#ifdef VI_ERROR_NSYS_CNTLR
	case VI_ERROR_NSYS_CNTLR:
		return "[VI_ERROR_NSYS_CNTLR] The interface associated with this session is not the system controller.";
#endif
#ifdef VI_ERROR_INTR_PENDING
	case VI_ERROR_INTR_PENDING:
		return "[VI_ERROR_INTR_PENDING] An interrupt is still pending from a previous call.";
#endif
#ifdef VI_ERROR_TRIG_NMAPPED
	case VI_ERROR_TRIG_NMAPPED:
		return "[VI_ERROR_TRIG_NMAPPED] The path from trigSrc to trigDest is not currently mapped.";
#endif
#ifdef VI_ERROR_NSUP_ALIGN_OFFSET
	case VI_ERROR_NSUP_ALIGN_OFFSET:
		return "[VI_ERROR_NSUP_ALIGN_OFFSET] The specified offset is not properly aligned for the access width of the operation.";
#endif
#ifdef VI_ERROR_USER_BUF
	case VI_ERROR_USER_BUF:
		return "[VI_ERROR_USER_BUF] A specified user buffer is not valid or cannot be accessed for the required size.";
#endif
#ifdef VI_ERROR_NSUP_WIDTH
	case VI_ERROR_NSUP_WIDTH:
		return "[VI_ERROR_NSUP_WIDTH] Specified width is not supported by this hardware.";
#endif
#ifdef VI_ERROR_INV_PARAMETER
	case VI_ERROR_INV_PARAMETER:
		return "[VI_ERROR_INV_PARAMETER] The value of some parameter - which parameter is not known - is invalid.";
#endif
#ifdef VI_ERROR_INV_PROT
	case VI_ERROR_INV_PROT:
		return "[VI_ERROR_INV_PROT] The protocol specified is invalid.";
#endif
#ifdef VI_ERROR_INV_SIZE
	case VI_ERROR_INV_SIZE:
		return "[VI_ERROR_INV_SIZE] Invalid size of window specified.";
#endif
#ifdef VI_ERROR_WINDOW_MAPPED
	case VI_ERROR_WINDOW_MAPPED:
		return "[VI_ERROR_WINDOW_MAPPED] The specified session currently contains a mapped window.";
#endif
#ifdef VI_ERROR_NIMPL_OPER
	case VI_ERROR_NIMPL_OPER:
		return "[VI_ERROR_NIMPL_OPER] The given operation is not implemented.";
#endif
#ifdef VI_ERROR_INV_LENGTH
	case VI_ERROR_INV_LENGTH:
		return "[VI_ERROR_INV_LENGTH] Invalid length specified.";
#endif
#ifdef VI_ERROR_INV_MODE
	case VI_ERROR_INV_MODE:
		return "[VI_ERROR_INV_MODE] The specified mode is invalid.";
#endif
#ifdef VI_ERROR_SESN_NLOCKED
	case VI_ERROR_SESN_NLOCKED:
		return "[VI_ERROR_SESN_NLOCKED] The current session did not have any lock on the resource.";
#endif
#ifdef VI_ERROR_MEM_NSHARED
	case VI_ERROR_MEM_NSHARED:
		return "[VI_ERROR_MEM_NSHARED] The device does not export any memory.";
#endif
#ifdef VI_ERROR_NSUP_INTR
	case VI_ERROR_NSUP_INTR:
		return "[VI_ERROR_NSUP_INTR] The interface cannot generate an interrupt on the requested level or with the requested statusID value.";
#endif
#ifdef VI_ERROR_INV_LINE
	case VI_ERROR_INV_LINE:
		return "[VI_ERROR_INV_LINE] The value specified by the line parameter is invalid.";
#endif
#ifdef VI_ERROR_NSUP_LINE
	case VI_ERROR_NSUP_LINE:
		return "[VI_ERROR_NSUP_LINE] One of the specified lines (trigSrc or trigDest) is not supported by this VISA implementation, or the combination of lines is not a valid mapping.";
#endif
#ifdef VI_ERROR_NSUP_MECH
	case VI_ERROR_NSUP_MECH:
		return "[VI_ERROR_NSUP_MECH] The specified mechanism is not supported by the given event type.";
#endif
#ifdef VI_ERROR_MACHINE_NAVAIL
	case VI_ERROR_MACHINE_NAVAIL:
		return "[VI_ERROR_MACHINE_NAVAIL] The remotemachine does not exist or is not accepting any connections.";
#endif
#ifdef VI_ERROR_NPERMISSION
	case VI_ERROR_NPERMISSION:
		return "[VI_ERROR_NPERMISSION] Access to the remote machine is denied.";
#endif

	default:
		return "[UNKNOWN] Unknown VISA error.";
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
