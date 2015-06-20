/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-18
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/


#ifndef _LIB_HPWOTP_HPCTF_SVLBHELPER_
#define _LIB_HPWOTP_HPCTF_SVLBHELPER_

#include <czmq.h>


#define WORKER_READY   "\001"      //  Signals worker is ready

int s_handle_backend (zloop_t * loop, zmq_pollitem_t * poller, void *arg);


#endif