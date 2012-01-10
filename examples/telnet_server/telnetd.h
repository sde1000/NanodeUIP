/*
 * Copyright (c) 2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * $Id: telnetd.h,v 1.2 2006/06/07 09:43:54 adam Exp $
 *
 */
#ifndef __TELNETD_H__
#define __TELNETD_H__

#include "uipopt.h"
extern "C"
{
#include "memb.h"
}

void telnetd_appcall(void);
void telnetd_init(void);

// Be sure to adjust TCP_APP_STATE_SIZE if you change any of these! 
#ifndef TELNETD_CONF_LINELEN
#define TELNETD_CONF_LINELEN 30
#endif
#ifndef TELNETD_CONF_NUMLINES
// orig value #define TELNETD_CONF_NUMLINES 16
#define TELNETD_CONF_NUMLINES 5
#endif

struct telnetd_line {
  char line[TELNETD_CONF_LINELEN];
};

struct telnetd_state {
  char *lines[TELNETD_CONF_NUMLINES];
  char buf[TELNETD_CONF_LINELEN];
  u8_t bufptr;
  u8_t numsent;
  u8_t state;

  char linemem_memb_count[TELNETD_CONF_NUMLINES];
  telnetd_line linemem_memb_mem[TELNETD_CONF_NUMLINES];
  memb_blocks linemem;
};

#endif /* __TELNETD_H__ */
// vim:cin:ai:sts=2 sw=2 ft=cpp
