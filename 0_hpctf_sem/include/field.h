/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_FIELD
#define _LIB_HPWOTP_FIELD

#include <apue.h>
#include <itskylib.h>
#include <pthread.h>
#include <gameconst.h>

typedef struct {
	pthread_mutex_t mutex; 
	int flag;
} square;

typedef struct {
	int n; // side length
  square** field; 
} fldstruct;

fldstruct * initfield(int n);
void freefield(fldstruct *fs);
int isfinished(fldstruct *fs);
void printcolfield(int i);
void printfield(fldstruct *fs);
void sprintcolfield(int i, char *str);

#endif