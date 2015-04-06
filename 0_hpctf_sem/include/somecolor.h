/* (C) Hauronen Patronens waste of time projects!
* https://github.com/chubbson/zhaw_os_linux
* Author: David Hauri
* Date: 2014-03-15
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
*/

#ifndef _LIB_HPWOTP_COLOR
#define _LIB_HPWOTP_COLOR

// \x1B == \033 == \e
#define RESET  "\033[0m"
#define RESETX "\x1B[0m"
#define RESETE "\e[0m"
#define KBRED  "\x1B[1;31m"
#define KBGRN  "\x1B[1;32m"
#define KBYEL  "\x1B[1;33m"
#define KBBLU  "\x1B[1;34m"
#define KBMAG  "\x1B[1;35m"
#define KBCYN  "\x1B[1;36m"
#define KBWHT  "\x1B[1;37m"
#define KRED   "\x1B[0;31m"
#define KGRN   "\x1B[0;32m"
#define KYEL   "\x1B[0;33m"
#define KBLU   "\x1B[0;34m"
#define KMAG   "\x1B[0;35m"
#define KCYN   "\x1B[0;36m"
#define KWHT   "\x1B[0;37m"
#define KFRED   "\x1B[0;31;41m"
#define KFGRN   "\x1B[0;32;42m"
#define KFYEL   "\x1B[0;33;43m"
#define KFBLU   "\x1B[0;34;44m"
#define KFMAG   "\x1B[0;35;45m"
#define KFCYN   "\x1B[0;36;46m"
#define KFWHT   "\x1B[0;37;47m"

#define BLKBLDBLURED "\e[1;5;34;41m"

 /*
const char pink[] = "\e[1;35m";   
const char purple[] = "\e[0;35m"; 
*/
#endif
