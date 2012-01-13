/*
 * visa_channel.c --
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
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include "visa_utils.h"
#include "visa_channel.h"
#include "tcl_utils.h"
#include "tclvisa_utils.h"

#ifndef _WINDOWS
#include <strings.h>
#define _strcmpi strcasecmp
#endif

#define TCLVISA_NAME_PREFIX "visa_session"
#define TCLVISA_GET_OPTIONS "handshake mode queue timeout ttystatus xchar"
#define TCLVISA_SET_OPTIONS "handshake mode timeout ttycontrol xchar"
#define TCLVISA_OPTION_MODE "-mode"
#define TCLVISA_OPTION_TIMEOUT "-timeout"
#define TCLVISA_OPTION_FLOW "-handshake"
#define TCLVISA_OPTION_XCHAR "-xchar"
#define TCLVISA_OPTION_TTY_STATUS "-ttystatus"
#define TCLVISA_OPTION_TTY_CONTROL "-ttycontrol"
#define TCLVISA_OPTION_QUEUE "-queue"

#define VISA_MAX_BUF_SIZE	((unsigned int) 0xFFFFFFFF)

typedef struct TtyAttrs {
	int baud;
	int parity;
	int data;
	int stop;
} TtyAttrs;

/* forward declaration of channel internal handlers */
static int closeProc(ClientData instanceData, Tcl_Interp *interp);
static int blockModeProc(ClientData instanceData, int mode);
static int inputProc(ClientData instanceData, char *buf, int bufSize, int *errorCodePtr);
static int outputProc(ClientData instanceData, const char *buf, int toWrite, int *errorCodePtr);
static int setOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, const char *newValue);
static int getOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, Tcl_DString *dsPtr);
static void	watchProc(ClientData instanceData, int mask);
static int getHandleProc(ClientData instanceData, int direction, ClientData *handlePtr);

static int TtyParseMode(Tcl_Interp *interp,	const char *mode, int *speedPtr, int *parityPtr, int *dataPtr, int *stopPtr);
static int toVisaParity(int p);
static int toVisaStopBits(int s);
static int getTtyAttributes(Tcl_Interp *interp, VisaChannelData* data, TtyAttrs* tty);
static int setTtyAttributes(Tcl_Interp *interp, VisaChannelData* data, const TtyAttrs* tty);
static int toVisaFlow(Tcl_Interp *interp, const char* value);
static const char* fromVisaFlow(ViUInt16 flow);
static void fromVisaModemStatus(VisaChannelData* data, Tcl_DString *dsPtr);
static ViUInt16 toVisaModemStatus(int v);

/* VISA channel definition structure */
static Tcl_ChannelType visaChannelType = {
	"visa_session",	/* typeName */
	TCL_CHANNEL_VERSION_5,	/* version */
	&closeProc,	/* closeProc */
	&inputProc,	/* inputProc */
    &outputProc,	/* outputProc */
    NULL,	/* seekProc */
    &setOptionProc,	/* setOptionProc */
    &getOptionProc,	/* getOptionProc */
    &watchProc,	/* watchProc */
    &getHandleProc,	/* getHandleProc */
    NULL,	/* close2Proc */
    &blockModeProc,	/* blockModeProc */
    NULL,	/* flushProc */
    NULL,	/* handlerProc */
    NULL,	/* wideSeekProc */
    NULL,	/* threadActionProc */
    NULL	/* truncateProc */
};

VisaChannelData* createVisaChannel(Tcl_Interp* const interp, ViSession session) {
	Tcl_Channel channel;
	char channelName[16 + TCL_INTEGER_SPACE];
	VisaChannelData* data = (VisaChannelData*) malloc(sizeof(VisaChannelData));

	/* Create and fill internal channel data */
	data->session = session;
	data->blocking = 1;
	data->isRMSession = 0;

	/* Attempt to create Tcl channel */
	sprintf(channelName, "%s%u", TCLVISA_NAME_PREFIX, (unsigned) session);
	channel = Tcl_CreateChannel(&visaChannelType, channelName, (ClientData) data, TCL_READABLE | TCL_WRITABLE);
	if (NULL != channel) {
		/* Channel created successgully, register it for later use in Tcl IO procedures */
		Tcl_RegisterChannel(interp, channel);

		/* Configure channel  */
		Tcl_SetChannelOption(interp, channel, "-buffering", "line");
		Tcl_SetChannelOption(interp, channel, "-encoding", "binary");
		Tcl_SetChannelOption(interp, channel, "-translation", "binary");

		/* Store channel pointer */
		data->channel = channel;
	} else {
		/* Cannot create channel: free allocated resources */
		viClose(session);
		free(data);
		data = NULL;
	}

	return data;
}

