/*
 * Copyright (c) 2003, Adam Dunkelps->
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
 * $Id: telnetd.c,v 1.2 2006/06/07 09:43:54 adam Exp $
 *
 */

#include "NanodeUIP.h"
#include "Arduino.h"
#include "telnetd.h"
#include "shell.h"

#include <string.h>

#define ISO_nl       0x0a
#define ISO_cr       0x0d

#define STATE_NORMAL 0
#define STATE_IAC    1
#define STATE_WILL   2
#define STATE_WONT   3
#define STATE_DO     4
#define STATE_DONT   5
#define STATE_CLOSE  6

static struct telnetd_state* ps;

#define TELNET_IAC   255
#define TELNET_WILL  251
#define TELNET_WONT  252
#define TELNET_DO    253
#define TELNET_DONT  254
/*---------------------------------------------------------------------------*/
static char *
alloc_line(void)
{
  return reinterpret_cast<char*>(memb_alloc(&(ps->linemem)));
}
/*---------------------------------------------------------------------------*/
static void
dealloc_line(char *line)
{
  memb_free(&(ps->linemem), line);
}
/*---------------------------------------------------------------------------*/
void
shell_quit(const char *)
{
  ps->state = STATE_CLOSE;
}
/*---------------------------------------------------------------------------*/
static void
sendline(char *line)
{
  static unsigned int i;
  
  for(i = 0; i < TELNETD_CONF_NUMLINES; ++i) {
    if(ps->lines[i] == NULL) {
      ps->lines[i] = line;
      break;
    }
  }
  if(i == TELNETD_CONF_NUMLINES) {
    dealloc_line(line);
  }
}
/*---------------------------------------------------------------------------*/
void
shell_prompt(const char *str)
{
  char *line;
  line = alloc_line();
  if(line != NULL) {
    strncpy(line, str, TELNETD_CONF_LINELEN);
    /*    petsciiconv_toascii(line, TELNETD_CONF_LINELEN);*/
    sendline(line);
  }
}
/*---------------------------------------------------------------------------*/
void
shell_prompt_P(PGM_P str)
{
  char *line;
  line = alloc_line();
  if(line != NULL) {
    strncpy_P(line, str, TELNETD_CONF_LINELEN);
    /*    petsciiconv_toascii(line, TELNETD_CONF_LINELEN);*/
    sendline(line);
  }
}
/*---------------------------------------------------------------------------*/
void
shell_output(const char *str1, const char *str2)
{
  static unsigned len;
  char *line;

  line = alloc_line();
  if(line != NULL) {
    len = strlen(str1);
    strncpy(line, str1, TELNETD_CONF_LINELEN);
    if(str2 && len < TELNETD_CONF_LINELEN) {
      strncpy(line + len, str2, TELNETD_CONF_LINELEN - len);
    }
    len = strlen(line);
    if(len < TELNETD_CONF_LINELEN - 2) {
      line[len] = ISO_cr;
      line[len+1] = ISO_nl;
      line[len+2] = 0;
    }
    /*    petsciiconv_toascii(line, TELNETD_CONF_LINELEN);*/
    sendline(line);
  }
}
/*---------------------------------------------------------------------------*/
void
shell_output_P(PGM_P str1, PGM_P str2)
{
  static unsigned len;
  char *line;

  line = alloc_line();
  if(line != NULL) {
    len = strlen_P(str1);
    strncpy_P(line, str1, TELNETD_CONF_LINELEN);
    if(str2 && len < TELNETD_CONF_LINELEN) {
      strncpy_P(line + len, str2, TELNETD_CONF_LINELEN - len);
    }
    len = strlen(line);
    if(len < TELNETD_CONF_LINELEN - 2) {
      line[len] = ISO_cr;
      line[len+1] = ISO_nl;
      line[len+2] = 0;
    }
    /*    petsciiconv_toascii(line, TELNETD_CONF_LINELEN);*/
    sendline(line);
  }
}
/*---------------------------------------------------------------------------*/
void
shell_output_P_1st(PGM_P str1, const char* str2)
{
  static unsigned len;
  char *line;

  line = alloc_line();
  if(line != NULL) {
    len = strlen_P(str1);
    strncpy_P(line, str1, TELNETD_CONF_LINELEN);
    if(str2 && len < TELNETD_CONF_LINELEN) {
      strncpy(line + len, str2, TELNETD_CONF_LINELEN - len);
    }
    len = strlen(line);
    if(len < TELNETD_CONF_LINELEN - 2) {
      line[len] = ISO_cr;
      line[len+1] = ISO_nl;
      line[len+2] = 0;
    }
    /*    petsciiconv_toascii(line, TELNETD_CONF_LINELEN);*/
    sendline(line);
  }
}
/*---------------------------------------------------------------------------*/
void
telnetd_init(void)
{
  uip_listen(HTONS(23),telnetd_appcall);
  shell_init();
}
/*---------------------------------------------------------------------------*/
static void
acked(void)
{
  static unsigned int i;
  
  while(ps->numsent > 0) {
    dealloc_line(ps->lines[0]);
    for(i = 1; i < TELNETD_CONF_NUMLINES; ++i) {
      ps->lines[i - 1] = ps->lines[i];
    }
    ps->lines[TELNETD_CONF_NUMLINES - 1] = NULL;
    --ps->numsent;
  }
}
/*---------------------------------------------------------------------------*/
static void
senddata(void)
{
  static char *bufptr, *lineptr;
  static int buflen, linelen;
  
  bufptr = reinterpret_cast<char*>(uip_appdata);
  buflen = 0;
  for(ps->numsent = 0; ps->numsent < TELNETD_CONF_NUMLINES &&
	ps->lines[ps->numsent] != NULL ; ++ps->numsent) {
    lineptr = ps->lines[ps->numsent];
    linelen = strlen(lineptr);
    if(linelen > TELNETD_CONF_LINELEN) {
      linelen = TELNETD_CONF_LINELEN;
    }
    if(buflen + linelen < uip_mss()) {
      memcpy(bufptr, lineptr, linelen);
      bufptr += linelen;
      buflen += linelen;
    } else {
      break;
    }
  }
  uip_send(uip_appdata, buflen);
}
/*---------------------------------------------------------------------------*/
static void
closed(void)
{
  static unsigned int i;
  
  for(i = 0; i < TELNETD_CONF_NUMLINES; ++i) {
    if(ps->lines[i] != NULL) {
      dealloc_line(ps->lines[i]);
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
get_char(u8_t c)
{
  if(c == ISO_cr) {
    return;
  }
  
  ps->buf[(int)ps->bufptr] = c;
  if(ps->buf[(int)ps->bufptr] == ISO_nl ||
     ps->bufptr == sizeof(ps->buf) - 1) {
    if(ps->bufptr > 0) {
      ps->buf[(int)ps->bufptr] = 0;
      /*      petsciiconv_topetscii(ps->buf, TELNETD_CONF_LINELEN);*/
    }
	if((ps->bufptr + 1) < sizeof(ps->buf)) {
		ps->buf[(int)ps->bufptr +1] = 0;
	}
    shell_input(ps->buf);
    ps->bufptr = 0;
  } else {
    ++ps->bufptr;
  }
}
/*---------------------------------------------------------------------------*/
static void
sendopt(u8_t option, u8_t value)
{
  char *line;
  line = alloc_line();
  if(line != NULL) {
    line[0] = TELNET_IAC;
    line[1] = option;
    line[2] = value;
    line[3] = 0;
    sendline(line);
  }
}
/*---------------------------------------------------------------------------*/
static void
newdata(void)
{
  u16_t len;
  u8_t c;
  char *dataptr;
    
  
  len = uip_datalen();
  dataptr = (char *)uip_appdata;
  
  while(len > 0 && ps->bufptr < sizeof(ps->buf)) {
    c = *dataptr;
    ++dataptr;
    --len;
    switch(ps->state) {
    case STATE_IAC:
      if(c == TELNET_IAC) {
	get_char(c);
	ps->state = STATE_NORMAL;
      } else {
	switch(c) {
	case TELNET_WILL:
	  ps->state = STATE_WILL;
	  break;
	case TELNET_WONT:
	  ps->state = STATE_WONT;
	  break;
	case TELNET_DO:
	  ps->state = STATE_DO;
	  break;
	case TELNET_DONT:
	  ps->state = STATE_DONT;
	  break;
	default:
	  ps->state = STATE_NORMAL;
	  break;
	}
      }
      break;
    case STATE_WILL:
      /* Reply with a DONT */
      sendopt(TELNET_DONT, c);
      ps->state = STATE_NORMAL;
      break;
      
    case STATE_WONT:
      /* Reply with a DONT */
      sendopt(TELNET_DONT, c);
      ps->state = STATE_NORMAL;
      break;
    case STATE_DO:
      /* Reply with a WONT */
      sendopt(TELNET_WONT, c);
      ps->state = STATE_NORMAL;
      break;
    case STATE_DONT:
      /* Reply with a WONT */
      sendopt(TELNET_WONT, c);
      ps->state = STATE_NORMAL;
      break;
    case STATE_NORMAL:
      if(c == TELNET_IAC) {
	ps->state = STATE_IAC;
      } else {
	get_char(c);
      }
      break;
    }

    
  }
  
}
/*---------------------------------------------------------------------------*/
void
telnetd_appcall(void)
{
  ps = reinterpret_cast<telnetd_state *>(&(uip_conn->appstate));
  static unsigned int i;
  if(uip_connected()) {
    /*    tcp_markconn(uip_conn, &s);*/
    for(i = 0; i < TELNETD_CONF_NUMLINES; ++i) {
      ps->lines[i] = NULL;
    }
    ps->bufptr = 0;
    ps->state = STATE_NORMAL;

    ps->linemem.size = sizeof(telnetd_line);
    ps->linemem.num = TELNETD_CONF_NUMLINES;
    ps->linemem.count = ps->linemem_memb_count;
    ps->linemem.mem = ps->linemem_memb_mem;
    memb_init(&(ps->linemem));

    shell_start();
  }

  if(ps->state == STATE_CLOSE) {
    ps->state = STATE_NORMAL;
    uip_close();
    return;
  }
  
  if(uip_closed() ||
     uip_aborted() ||
     uip_timedout()) {
    closed();
  }
  
  if(uip_acked()) {
    acked();
  }
  
  if(uip_newdata()) {
    newdata();
  }
  
  if(uip_rexmit() ||
     uip_newdata() ||
     uip_acked() ||
     uip_connected() ||
     uip_poll()) {
    senddata();
  }
  ps = NULL;
}
/*---------------------------------------------------------------------------*/
// vim:cin:ai:sts=2 sw=2 ft=cpp
