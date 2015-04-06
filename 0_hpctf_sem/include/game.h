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
//#include <pthread.h>

typedef struct {
	enum { WAITING4PLAYERS, STARTED, FINISHED } gamestate;
	fldstruct * fs;
  sem_t freeplayerslots;
  int testvar;
} hpctf_game;

hpctf_game * inithpctf(int mapsize);
void freehpctf(hpctf_game *hpctf); 
void logon(hpctf_game*);
void logoff(hpctf_game*);

#endif