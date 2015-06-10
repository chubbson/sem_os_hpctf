/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-06
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_HPCTF_KVMAPHELPER
#define _LIB_HPWOTP_HPCTF_KVMAPHELPER

#include <czmq.h>


char * getKvValue(zhash_t * kvmap, char * key);
int getSize(zhash_t * kvmap);
char * getOwner(zhash_t * kvmap, int x, int y);
int getPlayerId(zhash_t * kvmap, char * playername);
int getState(zhash_t * kvmap);

void setState(zhash_t * kvmap, int seq, void * socket, int gamestate);
void setSize(zhash_t * kvmap, int seq, void * socket, int size);
void setOwner(zhash_t * kvmap, int seq, void * socket, int x, int y, char * playername);
void setPlayerid(zhash_t * kvmap, int seq, void * socket, char * playername, int plid);
int newPlidx(zhash_t * kvmap, int seq);

#endif