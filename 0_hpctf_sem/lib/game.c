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
#include <gamehelper.h>

hpctf_game * inithpctf(int mapsize)
{
  hpctf_game * p_hpctf = malloc(sizeof(hpctf_game));
  p_hpctf->fs = initfield(mapsize);
  sem_init(&p_hpctf->freeplayerslots, 0, MAXPLAYER); // threadshared, 6 player slots
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

  int val;
  int res = sem_getvalue(&hpctf->freeplayerslots, &val);

  if(MAXPLAYER - val >= 2)
    hpctf->gamestate = RUNNING;

  printf("sem val: %d | %d\n", val, res);
}

void logoff(hpctf_game *hpctf)
{
  // increase the number of free player slots
  sem_post(&(hpctf->freeplayerslots));

  int val;
  int res = sem_getvalue(&hpctf->freeplayerslots, &val);

  if(MAXPLAYER - val < 2)
    hpctf->gamestate = WAITING4PLAYERS;

  printf("sem val: %d | %d\n", val, res);
}

int capturetheflag(hpctf_game *hpctf, int y, int x, int player)
{
  if(y < 0 || y >= hpctf->fs->n)
    return -1;
  if(x < 0 || x >= hpctf->fs->n)
    return -2;
  if(player < 0 || player >= hpctf->fs->n)
    return -3;

  printgamestate(hpctf);
  if(hpctf->gamestate != RUNNING)
    return -4; 
 
  // todo: mod to phread_mutex_trylock
  // INUSE\n

  // lock field at x y
  pthread_mutex_lock(&hpctf->fs->field[y][x].mutex);
  hpctf->fs->field[y][x].flag = player;
  // send taken to player
  // unlock 
  pthread_mutex_unlock(&hpctf->fs->field[y][x].mutex);

  int res = isfinished(hpctf->fs); 
  if(res > 0)
  {
    hpctf->gamestate = FINISHED;
    printf("END %d \n", res);
    return 0;
  } 

  return 1;
}

// some kick but has no player array ...