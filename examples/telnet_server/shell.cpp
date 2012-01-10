 /*
 * Copyright (c) 2003, Adam Dunkels.
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
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: shell.c,v 1.1 2006/06/07 09:43:54 adam Exp $
 *
 */
#include <string.h>
#include "Arduino.h"
#include "NanodeUIP.h"
#include "shell.h"

struct ptentry {
  PGM_P commandstr;
  void (* pfunc)(const char *str);
};

#ifndef SHELL_PROMPT
#define SHELL_PROMPT "NanodeUIP> "
#endif

/*---------------------------------------------------------------------------*/
void
parse(const register char *str, struct ptentry *t)
{
  // RAM space to copy an entry down from progmem
  struct ptentry entry;

  // Copy down the first entry
  memcpy_P(&entry,t,sizeof(entry));

  // Keep looking at entries until the commandstr is empty
  while ( entry.commandstr != NULL )
  {
    // If the command matches
    if(strncmp_P(str, entry.commandstr, strlen_P(entry.commandstr)) == 0) {
      // We've found our goal, stop!
      break;
    }
    // Copy down the next entry
    memcpy_P(&entry,++t,sizeof(entry));
  }

  entry.pfunc(str);
}

extern char* __brkval;
extern char __heap_start;
extern int errno;

/*---------------------------------------------------------------------------*/
static void
show_memory(const char *str)
{
  char buf[25];
  sprintf_P(buf,PSTR("SP = 0x%x"),SP);
  shell_output(buf,NULL);
  sprintf_P(buf,PSTR("__brkval = %p"),__brkval?__brkval:&__heap_start);
  shell_output(buf,NULL);
  sprintf_P(buf,PSTR("Free memory = %u bytes"),SP-(__brkval?(uint16_t)__brkval:(uint16_t)&__heap_start));
  shell_output(buf,NULL);
}

/*---------------------------------------------------------------------------*/
static void
help(const char *)
{
  // TEXT HERE CAN ONLY BE 30 chars / output! based on telnetd.h 
  shell_output_P(PSTR("Available commands:"), NULL);
  shell_output_P(PSTR("help, ? - show help"), NULL);
  shell_output_P(PSTR("exit    - exit shell"), NULL);
  shell_output_P(PSTR("mem    - show memory"), NULL);
}
/*---------------------------------------------------------------------------*/
static void
unknown_command(const char *str)
{
  if(strlen(str) > 0) {
    shell_output_P_1st(PSTR("Unknown command: "), str);
  }
}
/*---------------------------------------------------------------------------*/

const char stats_str_p[] PROGMEM = "stats";
const char conn_str_p[] PROGMEM = "conn";
const char help_str_p[] PROGMEM = "help";
const char exit_str_p[] PROGMEM = "exit";
const char q_str_p[] PROGMEM = "?";
const char mem_str_p[] PROGMEM = "mem";

static struct ptentry parsetab[] PROGMEM =
  {{stats_str_p, help},
   {conn_str_p, help},
   {help_str_p, help},
   {exit_str_p, shell_quit},
   {q_str_p, help},
   {mem_str_p, show_memory},
   {NULL, unknown_command}};
/*---------------------------------------------------------------------------*/
void
shell_init(void)
{
}
/*---------------------------------------------------------------------------*/
void
shell_start(void)
{
  shell_output_P(PSTR("uIP command shell"), NULL);
  shell_output_P(PSTR("Type '?' and return for help"), NULL);
  shell_prompt_P(PSTR(SHELL_PROMPT));
}
/*---------------------------------------------------------------------------*/
void
shell_input(char *cmd)
{
  parse(cmd, parsetab);
  shell_prompt_P(PSTR(SHELL_PROMPT));
}
/*---------------------------------------------------------------------------*/
// vim:cin:ai:sts=2 sw=2 ft=cpp
