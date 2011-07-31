/*
* NanodeMAC.h
* Nanode MAC chip reading library header
*/

#ifndef _NANODEMAC_LIB_H
#define _NANODEMAC_LIB_H

#include <inttypes.h>
#include <WProgram.h>

class NanodeMAC {
  private:
	void unio_standby();
	void unio_start_header();
	void unio_sendByte(byte data);
	byte unio_readBytes(byte *addr, unsigned int length);
	inline bool unio_readBit();
  public:
	NanodeMAC( byte *macaddr );
};

#endif //_NANODEMAC_LIB_H

