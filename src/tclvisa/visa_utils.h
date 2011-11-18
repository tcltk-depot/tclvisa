/*
 * visa.h
 *
 *  Created on: 15.11.2011
 *      Author: andrey
 */

#ifndef VISA_UTILS_H_34237856365464
#define VISA_UTILS_H_34237856365464

#include <visa.h>

#define TCLVISA_ERROR_NULL_DATA		1000
#define TCLVISA_ERROR_BAD_CHANNEL	1001

const char* visaErrorMessage(ViStatus status);
const char* tclvisaErrorMessage(int error);

#endif /* VISA_UTILS_H_34237856365464 */
