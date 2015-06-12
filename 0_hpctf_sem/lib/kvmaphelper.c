/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-06
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <kvmaphelper.h>
#include <kvsimple.h>

#include <gamehelper.h>


char * getKvValue(zhash_t * kvmap, char * key)
{
  kvmsg_t * kvmsg = (kvmsg_t *) zhash_lookup (kvmap, key);
  if(kvmsg != NULL)
  {
    char * res = (char *)kvmsg_body (kvmsg);
    //kvmsg_dump(kvmsg);
    return res != NULL ? res : "";
  }

  return "";
}

int getSize(zhash_t * kvmap)
{
  return atoi(getKvValue(kvmap, "[fldlen]"));
}

char * getOwner(zhash_t * kvmap, int x, int y)
{
  char buf[256];
  sprintf(buf, "[%d][%d]", x, y);
  return getKvValue(kvmap, buf);
}

int getPlayerId(zhash_t * kvmap, char * playername)
{
  char buf[256];
  sprintf(buf, "{%s}", playername);
  return atoi(getKvValue(kvmap, buf));
}

int getState(zhash_t * kvmap)
{
  return atoi(getKvValue(kvmap, "[state]"));
}

int getPlidxCnt(zhash_t * kvmap)
{
  return atoi(getKvValue(kvmap, "[plidxcnt]"));
}

void setState(zhash_t * kvmap, int seq, void * socket, int gamestate)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[state]");
  kvmsg_fmt_body(kvmsg, "%d", gamestate);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void setSize(zhash_t * kvmap, int seq, void * socket, int size)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[fldlen]");
  kvmsg_fmt_body(kvmsg, "%d", size);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void setOwner(zhash_t * kvmap, int seq, void * socket, int x, int y, char * playername)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[%d][%d]", x, y);
  kvmsg_fmt_body(kvmsg, "%s", playername);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void setPlayerid(zhash_t * kvmap, int seq, void * socket, char * playername, int plid)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "{%s}", playername);
  kvmsg_fmt_body(kvmsg, "%d", plid);
  kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

void setPlidxCnt(zhash_t * kvmap, int seq, void * socket, int plidx)//, void * socket)
{
  kvmsg_t *kvmsg = kvmsg_new(seq);
  kvmsg_fmt_key(kvmsg, "[plidxcnt]");
  kvmsg_fmt_body(kvmsg, "%d", plidx);
  // do not publish plidxcnt
  //kvmsg_send(kvmsg, socket);
  kvmsg_store(&kvmsg, kvmap);
}

int newPlidx(zhash_t * kvmap, int seq)
{
  int plidxcnt = getPlidxCnt(kvmap);
  plidxcnt++; 
  setPlidxCnt(kvmap, seq, NULL, plidxcnt);
  return plidxcnt;
}

void printGameSettings(zhash_t * kvmap)
{
  int state = getState(kvmap);
  int size = getSize(kvmap);

  char * res = state == WAITING4PLAYERS
        ? "WAITING4PLAYERS"
        : state == RUNNING
        ? "RUNNING"
        : state == FINISHED
        ? "FINISHED"
        : ""; 

  printf("Gamestate: [%d]%s - Size: %d\n", state, res, size);
}
