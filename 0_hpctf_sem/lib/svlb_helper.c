/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-06-18
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/


#include <svlb_helper.h>
#include <game.h>

//  Handle input from worker, on backend
int s_handle_backend (zloop_t * loop, zmq_pollitem_t * poller, void *arg)
{
  //  Use worker identity for load-balancing
  hpctf_game * hpctf = (hpctf_game *) arg;
  zmsg_t * msg = zmsg_recv (hpctf->backend);
  if (msg) {
    zframe_t *identity = zmsg_unwrap (msg);
    zlist_append (hpctf->workers, identity);

    //  Enable reader on frontend if we went from 0 to 1 workers
    if (zlist_size (hpctf->workers) == 1) {
      zmq_pollitem_t poller = { hpctf->frontend, 0, ZMQ_POLLIN };
//      zloop_poller (loop, &poller, s_handle_frontend, hpctf);
    }
    //  Forward message to client if it's not a READY
    zframe_t *frame = zmsg_first (msg);
    if (memcmp (zframe_data (frame), WORKER_READY, 1) == 0)
    {  zmsg_destroy (&msg);  }
    else
    {  zmsg_send (&msg, hpctf->frontend);  }
  }
  return 0;
}