/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_HPCTF_GAME
#define _LIB_HPWOTP_HPCTF_GAME

#include <apue.h>
#include <itskylib.h>
#include <field.h>
#include <semaphore.h>
#include <czmq.h>
//#include <pthread.h>

#define MAXPLAYER 1878 //1878 // -> somecolor, added new fieldprint. 1878 different stante could be printed so that the new max player cnt


typedef struct {
	enum { WAITING4PLAYERS, RUNNING, FINISHED } gamestate;
	fldstruct * fs;
  sem_t freeplayerslots;
  int winner;
  char winnername[251];
  char * plidx[MAXPLAYER];

  zctx_t * ctx;               // context rapper
  void * responder;           // respond request
  void * fldpublisher;        // fld and state publisher
  zhash_t *kvmap;             //  Key-value store
  zloop_t *loop;              //  Reactor loop
  int seq;
//  int testvar;
} hpctf_game;

typedef struct {
  int idx;
  char * player; 
} player_idx;

hpctf_game * inithpctf(int mapsize);
void freehpctf(hpctf_game *hpctf); 
int logon(hpctf_game *hpctf);
int logoff(hpctf_game *hpctf);
int capturetheflag(hpctf_game *hpctf, int y, int x, char * playername);//int player);

#endif