VisaChannelData* getVisaChannelFromObj(Tcl_Interp* const interp, Tcl_Obj* objPtr) {
	VisaChannelData* data = NULL;
	Tcl_Channel channel = NULL;
	Tcl_ChannelType* type;
	int mode;

	/* Retrieve channel from object passed  */
	channel = Tcl_GetChannel(interp, TclGetString(objPtr), &mode);

	if (NULL != channel) {
		/* Retrieve channel type */
		type = Tcl_GetChannelType(channel);
		if (type == &visaChannelType) {
			/* Channel is of desired type, retrieve internal channel data */
			data = (VisaChannelData*) Tcl_GetChannelInstanceData(channel);
		} else {
			/* Notify about bad channel reference */
			Tcl_AppendResult(interp, tclvisaErrorMessage(TCLVISA_ERROR_BAD_CHANNEL), NULL);
		}
	}

	return data;
}

static VisaChannelData* validateData(ClientData instanceData, Tcl_Interp *interp) {
	VisaChannelData* data = (VisaChannelData*) instanceData;
	if (!data) {
		if (interp) {
			Tcl_AppendResult(interp, tclvisaErrorMessage(TCLVISA_ERROR_NULL_DATA), NULL);
		}
	}
	return data;
}

static int closeProc(ClientData instanceData, Tcl_Interp *interp) {
	ViStatus status;
	VisaChannelData* data = validateData(instanceData, interp);

	if (!data) {
		return TCL_ERROR;
	}

	if (!data->isRMSession) {
		viFlush(data->session, VI_WRITE_BUF | VI_IO_OUT_BUF);
	}
	status = viClose(data->session);
	if (status < 0) {
		if (interp) {
			Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
		}
	} else {
		free(data);
	}

	return status;
}

static int blockModeProc(ClientData instanceData, int mode) {
	VisaChannelData* data = validateData(instanceData, NULL);

	if (!data || data->isRMSession) {
		return -1;
	}

	switch (mode) {
	case TCL_MODE_BLOCKING:
		if (!data->blocking) {
			/* Restore saved timeout */
			data->blocking = 1;
			return setVisaTimeout(NULL, data, data->timeout) ? 0 : -1;
		}
		break;

	case TCL_MODE_NONBLOCKING:
		if (data->blocking) {
			/* Save current timeout value */
			int res = getVisaTimeout(NULL, data, &data->timeout);

			if (TCL_OK == res) {
				/* Set zero timeout */
				setVisaTimeout(NULL, data, VI_TMO_IMMEDIATE);
			}

			if (TCL_OK == res) {
				data->blocking = 0;
			}

			return res;
		}
		break;

	default:
		return -1;
	}

	return TCL_OK;
}

static int inputProc(ClientData instanceData, char *buf, int bufSize, int *errorCodePtr) {
	ViStatus status;
	ViUInt32 retCount = 0;
	int result;
	VisaChannelData* data = validateData(instanceData, NULL);

	if (!data || data->isRMSession) {
		return -1;
	}

	if ((ViInt64) bufSize > (ViInt64) VISA_MAX_BUF_SIZE) {
		/* restrict buffer size */
		bufSize = VISA_MAX_BUF_SIZE;
	}

	status = viRead(data->session, (ViPBuf) buf, (ViUInt32) bufSize, &retCount);
	result = (int) retCount;

	if (VI_ERROR_TMO == status) {
		if (!data->blocking && 0 == result) {
			*errorCodePtr = EAGAIN;
		}
	} else if (status < 0) {
		if (errorCodePtr) {
			*errorCodePtr = (int) status;
		}
		result = -1;
	}

	return result;
}

