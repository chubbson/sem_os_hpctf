/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-12
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#ifndef _LIB_HPWOTP_FIELDHELPER_
#define _LIB_HPWOTP_FIELDHELPER_

#include <kvmaphelper.h>
#include <field.h>

void * printplayer(zhash_t * kvmap, fldstruct * fs);
void * fld_dump(zhash_t * kvmap, fldstruct * fs);
//void printfield(fld_t *fs);
void sprintcolfield(int i, char *str);

#endif