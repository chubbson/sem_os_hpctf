/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <apue.h>
#include <itskylib.h>
#include <field.h>
#include <game.h>
//#include <pthread.h>

hpctf_game * inithpctf(int mapsize)
{
  hpctf_game * p_hpctf = malloc(sizeof(hpctf_game));
  p_hpctf->fs = initfield(mapsize);
  sem_init(&p_hpctf->freeplayerslots, 0, 6); // threadshared, 6 player slots
  p_hpctf->gamestate = WAITING4PLAYERS;

  return p_hpctf;
} 

void freehpctf(hpctf_game * p_hpctf)
{
  freefield(p_hpctf->fs);
  sem_destroy(&p_hpctf->freeplayerslots);

  free(p_hpctf);
}

void logon(hpctf_game *hpctf)
{
	// decrease the number of free player slots
	// if no slots available, wait for free slots
	sem_wait(&(hpctf->freeplayerslots));
}

void logoff(hpctf_game *hpctf)
{
  // increase the number of free player slots
	sem_post(&(hpctf->freeplayerslots));
}

// some kick but has no player array ...