static int outputProc(ClientData instanceData, const char *buf, int toWrite, int *errorCodePtr) {
	ViStatus status;
	ViUInt32 retCount = 0;
	int result;
	VisaChannelData* data = validateData(instanceData, NULL);

	if (!data || data->isRMSession) {
		return -1;
	}

	if ((ViInt64) toWrite > (ViInt64) VISA_MAX_BUF_SIZE) {
		/* restrict buffer size */
		toWrite = VISA_MAX_BUF_SIZE;
	}

	status = viWrite(data->session, (ViBuf) buf, (ViUInt32) toWrite, &retCount);
	result = (int) retCount;

	if (VI_ERROR_TMO == status && !data->blocking) {
		if (0 == result) {
			*errorCodePtr = EAGAIN;
		}
	} else if (status < 0) {
		if (errorCodePtr) {
			*errorCodePtr = (int) status;
		}
		result = -1;
	}

	return result;
}

static int setOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, const char *newValue) {
    size_t len, vlen;
	ViStatus status;
	int argc;
	const char** argv;
	VisaChannelData* data = (VisaChannelData*) instanceData;

	if (!data || data->isRMSession) {
		return -1;
	}

	len = strlen(optionName);
	vlen = strlen(newValue);

    /*
     * Option -mode baud,parity,databits,stopbits
     */

	if ((len > 2) && (strncmp(optionName, TCLVISA_OPTION_MODE, len) == 0)) {
		TtyAttrs tty;
		if (TCL_OK != TtyParseMode(interp, newValue, &tty.baud, &tty.parity, &tty.data, &tty.stop)) {
			return TCL_ERROR;
		}

		if (TCL_OK != setTtyAttributes(interp, data, &tty)) {
			return TCL_ERROR;
		}

		return TCL_OK;
	}

    /*
     * Option -timeout msec
     */
	if (len > 2 && (strncmp(optionName, TCLVISA_OPTION_TIMEOUT, len) == 0)) {
		int msec;

		if (TCL_OK != Tcl_GetInt(interp, newValue, &msec)) {
			return TCL_ERROR;
		}

		return setVisaTimeout(interp, data, (ViUInt32) msec);
	}

    /*
     * Option -handshake none|xonxoff|rtscts|dtrdsr
     */
	if (len > 1 && strncmp(optionName, TCLVISA_OPTION_FLOW, len) == 0) {
		int flow = toVisaFlow(interp, newValue);
		if (flow >= 0) {
			status = viSetAttribute(data->session, VI_ATTR_ASRL_FLOW_CNTRL, (ViAttrState) flow);
			goto visa_result;
		}
		return TCL_ERROR;
    }

    /*
     * Option -xchar {\x11 \x13}
     */
	if (len > 1 && strncmp(optionName, TCLVISA_OPTION_XCHAR, len) == 0) {
		if (TCL_OK != Tcl_SplitList(interp, newValue, &argc, &argv)) {
			return TCL_ERROR;
		}

		if (argc == 2) {
			Tcl_DString ds;
			Tcl_DStringInit(&ds);

			Tcl_UtfToExternalDString(NULL, argv[0], -1, &ds);
			status = viSetAttribute(data->session, VI_ATTR_ASRL_XON_CHAR, (ViAttrState) *Tcl_DStringValue(&ds));
			Tcl_DStringSetLength(&ds, 0);

			if (status >= 0) {
				Tcl_UtfToExternalDString(NULL, argv[1], -1, &ds);
				status = viSetAttribute(data->session, VI_ATTR_ASRL_XOFF_CHAR, (ViAttrState) *Tcl_DStringValue(&ds));
			}

			Tcl_DStringFree(&ds);
			ckfree((char*) argv);
			goto visa_result;
		} else {
			if (interp) {
				Tcl_AppendResult(interp, "bad value for -xchar: should be a list of two elements", NULL);
			}
		}

		ckfree((char*) argv);
		return TCL_ERROR;
    }

    /*
     * Option -ttycontrol {DTR 1 RTS 0 BREAK 0}
     */
	if (len > 4 && strncmp(optionName, TCLVISA_OPTION_TTY_CONTROL, len) == 0) {
		int i, flag;

		if (Tcl_SplitList(interp, newValue, &argc, &argv) == TCL_ERROR) {
			return TCL_ERROR;
		}

		if ((argc % 2) == 1) {
			if (interp) {
				Tcl_AppendResult(interp, "bad value for -ttycontrol: should be a list of signal,value pairs", NULL);
			}
			ckfree((char *) argv);
			return TCL_ERROR;
		}

		for (i = 0; i < argc - 1; i += 2) {
			if (Tcl_GetBoolean(interp, argv[i + 1], &flag) == TCL_ERROR) {
				ckfree((char *) argv);
				return TCL_ERROR;
			}

			if (_strcmpi(argv[i], "DTR") == 0) {
				if ((status = viSetAttribute(data->session, VI_ATTR_ASRL_DTR_STATE, toVisaModemStatus(flag))) < 0) {
					ckfree((char *) argv);
					goto visa_result;
				}
			}

			else if (_strcmpi(argv[i], "RTS") == 0) {
				if ((status = viSetAttribute(data->session, VI_ATTR_ASRL_RTS_STATE, toVisaModemStatus(flag))) < 0) {
					ckfree((char *) argv);
					goto visa_result;
				}
			}

#ifdef VI_ATTR_ASRL_BREAK_STATE
			else if (_strcmpi(argv[i], "BREAK") == 0) {
				if ((status = viSetAttribute(data->session, VI_ATTR_ASRL_BREAK_STATE, toVisaModemStatus(flag))) < 0) {
					ckfree((char *) argv);
					goto visa_result;
				}
			}
#endif

			else {
				if (interp) {
					Tcl_AppendResult(interp, "bad signal \"", argv[i], "\" for -ttycontrol: must be DTR, RTS"
#ifdef VI_ATTR_ASRL_BREAK_STATE
						"or BREAK"
#endif
						, NULL);
				}
				ckfree((char *) argv);
				return TCL_ERROR;
			}
		} /* -ttycontrol options loop */

		ckfree((char *) argv);
		return TCL_OK;
	}

    return Tcl_BadChannelOption(interp, optionName, TCLVISA_SET_OPTIONS);

visa_result:
	/* Handle VISA return status */
	if (status < 0) {
		if (NULL != interp) {
			Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
		}
		return TCL_ERROR;
	} else {
		return TCL_OK;
	}
}

