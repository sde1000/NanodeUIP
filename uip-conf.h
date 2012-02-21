/*
 * Copyright (c) 2007, Swedish Institute of Computer Science
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * $Id: uip-conf.h,v 1.6 2006/06/12 08:00:31 adam Exp $
 */


#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <inttypes.h>

/**
 * Statistics datatype
 *
 * This typedef defines the dataype used for keeping statistics in
 * uIP.
 *
 * \hideinitializer
 */
typedef unsigned short uip_stats_t;

/**
 * Maximum number of TCP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_CONNECTIONS 3

/**
 * Maximum number of listening TCP ports.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_LISTENPORTS 3

/**
 * uIP buffer size.
 *
 * \hideinitializer
 */
#define UIP_CONF_BUFFER_SIZE     420

/**
 * CPU byte order.
 *
 * \hideinitializer
 */
#define UIP_CONF_BYTE_ORDER      BIG_ENDIAN

/**
 * Logging on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_LOGGING         0

/**
 * UDP support on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP             1

#define UIP_CONF_UDP_CONNS       2

/**
 * UDP checksums on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CHECKSUMS   1

/**
 * uIP statistics on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_STATISTICS      0

#define UIP_CONF_RESOLV_ENTRIES  2

/* Here we include the header file for the application(s) we use in
   our project. */
/* Oh no we don't! We define the maximum size of application state instead. */
/*#include "smtp.h"*/
/*#include "hello-world.h"*/
/*#include "telnetd.h"*/
/*#include "webserver.h"*/
/*#include "dhcpc.h"*/
/*#include "resolv.h"*/
/*#include "webclient.h"*/
/* XXX must figure out a better way of doing this! */

#define TCP_APP_STATE_SIZE 150

typedef void tcp_appcall_fn(void);
typedef void udp_appcall_fn(void);

typedef struct tcp_appstate {
  uint8_t state[TCP_APP_STATE_SIZE];
} uip_tcp_appstate_t;

extern void nullproc(void);

#if 0
#define UIP_APPCALL() do { if (uip_conn && uip_conn->appcall) uip_conn->appcall(); } while (0)
#define UIP_UDP_APPCALL() do { uip_udp_conn->appcall(); } while (0)
#else
#define UIP_APPCALL() uip_conn->appcall()
#define UIP_UDP_APPCALL() uip_udp_conn->appcall()
#endif

#define UIPASSERT_LINE(predicate,line) \
  typedef char assertion_failed_line_##line [2*((predicate)!=0)-1];

/* Yes, this intermediate macro really is necessary! */
#define UIPASSERT_LINE_(predicate,line) UIPASSERT_LINE(predicate,line)

#define UIPASSERT(predicate) UIPASSERT_LINE_(predicate,__LINE__)

#endif /* __UIP_CONF_H__ */
