/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-06
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_HPCTF_KVMAPHELPER
#define _LIB_HPWOTP_HPCTF_KVMAPHELPER

#include <czmq.h>

/*
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
void printGameSettings(zhash_t * kvmap);
*/

// cuz of malloc, do not publish this mehtod. -> show in warning
// char * dupKvValue(zhash_t * kvmap, char * key);
int kvmap_getIntKvValue(zhash_t * kvmap, char *key);
int kvmap_getSize(zhash_t * kvmap);
int kvmap_getPlayerId(zhash_t * kvmap, char * playername);
int kvmap_getState(zhash_t * kvmap);
int kvmap_getPlidxCnt(zhash_t * kvmap);
// dont forget free uses strdup which using malloc
char * kvmap_dupWinner(zhash_t * kvmap);
// dont forget free uses strdup which using malloc
char * kvmap_dupOwner(zhash_t * kvmap, int x, int y);

void kvmap_setWinner(zhash_t * kvmap, int seq, char * playername);
void kvmap_setState(zhash_t * kvmap, int seq, void * socket, int gamestate);
void kvmap_setSize(zhash_t * kvmap, int seq, void * socket, int size);
void kvmap_setOwner(zhash_t * kvmap, int seq, void * socket, int x, int y, char * playername);
void kvmap_setPlayerId(zhash_t * kvmap, int seq, void * socket, char * playername, int plid);
void kvmap_setPlidxCnt(zhash_t * kvmap, int seq, void * socket, int plidx);
int  kvmap_newPlidx(zhash_t * kvmap, int seq);
void kvmap_printGameSettings(zhash_t * kvmap);

#endif