static int getOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, Tcl_DString *dsPtr) {
    size_t len;
    char buf[3*TCL_INTEGER_SPACE + 16];
    int valid = 0;		/* Flag if valid option parsed. */
	VisaChannelData* data = (VisaChannelData*) instanceData;

	if (!data || data->isRMSession || !optionName) {
		return -1;
	}

	len = strlen(optionName);

    /*
     * Option -mode baud,parity,databits,stopbits
     */
	if (len > 2 && strncmp(optionName, TCLVISA_OPTION_MODE, len) == 0) {
		TtyAttrs tty;
		int res = getTtyAttributes(interp, data, &tty);
		if (TCL_OK == res) {
			valid = 1;
			sprintf(buf, "%d,%c,%d,%d", tty.baud, tty.parity, tty.data, tty.stop);
			Tcl_DStringAppendElement(dsPtr, buf);
		}
		return res;
    }

    /*
     * Option -timeout msec
     */
	if (len > 2 && strncmp(optionName, TCLVISA_OPTION_TIMEOUT, len) == 0) {
		ViUInt32 timeout;
		int res = getVisaTimeout(interp, data, &timeout);
		if (TCL_OK == res) {
			valid = 1;
			sprintf(buf, "%d", (int) timeout);
			Tcl_DStringAppendElement(dsPtr, buf);
		}
		return res;
    }

    /*
     * Option -handshake none|xonxoff|rtscts|dtrdsr
     */
	if (len > 2 && strncmp(optionName, TCLVISA_OPTION_FLOW, len) == 0) {
		ViUInt16 flow;
		ViStatus status = viGetAttribute(data->session, VI_ATTR_ASRL_FLOW_CNTRL, &flow);

		if (status < 0) {
			if (interp) {
				Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
				return TCL_ERROR;
			}
		} else {
			valid = 1;
			sprintf(buf, "%s", fromVisaFlow(flow));
			Tcl_DStringAppendElement(dsPtr, buf);
		}
    }

    /*
     * Get option -xchar
     */
    if (len > 1 && strncmp(optionName, "-xchar", len) == 0) {
		Tcl_DString ds;
		ViUInt8 xon, xoff;
		ViStatus status1 = viGetAttribute(data->session, VI_ATTR_ASRL_XON_CHAR, &xon);
		ViStatus status2 = viGetAttribute(data->session, VI_ATTR_ASRL_XOFF_CHAR, &xoff);

		if (status1 >= 0 && status2 >= 0) {
			char s[2] = {0, 0};
			Tcl_DStringInit(&ds);

			s[0] = (char) xon;
			Tcl_ExternalToUtfDString(NULL, s, 1, &ds);
			Tcl_DStringAppendElement(dsPtr, (const char*) Tcl_DStringValue(&ds));
			Tcl_DStringSetLength(&ds, 0);

			s[0] = (char) xoff;
			Tcl_ExternalToUtfDString(NULL,  s, 1, &ds);
			Tcl_DStringAppendElement(dsPtr, (const char*) Tcl_DStringValue(&ds));
			Tcl_DStringFree(&ds);

			valid = 1;
		} else{
			if (interp) {
				Tcl_AppendResult(interp, visaErrorMessage(status1 < 0 ? status1 : status2), NULL);
			}
			return TCL_ERROR;
		}
    }

    /*
     * Get option -ttystatus
     * Option is readonly and returned by [fconfigure chan -ttystatus] but not
     * returned by unnamed [fconfigure chan].
     */
    if (len > 4 && strncmp(optionName, TCLVISA_OPTION_TTY_STATUS, len) == 0) {
		fromVisaModemStatus(data, dsPtr);
		valid = 1;
	}

    /*
     * Get option -queue
     * Option is readonly and returned by [fconfigure chan -queue] but not
     * returned by unnamed [fconfigure chan].
     */
	if (len > 1 && strncmp(optionName, TCLVISA_OPTION_QUEUE, len) == 0) {
		int inBuffered, outBuffered;
		ViUInt32 inQueue;
		ViStatus status = viGetAttribute(data->session, VI_ATTR_ASRL_AVAIL_NUM, &inQueue);

		if (status < 0) {
			if (interp) {
				Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
			}
			return TCL_ERROR;
		}

		inBuffered = Tcl_InputBuffered(data->channel);
		outBuffered = Tcl_OutputBuffered(data->channel);

		sprintf(buf, "%d", (int) (inBuffered + inQueue));
		Tcl_DStringAppendElement(dsPtr, buf);
		sprintf(buf, "%d", outBuffered);
		Tcl_DStringAppendElement(dsPtr, buf);

		valid = 1;
	}

	if (valid) {
		return TCL_OK;
    }

    return Tcl_BadChannelOption(interp, optionName, TCLVISA_GET_OPTIONS);
}

