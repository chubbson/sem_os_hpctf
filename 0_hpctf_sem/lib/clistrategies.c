/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-12
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/


#include <unistd.h>
#include <clistrategies.h> 
#include <client.h> 


void strategie2(int pid, game_settings * gs)
{
  printf("strategie2\n");
  int res = TRUE;
  while(res)
  {
    res = sendTake(gs, 2, 2, pid);
    usleep(gs->updms*1000);
  }
}

void strategie3(int pid, game_settings * gs)
{
  printf("strategie3\n");
  int res = TRUE;
  int basex = randof(gs->fieldsize);
  int basey = randof(gs->fieldsize);
  while(res)
    for (int x = 0; x < gs->fieldsize && res; x++)
      for (int y = 0; y < gs->fieldsize && res; y++)
      {
        res = sendTake(gs, (basex+x)%gs->fieldsize, (basey+y)%gs->fieldsize, pid);
        usleep(gs->updms*1000);
      }
}

void strategie4(int pid, game_settings * gs)
{
  printf("strategie4\n");
  int res = TRUE;
  int basex = randof(gs->fieldsize);
  int basey = randof(gs->fieldsize);
  while(res)
    for (int y = gs->fieldsize-1; y >= 0 && res; y--)
      for (int x = gs->fieldsize-1; x >= 0 && res; x--)
      {
        res = sendTake(gs, (basex+x)%gs->fieldsize, (basey+y)%gs->fieldsize, pid);
        usleep(gs->updms*1000);
      }
}

void strategie5(int pid, game_settings * gs)
{
  printf("strategie5\n");
  int res = TRUE;
  int basex = randof(gs->fieldsize);
  int basey = randof(gs->fieldsize);
  while(res)
    for (int x = gs->fieldsize-1; x >= 0 && res; x--)
      for (int y = gs->fieldsize-1; y >= 0 && res; y--)
      {
        res = sendTake(gs, (basex+x)%gs->fieldsize, (basey+y)%gs->fieldsize, pid);
        usleep(gs->updms*1000);
      }
}

void strategie6(int pid, game_settings * gs)
{
  printf("strategie6\n");
  int res = TRUE;
  while(res)
  {
    res = sendTake(gs, randof (gs->fieldsize), randof (gs->fieldsize), pid);
    usleep(gs->updms*1000);
  }
}

void strategie1(int pid, game_settings * gs)
{
  printf("strategie1\n");
  int res = TRUE;
  int basex = randof(gs->fieldsize);
  int basey = randof(gs->fieldsize);

  while(res)
    for (int y = 0; y < gs->fieldsize && res; y++)
      for (int x = 0; x < gs->fieldsize && res; x++)
      {
        res = sendTake(gs, (basex+x)%gs->fieldsize, (basey+y)%gs->fieldsize, pid);
        usleep(gs->updms*1000);
      }
}