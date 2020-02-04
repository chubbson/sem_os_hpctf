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
#include <gameconst.h>


typedef struct {
	enum { WAITING4PLAYERS = 0, RUNNING = 1, FINISHED =2 } gamestate;
	fldstruct * fs;
  sem_t freeplayerslots;
  int winner;
  char winnername[254];
  char * plidx[MAXPLAYER];

  zsock_t * frontend;           // respond request
  zsock_t * backend;            // load balancer
  zsock_t * fldpublisher;        // fld and state publisher
  zhash_t *kvmap;             //  Key-value store
  zloop_t *loop;              //  Reactor loop
  int64_t seq;
//  int testvar;
  zlist_t * workers;
  int verbose;

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