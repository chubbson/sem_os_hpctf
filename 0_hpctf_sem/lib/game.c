/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <apue.h>
#include <itskylib.h> 
#include <fieldhelper.h>
#include <game.h>
//#include <pthread.h>
#include <gamehelper.h>
#include <signal.h>

#include <kvsimple.h>
#include <kvmaphelper.h>

//#include "lib/kvsimple.c"

/*
hpctf_game * inithpctf(int mapsize)
{
  hpctf_game * p_hpctf = malloc(sizeof(hpctf_game));
  p_hpctf->fs = initfield(mapsize);
  sem_init(&p_hpctf->freeplayerslots, 0, MAXPLAYER); // threadshared, 6 player slots
  p_hpctf->gamestate = WAITING4PLAYERS;
  p_hpctf->seq = 0;
  for (int i = 0; i < MAXPLAYER; ++i)
    p_hpctf->plidx[i] = NULL;

  p_hpctf->ctx = zctx_new ();
  assert(p_hpctf->ctx);
  //zctx_set_linger (p_hpctf->ctx, 5);       //  5 msecs
  p_hpctf->kvmap = zhash_new ();
  p_hpctf->responder = zsocket_new (p_hpctf->ctx, ZMQ_REP);
  p_hpctf->fldpublisher = zsocket_new (p_hpctf->ctx, ZMQ_PUB);
  p_hpctf->loop = zloop_new ();

  return p_hpctf;
} 
*/

hpctf_game * inithpctf(int mapsize)
{
  hpctf_game * p_hpctf = malloc(sizeof(hpctf_game));
  sem_init(&p_hpctf->freeplayerslots, 0, MAXPLAYER); // threadshared, 6 player slots
  p_hpctf->fs = initfield(mapsize);
    p_hpctf->gamestate = WAITING4PLAYERS;
  p_hpctf->seq = 0;
  
  for (int i = 0; i < MAXPLAYER; ++i)
    p_hpctf->plidx[i] = NULL;

/*
  p_hpctf->plidx[1] = strdup("asdcf");
  p_hpctf->plidx[2] = strdup("aswdf");
  p_hpctf->plidx[3] = strdup("assdf");
  p_hpctf->plidx[4] = strdup("asgdf");
  p_hpctf->plidx[5] = strdup("aascsdf");  
*/

  p_hpctf->ctx = zctx_new ();
  p_hpctf->kvmap = zhash_new ();
  p_hpctf->loop = zloop_new ();

  p_hpctf->frontend = zsocket_new (p_hpctf->ctx, ZMQ_REP);
  p_hpctf->fldpublisher = zsocket_new (p_hpctf->ctx, ZMQ_PUB);
//  zsocket_bind (p_hpctf->fldpublisher, "tcp://*:%d", 5556 + 1);
  //p_hpctf->responder = zsocket_new (p_hpctf->ctx, ZMQ_REP);

  return p_hpctf;
}

void freehpctf(hpctf_game * p_hpctf)
{
  
  //int rc = zsocket_signal (p_hpctf->responder);
  //assert (rc == 0);
  int rc = zsocket_signal (p_hpctf->fldpublisher);
  assert (rc == 0);
  /*
  rc = zsocket_wait (reader);
  assert (rc == 0);
  */
  zsocket_destroy (p_hpctf->ctx, p_hpctf->frontend);
  zsocket_destroy (p_hpctf->ctx, p_hpctf->fldpublisher);
  zloop_destroy (&p_hpctf->loop);
  zhash_destroy (&p_hpctf->kvmap);
  zctx_destroy(&p_hpctf->ctx);

  for (int i = 0; i < MAXPLAYER; ++i)
    if (p_hpctf->plidx[i] != NULL)
      free(p_hpctf->plidx[i]);

  freefield(p_hpctf->fs);
  sem_destroy(&p_hpctf->freeplayerslots);
  free(p_hpctf);
}
/*
void freehpctf(hpctf_game * p_hpctf)
{

  printf("free1\n");
  zloop_destroy (&p_hpctf->loop);

  zmq_close (p_hpctf->responder);
  zmq_close (p_hpctf->fldpublisher);

  printf("free2\n");
  zhash_destroy (&p_hpctf->kvmap);
  printf("free3\n");
  zctx_destroy (&p_hpctf->ctx);
  printf("free4\n");

  for (int i = 0; i < MAXPLAYER; ++i)
    if (p_hpctf->plidx[i] != NULL)
      free(p_hpctf->plidx[i]);

  freefield(p_hpctf->fs);
  sem_destroy(&p_hpctf->freeplayerslots);

  free(p_hpctf);
}
*/

int logon(hpctf_game *hpctf) 
{
	// decrease the number of free player slots
	// if no slots available, wait for free slots
	if(sem_trywait(&(hpctf->freeplayerslots)) == -1)
    // check errno
    return -1;

  int val;
  /*int res = */sem_getvalue(&hpctf->freeplayerslots, &val);

//debug  printf("sem val: %d | %d\n", val, res);
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
  /*int res = */sem_getvalue(&hpctf->freeplayerslots, &val);

//debug  printf("sem val: %d | %d\n", val, res);
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
  char * ptmp = strdup(player);
  int plid = kvmap_getPlayerId(hpctf->kvmap, ptmp); 

  if(plid == 0)
  {
    int tmppid = kvmap_newPlidx(hpctf->kvmap, hpctf->seq++);
    hpctf->plidx[tmppid] = strdup(ptmp);
    kvmap_setPlayerId(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->plidx[tmppid], tmppid);
    plid = tmppid;
  }
  free(ptmp);

  return plid;
}

int playerid_orig(hpctf_game *hpctf, char * player)
{
//  printf("player %s\n", player);
//§k char * ptmp = strdup(player);
  int plid = kvmap_getPlayerId(hpctf->kvmap, player);
  if (plid == 0)
  {
    for (int i = 0; i < MAXPLAYER; ++i)
    {
      plid = i+1;
      if(hpctf->plidx[i] == NULL)
      {
        hpctf->plidx[i] = strdup(player);
        if(!zctx_interrupted)
        {
          kvmap_setPlayerId(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, player, plid);
          printf("%s %d %s - plidx[%d]=%s\n", "playerIdStored", plid, player, i, hpctf->plidx[i]);
        }
      }

      if(strcmp(player, hpctf->plidx[i]) == 0)
        return plid;
    }
  }
  else 
    return plid;


  return 0;
}
 

int capturetheflag(hpctf_game *hpctf, int x, int y, char * playername)// int player/*, char * playername*/)
{
  printf("%s x:%d y:%d n:%d gs:%d \n", "enter ctf", x, y, hpctf->fs->n, hpctf->gamestate);
  if(y < 0 || y >= hpctf->fs->n)
    return -1;
  if(x < 0 || x >= hpctf->fs->n)
    return -2;
//  if(player < 0 || player >= hpctf->fs->n)
//    return -3;

//debug  printgamestate(hpctf);
  if(hpctf->gamestate != RUNNING)
    return -4; 

  int plid = playerid(hpctf, playername); 
  char * ptmp = hpctf->plidx[plid];
 
  if (plid == 0)
    return -1;

  int retres = TRUE;
 
  // lock field at x y
  if(pthread_mutex_trylock(&hpctf->fs->field[y][x].mutex) == 0)
  {
    hpctf->fs->field[y][x].flag = plid; // player

    if(!zctx_interrupted)
    {
      kvmap_setOwner(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, x, y, ptmp);
    }

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