/*
* NanodeMAC.h
* Nanode MAC chip reading library header
*/

#ifndef _NANODEMAC_LIB_H
#define _NANODEMAC_LIB_H

#include <inttypes.h>
#include <WProgram.h>

extern "C" {
#include "uip.h"
}

void NanodeMAC(struct uip_eth_addr *mac);

#endif //_NANODEMAC_LIB_H

