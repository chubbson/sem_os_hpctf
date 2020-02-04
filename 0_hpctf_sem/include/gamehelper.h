/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-04-07
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_HPCTF_GAMEHELPER
#define _LIB_HPWOTP_HPCTF_GAMEHELPER

#include <game.h>
#include <czmq.h>

typedef struct {
  int fullslots; // side length
  int emptyslots; 
  int fieldsize;
  void * requester;
  char * endpoint;
  zhash_t * kvmap;
  int updms;
} game_settings; //playerslots;

void printgamestate(hpctf_game* hpctf);
//playerslots getplayercount(hpctf_game *hpctf);
game_settings getgamesettings(hpctf_game *hpctf);
game_settings gamesettings(int fieldsize);
void printgamesettings(game_settings *gs);
//void printplayer(hpctf_game *hpctf);

#endif