/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-04-12
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_COMMAND
#define _LIB_HPWOTP_COMMAND

#include <apue.h>
#include <itskylib.h>
#include <gamehelper.h>

typedef struct {
  enum { UNKNOWN, HELLO, SIZE, NACK, START, TAKE, TAKEN, INUSE, STATUS, PLAYER, END } command;
  int n, x, y;
  //char * player;
  //int player;
  char * playername;
} cmd;

cmd * parseandinitcommand(char * cmdstr);
int verifycommand(cmd * cmdptr, game_settings * settings);

#endif