static void	watchProc(ClientData instanceData, int mask) {
	VisaChannelData* data = (VisaChannelData*) instanceData;

	if (!data || data->isRMSession) {
		return;
	}

	/* avoid "unused parameter" warning */
	UNREFERENCED_PARAMETER(mask);	
}

static int getHandleProc(ClientData instanceData, int direction, ClientData *handlePtr) {
	VisaChannelData* data = (VisaChannelData*) instanceData;

	if (!data || data->isRMSession) {
		return -1;
	}

	/* avoid "unused parameter" warning */
	UNREFERENCED_PARAMETER(direction);	
	UNREFERENCED_PARAMETER(handlePtr);	

	return TCL_OK;
}

/* Code is taken from Tcl source file tclUnixChan.c
 *---------------------------------------------------------------------------
 *
 * TtyParseMode --
 *
 *	Parse the "-mode" argument to the fconfigure command. The argument is
 *	of the form baud,parity,data,stop.
 *
 * Results:
 *	The return value is TCL_OK if the argument was successfully parsed,
 *	TCL_ERROR otherwise. If TCL_ERROR is returned, an error message is
 *	left in the interp's result (if interp is non-NULL).
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static int
TtyParseMode(
    Tcl_Interp *interp,		/* If non-NULL, interp for error return. */
    const char *mode,		/* Mode string to be parsed. */
    int *speedPtr,		/* Filled with baud rate from mode string. */
    int *parityPtr,		/* Filled with parity from mode string. */
    int *dataPtr,		/* Filled with data bits from mode string. */
    int *stopPtr)		/* Filled with stop bits from mode string. */
{
    int i, end;
    char parity;
    static const char *bad = "bad value for -mode";

	i = sscanf(mode, "%d,%c,%d,%d%n", speedPtr, &parity, dataPtr, stopPtr, &end);
	if ((i != 4) || (mode[end] != '\0')) {
		if (interp != NULL) {
			Tcl_AppendResult(interp, bad, ": should be baud,parity,data,stop", NULL);
		}
		return TCL_ERROR;
    }

    if (strchr("noems", parity) == NULL) {
		if (interp != NULL) {
			Tcl_AppendResult(interp, bad, " parity: should be n, o, e, m, or s", NULL);
		}
		return TCL_ERROR;
    }
	*parityPtr = parity;

    if ((*dataPtr < 5) || (*dataPtr > 8)) {
		if (interp != NULL) {
			Tcl_AppendResult(interp, bad, " data: should be 5, 6, 7, or 8", NULL);
		}
		return TCL_ERROR;
    }

    if ((*stopPtr < 1) || (*stopPtr > 2)) {
		if (interp != NULL) {
			Tcl_AppendResult(interp, bad, " stop: should be 1 or 2", NULL);
		}
		return TCL_ERROR;
    }

    return TCL_OK;
}

