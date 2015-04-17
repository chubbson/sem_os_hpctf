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

  for (int i = 0; i < MAXPLAYER; ++i)
    p_hpctf->plidx[i] = NULL;

  return p_hpctf;
} 

void freehpctf(hpctf_game * p_hpctf)
{
  for (int i = 0; i < MAXPLAYER; ++i)
    if (p_hpctf->plidx[i] != NULL)
      free(p_hpctf->plidx[i]);

  freefield(p_hpctf->fs);
  sem_destroy(&p_hpctf->freeplayerslots);

  free(p_hpctf);
}

int logon(hpctf_game *hpctf) 
{
	// decrease the number of free player slots
	// if no slots available, wait for free slots
	if(sem_trywait(&(hpctf->freeplayerslots)) == -1)
    // check errno
    return -1;

  int val;
  int res = sem_getvalue(&hpctf->freeplayerslots, &val);

  printf("sem val: %d | %d\n", val, res);
  if(hpctf->gamestate != RUNNING && MAXPLAYER - val >= 2)
  {  
    hpctf->gamestate = RUNNING;
    return TRUE;
  }
  return FALSE;
}

int logoff(hpctf_game *hpctf)
{
  // increase the number of free player slots
  sem_post(&(hpctf->freeplayerslots));

  int val;
  int res = sem_getvalue(&hpctf->freeplayerslots, &val);

  printf("sem val: %d | %d\n", val, res);
  if(hpctf->gamestate != WAITING4PLAYERS && MAXPLAYER - val < 2)
  {
    hpctf->gamestate = WAITING4PLAYERS;
    return TRUE;
  }
  return FALSE;
}

void initplidx(hpctf_game *hpctf)
{
  for (int i = 0; i < MAXPLAYER; ++i)
  {
//    if(hpctf->plidx[i] != NULL)
//      free(hpctf->plidx[i]);
    hpctf->plidx[i] = NULL;
  }
}

int playerid(hpctf_game *hpctf, char * player)
{
  for (int i = 0; i < MAXPLAYER; ++i)
  {
    if(hpctf->plidx[i] == NULL)
    {
      hpctf->plidx[i] = strdup(player);
      return (i+1);
    }

    if(strcmp(player, hpctf->plidx[i]) == 0)
      return (i+1);
  }

  return 0;
}
 

int capturetheflag(hpctf_game *hpctf, int y, int x, char * playername)// int player/*, char * playername*/)
{
  if(y < 0 || y >= hpctf->fs->n)
    return -1;
  if(x < 0 || x >= hpctf->fs->n)
    return -2;
//  if(player < 0 || player >= hpctf->fs->n)
//    return -3;

  printgamestate(hpctf);
  if(hpctf->gamestate != RUNNING)
    return -4; 

/*  char buf[245];
  int n = sprintf(buf, "Player %d", player);
  if (n <= 0)
    return -5; 
  buf[n] = '\0';
*/
  int plid = playerid(hpctf, playername); //########
  printf("plid: %d - %s\n", plid, playername);

  if (plid == 0)
    return -1;

  int retres = TRUE;
 
  // lock field at x y
  if(pthread_mutex_trylock(&hpctf->fs->field[y][x].mutex) == 0)
  {
    hpctf->fs->field[y][x].flag = plid; // player

    // send taken to player
    // unlock 
    pthread_mutex_unlock(&hpctf->fs->field[y][x].mutex);

    int res = isfinished(hpctf->fs); 
    if(res > 0)
    {
      hpctf->gamestate = FINISHED;
      hpctf->winner = res;

      char fld[252]; 
      fld[0] = '\0';
      sprintcolfield(res, fld);
      sprintf(hpctf->winnername, "%s=%s\n", fld, hpctf->plidx[res-1]);

    } 
  }
  else
  {
    retres = FALSE; 
  }

  return retres;
}

// some kick but has no player array ...