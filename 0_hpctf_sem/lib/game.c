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

hpctf_game * inithpctf(int mapsize)
{
  hpctf_game * p_hpctf = malloc(sizeof(hpctf_game));
  sem_init(&p_hpctf->freeplayerslots, 0, MAXPLAYER); // threadshared, 6 player slots
  p_hpctf->fs = initfield(mapsize);
    p_hpctf->gamestate = WAITING4PLAYERS;
  p_hpctf->seq = 0;
  
  for (int i = 0; i < MAXPLAYER; ++i)
    p_hpctf->plidx[i] = NULL;

  p_hpctf->ctx = zctx_new ();
  p_hpctf->kvmap = zhash_new ();
  p_hpctf->workers = zlist_new ();

  p_hpctf->loop = zloop_new ();
  return p_hpctf;
}

void freehpctf(hpctf_game * p_hpctf)
{
  
  //int rc = zsocket_signal (p_hpctf->responder);
  //assert (rc == 0);
  int rc = zsocket_signal (p_hpctf->fldpublisher);
  zsocket_signal(p_hpctf->frontend);
  assert (rc == 0);
  /*
  rc = zsocket_wait (reader);
  assert (rc == 0);
  */
  //  When we're done, clean up properly
  //zloop_destroy (&p_hpctf->loop);
  while (zlist_size (p_hpctf->workers)) {
      zframe_t *frame = (zframe_t *) zlist_pop (p_hpctf->workers);
      zframe_destroy (&frame);
  }
  zlist_destroy (&p_hpctf->workers);
  zsocket_destroy (p_hpctf->ctx, p_hpctf->backend);
  zsocket_destroy (p_hpctf->ctx, p_hpctf->frontend);
  zsocket_destroy (p_hpctf->ctx, p_hpctf->fldpublisher);
  zhash_destroy (&p_hpctf->kvmap);
  zctx_destroy(&p_hpctf->ctx);

  for (int i = 0; i < MAXPLAYER; ++i)
    if (p_hpctf->plidx[i] != NULL)
      free(p_hpctf->plidx[i]);

  freefield(p_hpctf->fs);
  sem_destroy(&p_hpctf->freeplayerslots);
  free(p_hpctf);
}

int logon(hpctf_game *hpctf) 
{
  int retres = 0; 
  // decrease the number of free player slots
  // if no slots available, wait for free slots
  if(sem_trywait(&(hpctf->freeplayerslots)) == -1)
    // check errno
    return retres;

  int val;
  int res = sem_getvalue(&hpctf->freeplayerslots, &val);
  int plcnt = MAXPLAYER - val;
  printf("Players left: %d hello received %d\n", val, plcnt);


  retres += 0x01; // player logged in 

  if(hpctf->gamestate != RUNNING 
  && (MAXPLAYER - val) >= (hpctf->fs->n/2))
  {  
    hpctf->gamestate = RUNNING;
    retres += 0x02; // state swiched to running
  } 

  if ((plcnt > 1 && plcnt%10 == 0))
  {
    retres += 0x04; // start new worker thread
  }

  return retres;

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
  if(y < 0 || y >= hpctf->fs->n)
    return -1;
  if(x < 0 || x >= hpctf->fs->n)
    return -2;

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