static int toVisaParity(int p) {
	switch (p) {
		case 'n': return VI_ASRL_PAR_NONE;
		case 'o': return VI_ASRL_PAR_ODD;
		case 'e': return VI_ASRL_PAR_EVEN;
		case 'm': return VI_ASRL_PAR_MARK;
		case 's': return VI_ASRL_PAR_SPACE;
	}
	return -1;
}

static int fromVisaParity(int p) {
	switch (p) {
		case VI_ASRL_PAR_NONE: return 'n';
		case VI_ASRL_PAR_ODD: return 'o';
		case VI_ASRL_PAR_EVEN: return 'e';
		case VI_ASRL_PAR_MARK: return 'm';
		case VI_ASRL_PAR_SPACE: return 's';
	}
	return -1;
}

static int toVisaStopBits(int s) {
	switch (s) {
		case 1: return VI_ASRL_STOP_ONE;
		case 2: return VI_ASRL_STOP_TWO;
	}
	return -1;
}

static int fromVisaStopBits(int s) {
	switch (s) {
		case VI_ASRL_STOP_ONE: return 1;
		case VI_ASRL_STOP_TWO: return 2;
	}
	return -1;
}

static int getTtyAttributes(Tcl_Interp *interp, VisaChannelData* data, TtyAttrs* tty) {
	ViStatus status;
	ViUInt32 v;

	v = 0;
	status = viGetAttribute(data->session, VI_ATTR_ASRL_BAUD, &v);
	if (status < 0) {
		goto error;
	}
	tty->baud = (int) v;

	v = 0;
	status = viGetAttribute(data->session, VI_ATTR_ASRL_PARITY, &v);
	if (status < 0) {
		goto error;
	}
	tty->parity = fromVisaParity((int) v);

	v = 0;
	status = viGetAttribute(data->session, VI_ATTR_ASRL_DATA_BITS, &v);
	if (status < 0) {
		goto error;
	}
	tty->data = (int) v;

	v = 0;
	status = viGetAttribute(data->session, VI_ATTR_ASRL_STOP_BITS, &v);
	if (status < 0) {
		goto error;
	} 
	tty->stop = fromVisaStopBits((int) v);

	return TCL_OK;

error:
	if (interp != NULL) {
		Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
	}
	return TCL_ERROR;
}

static int setTtyAttributes(Tcl_Interp *interp, VisaChannelData* data, const TtyAttrs* tty) {
	ViStatus status;

	status = viSetAttribute(data->session, VI_ATTR_ASRL_BAUD, (ViAttrState) tty->baud);
	if (status < 0) {
		goto error;
	}
	status = viSetAttribute(data->session, VI_ATTR_ASRL_PARITY, (ViAttrState) toVisaParity(tty->parity));
	if (status < 0) {
		goto error;
	}
	status = viSetAttribute(data->session, VI_ATTR_ASRL_DATA_BITS, (ViAttrState) tty->data);
	if (status < 0) {
		goto error;
	}
	status = viSetAttribute(data->session, VI_ATTR_ASRL_STOP_BITS, (ViAttrState) toVisaStopBits(tty->stop));
	if (status < 0) {
		goto error;
	}

	return TCL_OK;

error:
	if (NULL != interp) {
		Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
	}

	return TCL_ERROR;
}

