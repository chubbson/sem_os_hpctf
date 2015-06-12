/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-12
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_CLIENT
#define _LIB_HPWOTP_CLIENT

#include <gamehelper.h>
#include <command.h>

void usage(int argc, char const *argv[]);
cmd * sendCmd(game_settings * gs, char * scmd);
int sendHello(game_settings * gs);
int sendTake(game_settings * gs, int x, int y, int pid);

#endif