/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-06
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <kvmaphelper.h>
#include <kvsimple.h>

#include <gamehelper.h>

// dublicates value, using malloc, dont forget to free
char * kvmap_dupKvValue(zhash_t * kvmap, char * key)
{
  char * res = NULL;
  kvmsg_t * kvmsg = (kvmsg_t *) zhash_lookup (kvmap, key);
  if(kvmsg != NULL)
  {
//    char * res = (char *)kvmsg_body (kvmsg);
    res = strndup((char *)kvmsg_body (kvmsg), kvmsg_size(kvmsg));
  }

  if (res == NULL)
    res  = strdup("");
  return res;
}

int kvmap_getIntKvValue(zhash_t * kvmap, char *key)
{
  char * sres = kvmap_dupKvValue(kvmap, key);
  int res = atoi(sres);
  free(sres);
  return res; 
}

int kvmap_getSize(zhash_t * kvmap)
{
  return kvmap_getIntKvValue(kvmap, "[fldlen]");
}

// uses malloc inside, dont forget to free value
char * kvmap_dupOwner(zhash_t * kvmap, int x, int y)
{
  char buf[256];
  sprintf(buf, "[%d][%d]", x, y);
  return kvmap_dupKvValue(kvmap, buf);
}

int kvmap_getPlayerId(zhash_t * kvmap, char * playername)
{
  char buf[256];
  sprintf(buf, "{%s}", playername);
  int ires = kvmap_getIntKvValue(kvmap, buf);
  return ires;
}

int kvmap_getState(zhash_t * kvmap)
{
  return kvmap_getIntKvValue(kvmap, "[state]");
}

int kvmap_getPlidxCnt(zhash_t * kvmap)
{
  return kvmap_getIntKvValue(kvmap, "[plidxcnt]");
}

// dont forget, free result 
char * kvmap_dupWinner(zhash_t * kvmap)
{
  return kvmap_dupKvValue(kvmap, "[winner]");
}

void kvmap_setWinner(zhash_t * kvmap, int seq, char * playername)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[winner]");
  kvmsg_fmt_body(kvmsg, playername);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void kvmap_setState(zhash_t * kvmap, int seq, void * socket, int gamestate)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[state]");
  kvmsg_fmt_body(kvmsg, "%d", gamestate);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void kvmap_setSize(zhash_t * kvmap, int seq, void * socket, int size)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[fldlen]");
  kvmsg_fmt_body(kvmsg, "%d", size);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void kvmap_setOwner(zhash_t * kvmap, int seq, void * socket, int x, int y, char * playername)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[%d][%d]", x, y);
  kvmsg_fmt_body(kvmsg, "%s", playername);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void kvmap_setPlayerId(zhash_t * kvmap, int seq, void * socket, char * playername, int plid)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "{%s}", playername);
  kvmsg_fmt_body(kvmsg, "%d", plid);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void kvmap_setPlidxCnt(zhash_t * kvmap, int seq, void * socket, int plidx)//, void * socket)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[plidxcnt]");
  kvmsg_fmt_body(kvmsg, "%d", plidx);
  // do not publish plidxcnt
  //kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

int kvmap_newPlidx(zhash_t * kvmap, int seq)
{
  int plidxcnt = kvmap_getPlidxCnt(kvmap);
  plidxcnt++; 
  kvmap_setPlidxCnt(kvmap, seq, NULL, plidxcnt);
  return plidxcnt;
}

void kvmap_printGameSettings(zhash_t * kvmap)
{
  int state = kvmap_getState(kvmap);
  int size = kvmap_getSize(kvmap);

  char * res = NULL;

  switch(state)
  {
    case RUNNING:
      res = "RUNNING";
      break;
    case WAITING4PLAYERS:
      res = "WAITING4PLAYERS";
      break;
    case FINISHED:
      res = "FINISHED";
      break;
    default: 
      res = ""; 
      break;
  }


  printf("Gamestate: [%d]%s - Size: %d\n", state, res, size);
}