int getVisaTimeout(Tcl_Interp *interp, VisaChannelData* data, ViUInt32* timeout) {
	/* In non-blocking mode timeouts are set automatically */
	if (!data->blocking) {
		/* Returns saved value rather than actual device timeout */
		*timeout = (int) data->timeout;
	} else {
		/* Attempt to get attribute */
		ViStatus status = viGetAttribute(data->session, (ViAttr) VI_ATTR_TMO_VALUE, timeout);
		if (status < 0) {
			Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}

int setVisaTimeout(Tcl_Interp *interp, VisaChannelData* data, ViUInt32 timeout) {
	/* In non-blocking mode timeouts are set automatically */
	if (!data->blocking) {
		/* Save specified specified value for later use */
		data->timeout = timeout;
	} else {
		/* Attempt to set attribute */
		ViStatus status = viSetAttribute(data->session, (ViAttr) VI_ATTR_TMO_VALUE, (ViAttrState) timeout);
		if (status < 0) {
			Tcl_AppendResult(interp, visaErrorMessage(status), NULL);
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}

static int toVisaFlow(Tcl_Interp *interp, const char* value) {
	if (_strcmpi(value, "NONE") == 0) {
		return VI_ASRL_FLOW_NONE;
	} else if (_strcmpi(value, "XONXOFF") == 0) {
		return VI_ASRL_FLOW_XON_XOFF;
	} else if (_strcmpi(value, "RTSCTS") == 0) {
		return VI_ASRL_FLOW_RTS_CTS;
	} else if (_strcmpi(value, "DTRDSR") == 0) {
		return VI_ASRL_FLOW_DTR_DSR;
	} else {
	    if (interp) {
			Tcl_AppendResult(interp, "bad value for -handshake: must be one of xonxoff, rtscts, dtrdsr or none", NULL);
	    }
	    return -1;
	}
}

static const char* fromVisaFlow(ViUInt16 flow) {
	switch (flow) {
		case VI_ASRL_FLOW_NONE: return "none";
		case VI_ASRL_FLOW_XON_XOFF: return "xonxoff";
		case VI_ASRL_FLOW_RTS_CTS: return "rtscts";
		case VI_ASRL_FLOW_DTR_DSR: return "dtrdsr";
		default: return "none";
	}
}

static const char* getModemBitStatus(ViUInt16 v) {
	switch (v) {
		case VI_STATE_ASSERTED: return "1";
		case VI_STATE_UNASSERTED: return "0";
		default: return "x";
	}
}

static void fromVisaModemStatus(VisaChannelData* data, Tcl_DString *dsPtr) {
	ViStatus status;
	ViUInt16 v;

	status = viGetAttribute(data->session, VI_ATTR_ASRL_CTS_STATE, &v);
	if (status >= 0) {
		Tcl_DStringAppendElement(dsPtr, "CTS");
		Tcl_DStringAppendElement(dsPtr, getModemBitStatus(v));
	}

	status = viGetAttribute(data->session, VI_ATTR_ASRL_DSR_STATE, &v);
	if (status >= 0) {
		Tcl_DStringAppendElement(dsPtr, "DSR");
		Tcl_DStringAppendElement(dsPtr, getModemBitStatus(v));
	}

	status = viGetAttribute(data->session, VI_ATTR_ASRL_RI_STATE, &v);
	if (status >= 0) {
		Tcl_DStringAppendElement(dsPtr, "RING");
		Tcl_DStringAppendElement(dsPtr, getModemBitStatus(v));
	}

	status = viGetAttribute(data->session, VI_ATTR_ASRL_DCD_STATE, &v);
	if (status >= 0) {
		Tcl_DStringAppendElement(dsPtr, "DCD");
		Tcl_DStringAppendElement(dsPtr, getModemBitStatus(v));
	}
}

static ViUInt16 toVisaModemStatus(int v) {
	return v ? VI_STATE_ASSERTED : VI_STATE_UNASSERTED;
}
