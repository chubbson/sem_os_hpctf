/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-12
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <fieldhelper.h>
#include <somecolor.h>


void * printplayer(zhash_t * kvmap, fldstruct * fs)
{
  int size = fs->n;
  bool players[MAXPLAYER] = {FALSE};
  char * prntlpayer =  '\0';
  int res = (size*size*30 + size + 1) * sizeof(char);
  prntlpayer = malloc(res);
  prntlpayer[0] = '\0';
  char buf[15];
  buf[0] = '\0';

  for (int y = 0; y < size; ++y)
    for (int x = 0; x < size; ++x)
    {
      char * playername = kvmap_dupOwner(kvmap, x, y);
      int plid = kvmap_getPlayerId(kvmap, playername);
      fs->field[y][x].flag = plid;

      if(plid > 0 && players[plid] == FALSE)
      { 
        players[plid] = TRUE;
        sprintcolfield(plid, prntlpayer);
        int n = snprintf(buf, 15, ":%d=%s, ", plid, playername);
        buf[n] = '\0';
        strcat(prntlpayer, buf);
      }
      free(playername);
    }

  puts(prntlpayer);
  free(prntlpayer);

  return NULL;
}

void * fld_dump(zhash_t * kvmap, fldstruct * fs)
{
  int size = fs->n;
  char buf[256] = "\0"; 
  for (int y = 0; y < size; ++y)
    for (int x = 0; x < size; ++x)
    {
      char * playername = kvmap_dupOwner(kvmap, x, y);
      int plid = kvmap_getPlayerId(kvmap, playername);
      snprintf(buf, 256, "%d:[%d][%d]\t- %04d:\t%s", size, x, y, plid, playername);
      puts(buf);
      free(playername);
    }

  return NULL; 
}

void sprintcolfield(int i, char *str)/*, int n, char * str*/
{ 
  if(str == NULL)
    return;

  scatcolor(i, str);
} 


void printfield(fldstruct *fs)
{
  int n2 = fs->n + 2;
  char * prntfld;

  // alloc enough 
  int res = (n2*n2*15 + n2 + 1) * sizeof(char);
  if((prntfld = malloc(res)) != NULL)
  {
    prntfld[0] = '\0';
    // top border
    for (int i = 0; i < (fs->n+2); ++i)
      sprintcolfield(-1, prntfld);
    strcat(prntfld, "\n");

    // print field line 
    for (int y = 0; y < fs->n; ++y)
    {
      // left border
      sprintcolfield(-1, prntfld);

      for(int x = 0; x < fs->n; ++x)
      {
        sprintcolfield(fs->field[y][x].flag, prntfld);
      }

      // right border
      sprintcolfield(-1, prntfld);
      strcat(prntfld, "\n");
    }

    // bottom border
    for (int i = 0; i < (fs->n+2); ++i)
      sprintcolfield(-1, prntfld);
    strcat(prntfld, "\n");

    int res2 = strlen(prntfld);
    prntfld[res2] = '\0';

    printf("%s", prntfld);
    free(prntfld);
  }
  else
  {
    // malloc failed 
    // exit ? 
  }
  return; 
}
