/*
 * visachannel.h
 *
 *  Created on: 15.11.2011
 *      Author: andrey
 */

#ifndef VISA_CHANNEL_H_23874237846253613
#define VISA_CHANNEL_H_23874237846253613

#include <tcl.h>
#include <visa.h>

typedef struct _VisaChannelData {
	ViSession session;
	short blocking;
	Tcl_Channel channel;
} VisaChannelData;

Tcl_Channel createVisaChannel(Tcl_Interp* const interp, ViSession session);
VisaChannelData* getVisaChannelFromObj(Tcl_Interp* const interp, Tcl_Obj* objPtr);

#endif /* VISA_CHANNEL_H_23874237846253613 */
