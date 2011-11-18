/*
 * visa_utils.h --
 *
 * This file is part of tclvisa library.
 *
 * Copyright (c) 2011 Andrey V. Nakin <andrey.nakin@gmail.com>
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef VISA_UTILS_H_34237856365464
#define VISA_UTILS_H_34237856365464

#include <visa.h>

#define TCLVISA_ERROR_NULL_DATA		1000
#define TCLVISA_ERROR_BAD_CHANNEL	1001

const char* visaErrorMessage(ViStatus status);
const char* tclvisaErrorMessage(int error);

#endif /* VISA_UTILS_H_34237856365464 */
