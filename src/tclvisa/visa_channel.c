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

#define TCLVISA_NAME_PREFIX "visa_session"

#define VISA_BLOCKING	1
#define VISA_NON_BLOCKING	0

#define VISA_MAX_BUF_SIZE	((unsigned int) 0xFFFFFFFF)

#define TCLVISA_OPTION_MODE "mode"

/* forward declaration of channel internal handlers */
static int closeProc(ClientData instanceData, Tcl_Interp *interp);
static int blockModeProc(ClientData instanceData, int mode);
static int inputProc(ClientData instanceData, char *buf, int bufSize, int *errorCodePtr);
static int outputProc(ClientData instanceData, const char *buf, int toWrite, int *errorCodePtr);
static int setOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, const char *newValue);
static int getOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, Tcl_DString *dsPtr);
static void	watchProc(ClientData instanceData, int mask);
static int getHandleProc(ClientData instanceData, int direction, ClientData *handlePtr);

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

static const char* tclVisaOptions = TCLVISA_OPTION_MODE;

VisaChannelData* createVisaChannel(Tcl_Interp* const interp, ViSession session) {
	Tcl_Channel channel;
	char channelName[16 + TCL_INTEGER_SPACE];
	VisaChannelData* data = (VisaChannelData*) malloc(sizeof(VisaChannelData));

	/* Create and fill internal channel data */
	data->session = session;
	data->blocking = VISA_BLOCKING;
	data->isRMSession = 0;
	data->eof = 0;

	/* Attempt to create Tcl channel */
	sprintf(channelName, "%s%u", TCLVISA_NAME_PREFIX, session);
	channel = Tcl_CreateChannel(&visaChannelType, channelName, (ClientData) data, TCL_READABLE | TCL_WRITABLE);
	if (NULL != channel) {
		/* Channel created successgully, register it for later use in Tcl IO procedures */
		Tcl_RegisterChannel(interp, channel);
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
		viFlush(data->session, VI_WRITE_BUF | VI_IO_OUT_BUF | VI_READ_BUF_DISCARD | VI_IO_IN_BUF_DISCARD);
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

printf("blockModeProc enter session=%u blocking=%i\n", data->session, (mode == TCL_MODE_BLOCKING));
	switch (mode) {
	case TCL_MODE_BLOCKING:
		data->blocking = VISA_BLOCKING;
		break;

	case TCL_MODE_NONBLOCKING:
		data->blocking = VISA_NON_BLOCKING;
		break;

	default:
printf("blockModeProc leave error\n");
		return -1;
	}

printf("blockModeProc leave\n");
	return TCL_OK;
}

static int inputProc(ClientData instanceData, char *buf, int bufSize, int *errorCodePtr) {
	ViStatus status;
	int result;
	VisaChannelData* data = validateData(instanceData, NULL);

	if (!data || data->isRMSession) {
		return -1;
	}

	if (data->eof) {
		data->eof = 0;
		return 0;
	}

	if ((ViInt64) bufSize > (ViInt64) VISA_MAX_BUF_SIZE) {
		/* restrict buffer size */
		bufSize = VISA_MAX_BUF_SIZE;
	}

/* !!!	if (0 && VISA_NON_BLOCKING == data->blocking) {
		status = viReadAsync(data->session, (ViPBuf) buf, (ViUInt32) bufSize, NULL);
		result = EAGAIN;
	} else */ {
		ViUInt32 retCount;
		status = viRead(data->session, (ViPBuf) buf, (ViUInt32) bufSize, &retCount);
		result = (int) retCount;
	}

	if (status < 0) {
		if (errorCodePtr) {
			*errorCodePtr = (int) status;
		}
		result = -1;
	} else {
		if (VI_SUCCESS_MAX_CNT != status) {
			data->eof = 1;
		}
	}

	return result;
}

static int outputProc(ClientData instanceData, const char *buf, int toWrite, int *errorCodePtr) {
	ViStatus status;
	int result;
	VisaChannelData* data = validateData(instanceData, NULL);

	if (!data || data->isRMSession) {
		return -1;
	}

	if ((ViInt64) toWrite > (ViInt64) VISA_MAX_BUF_SIZE) {
		/* restrict buffer size */
		toWrite = VISA_MAX_BUF_SIZE;
	}

	/* !!! if (0 && VISA_NON_BLOCKING == data->blocking) {
		status = viWriteAsync(data->session, (ViBuf) buf, (ViUInt32) toWrite, NULL);
		result = EAGAIN;
	} else */ {
		ViUInt32 retCount;
		status = viWrite(data->session, (ViBuf) buf, (ViUInt32) toWrite, &retCount);
		result = (int) retCount;
	}

	if (status < 0) {
		if (errorCodePtr) {
			*errorCodePtr = (int) status;
		}
		result = -1;
	}

	return result;
}

static int setOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, const char *newValue) {
	VisaChannelData* data = (VisaChannelData*) instanceData;

printf("setOptionProc enter\n");
	if (0 == strcmp(optionName, TCLVISA_OPTION_MODE)) {
		int i = VI_ATTR_ASRL_BAUD;
		/* expected value format is <baud rate>,<parity>,<byte length>,<stop bits> */
		/* for example: 9600,n,8,1 */

		/*
		 * VI_ATTR_ASRL_BAUD
		 * VI_ATTR_ASRL_DATA_BITS
		 * VI_ATTR_ASRL_PARITY: VI_ASRL_PAR_NONE, VI_ASRL_PAR_ODD, VI_ASRL_PAR_EVEN, VI_ASRL_PAR_MARK, VI_ASRL_PAR_SPACE
		 * VI_ATTR_ASRL_STOP_BITS: VI_ASRL_STOP_ONE, VI_ASRL_STOP_TWO
		 */
printf("setOptionProc leave\n");
		return TCL_OK;
	} else {
		/* unknown option name */
printf("setOptionProc leave error\n");
		return Tcl_BadChannelOption(interp, optionName, tclVisaOptions);
	}
}

static int getOptionProc(ClientData instanceData, Tcl_Interp *interp, const char *optionName, Tcl_DString *dsPtr) {
	VisaChannelData* data = (VisaChannelData*) instanceData;

printf("getOptionProc enter\n");
printf("getOptionProc leave\n");
		return TCL_OK;
}

static void	watchProc(ClientData instanceData, int mask) {
	VisaChannelData* data = (VisaChannelData*) instanceData;

printf("watchProc enter\n");
printf("watchProc leave\n");
}

static int getHandleProc(ClientData instanceData, int direction, ClientData *handlePtr) {
	VisaChannelData* data = (VisaChannelData*) instanceData;

printf("getHandleProc enter\n");
printf("getHandleProc leave\n");
		return TCL_OK;
}
