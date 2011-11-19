/*
 * visachannel.h --
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

#ifndef VISA_CHANNEL_H_23874237846253613
#define VISA_CHANNEL_H_23874237846253613

#include <tcl.h>
#include <visa.h>

typedef struct _VisaChannelData {
	ViSession session;
	short blocking, isRMSession, eof;
	Tcl_Channel channel;
	ViUInt32 timeout;
} VisaChannelData;

VisaChannelData* createVisaChannel(Tcl_Interp* const interp, ViSession session);
VisaChannelData* getVisaChannelFromObj(Tcl_Interp* const interp, Tcl_Obj* objPtr);
int getVisaTimeout(Tcl_Interp *interp, VisaChannelData* data, ViUInt32* timeout);
int setVisaTimeout(Tcl_Interp *interp, VisaChannelData* data, ViUInt32 timeout);

#endif /* VISA_CHANNEL_H_23874237846253613 */
