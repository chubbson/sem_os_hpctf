/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/
 
#include <unistd.h>
#include <apue.h>
#include <itskylib.h>
#include <field.h>
#include <game.h>
#include <command.h>
#include <czmq.h>
#include <assert.h>

#include <gamehelper.h>
#include <somecolor.h>

#include <kvsimple.h>
#include <kvmaphelper.h>
 
#include <svlb_helper.h>

static int s_interrupted = 0; 
 
// Signal handling 
// call s_catch_signals() in your application at startup, 
// and then exit your mainloop if s_interrupted is ever 1. 
static void s_signal_handler(int signal_value)
{
  s_interrupted = 1; 
}

static void s_catch_signals(void)
{
  struct sigaction action; 
  action.sa_handler = s_signal_handler;
  action.sa_flags = 0; 
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL); 
}

int verbose = FALSE;
int size = 0; 

int handlecommand(char * buf, hpctf_game * hpctfptr, cmd * cmdptr, int64_t * seq);
 

void usage(int argc, char const *argv[])
{
  printf("USAGE:\n\n%s\n", argv[0]);
  printf("\t4\tparsing int as size, must be greater or equal 4\n");
  printf("\t-v\tVerbose\n");
 
  for (int i = 1; i < argc; ++i)
  {
    if(atoi(argv[i]) >= 4)
    {
      size = atoi(argv[i]);
      printf("Size = %d\n", size);
    }
    else if(strcmp(argv[i], "-v") == 0)
    {  
      verbose = TRUE; 
    }
  }

  if(size < 4)
  {
    puts("start with field size of at least 4");
    exit(0);
  }

} 


void * msgframes_dump(zmsg_t * msg)
{
  zframe_t * frame = zmsg_first(msg);
  int i= 0; 
  while(frame)
  {
    char * frmctx = zframe_strdup(frame);

    printf("[%d] '%s'\n", i++, frmctx);
    free(frmctx);
    frame = zmsg_next(msg);
  }
  return NULL;

}

//  Wortker using REQ socket to do load-balancing
static void * worker_task(void *args)
{
  hpctf_game * hpctf = (hpctf_game *) args;

  zctx_t * ctx = zctx_new();
  void * worker = zsocket_new(ctx, ZMQ_REQ);
  zsocket_connect (worker, "ipc://backend.ipc");

  //  Tell broker we're ready for work
  zframe_t *frame = zframe_new (WORKER_READY, 1);
  puts("sending - WORKER_READY");
  zframe_send (&frame, worker, 0);

  int64_t sequence = 0; 

  //  Process messages as they arrive
  while (!zctx_interrupted) {
    printf("seq: %" PRId64 "\n", sequence++);
    zmsg_t *msg = zmsg_recv (worker);
    if (!msg) 
    {
      errno = zmq_errno(); 
      if (errno == EAGAIN) 
      {
        printf ("I: EAGAIN!\n"); 
        continue; 
      } 
      if (errno == ETERM) 
      { 
        printf ("I: Terminated!\n"); 
        break; 
      } 
      printf ("E: (%d) %s\n", errno, strerror(errno)); 
      break; 
    }

    zmsg_dump(msg);
    frame = zmsg_last (msg);
    if(frame)
    {
      char * sval = zframe_strdup(frame);
      if (sval)
      {
        cmd * cmdptr = parseandinitcommand(sval);
        char scmd256[256];
        int n = handlecommand(scmd256, hpctf, cmdptr, &sequence);
        zframe_reset(frame,  scmd256, n);
        zmsg_dump(msg);
        int rc = zmsg_send (&msg, worker);  
        if(rc != 0)
          break;    

        free(cmdptr);
        free(sval);
      }
    }
    zmsg_destroy(&msg);
  }

  puts("zctx_interrupted %d\n");
  zctx_destroy (&ctx);
  return NULL;
}


int handlecommand(char * buf, hpctf_game * hpctfptr, cmd * cmdptr, int64_t * seq)
{
  int n = 0;
  game_settings gs = getgamesettings(hpctfptr);
  if(hpctfptr->verbose)
    cmd_dump(cmdptr);

  int commandsucceed = verifycommand(cmdptr, &gs);
  if(commandsucceed == TRUE)
  {
    if(hpctfptr->gamestate == FINISHED 
    && cmdptr->command != HELLO)
    { 
      if((n = sprintf(buf, 
                      "END %s\n", 
                      hpctfptr->winnername)) > 0)
      {
        logoff(hpctfptr);
        return n; 
      }
    }



    int res; 
    switch(cmdptr->command)
    {
      case HELLO:
        res = logon(hpctfptr);
        if(res == 0)
        {
          n = sprintf(buf, "%s", "NACK\n");
        }
        if(res & 0x01)
        {
          n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n);
        }
        if(res & 0x02)
        {} // state switched to running
        if(res & 0x04)
        {
          //if (verbose)
            puts("starting new worker thread");
          zthread_new(worker_task, hpctfptr);
        }
        break;
      case TAKE:
//debug        printf("%s\n", "command take");
        if((res = capturetheflag(hpctfptr, 
                                 cmdptr->x, 
                                 cmdptr->y, 
                                 cmdptr->playername)) >= 0)
        {
          n = res 
              ? sprintf(buf, "%s", "TAKEN\n")
              : sprintf(buf, "%s", "INUSE\n");
        }
        else if (res == -4) // game is not running
        {
          if((n = sprintf(buf, 
                          "END %s\n", 
                          hpctfptr->winnername)) > 0)
            logoff(hpctfptr);
    // dis
        }
        else
        {
          // Disconnect, unknown player, slots full
          n = sprintf(buf, "NACK\n");
          logoff(hpctfptr);
        }
        break;
      case STATUS:
        n = sprintf(buf, "%d\n", (hpctfptr->fs->field[cmdptr->y][cmdptr->x]).flag);
        break;
      case UNKNOWN:
      default:
        printf("unknown command\n");
        // drop command 
        break;
    }    
  }
  else
  {
    printf("%s\n", "handlecommand, verification failed -> sending NACK");
    // Disconnect, unknown player, slots full
    
  } 

  n = n == 0 
        ? sprintf(buf, "%s", "NACK\n")
        : n;
  return n; 
}

static int s_timer_syncplid_event (zloop_t *loop, int timer_id, void *arg)
{ 
  //sync players
  bool fldbool[MAXPLAYER] = {FALSE};
  hpctf_game * hpctf = (hpctf_game *)arg;
  if(hpctf && !zctx_interrupted)
  {
    if(hpctf->verbose)
      puts("s_timer_syncplid_event");
    for (int x = 0; x < hpctf->fs->n; x++)
      for (int y = 0; y < hpctf->fs->n; y++)
      {
        int plid = hpctf->fs->field[y][x].flag;
        
        if(!fldbool[plid])
        {
          kvmap_setPlayerId(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->plidx[plid], plid);
          //printf("%d - %s \n", plid, hpctf->plidx[plid]);
          fldbool[plid] = TRUE;
        }   
      } 
  }

  return 0; 
}

static int s_timer_publishstate_event (zloop_t *loop, int timer_id, void *arg)
{
  hpctf_game * hpctf = (hpctf_game *)arg;
  if(hpctf && !zctx_interrupted)
  {
    if(hpctf->verbose)
      puts("s_timer_publishstate_event");
    kvmap_setState(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->gamestate);
    kvmap_setSize(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->fs->n);
    if(hpctf->verbose)
      printf("kvmsg state %d fldlen=%d\n", hpctf->gamestate, hpctf->fs->n);
  }
  return 0;
}


static int s_timer_syncfield_event (zloop_t * loop, int timer_id, void *arg)
{
  //sync players
  hpctf_game * hpctf = (hpctf_game*)arg;

  if(hpctf)
  {
    if(hpctf->verbose)
    {
      puts("s_timer_syncfield_event");
      printf("n: %d\n",hpctf->fs->n);
    }
    for (int x = 0; x < hpctf->fs->n; x++)//n; x++)
      for (int y = 0; y < hpctf->fs->n; y++)//n; y++)
      {
        int plid = hpctf->fs->field[y][x].flag; 
        if(hpctf->verbose)
          printf("hpctf->fs->field[%d][%d].flag: %d\n", y,x,plid);
        if (plid > 0)
        {
          char * ptmp = strdup(hpctf->plidx[plid]);
          if(ptmp)
            kvmap_setOwner(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, x, y, ptmp);

          free(ptmp);
        }
      }
  }

  return 0;
}



void startzmqserver(hpctf_game * hpctf)
{
  #ifndef WIN32
    s_catch_signals ();
  #else
    signal (SIGINT, s_signal_handler);
    signal (SIGTERM, s_signal_handler);
  #endif

  hpctf->frontend = zsocket_new(hpctf->ctx, ZMQ_ROUTER);
  hpctf->backend = zsocket_new(hpctf->ctx, ZMQ_ROUTER);
  hpctf->fldpublisher = zsocket_new (hpctf->ctx, ZMQ_PUB);
  int rc1 = zmq_bind(hpctf->frontend, "tcp://*:5555");
  assert(rc1 == 0);

  int rc2 = zmq_bind(hpctf->fldpublisher, "tcp://*:5556");
  assert(rc2 == 0);

  zsocket_bind(hpctf->backend, "ipc://backend.ipc");
  zclock_sleep(200);

  printf("%s\n", "before start worker");
  // start at least one worker - detached autonomous thread
  zthread_new(worker_task, hpctf);
  zmq_pollitem_t poller = { hpctf->backend, 0, ZMQ_POLLIN };
  zloop_poller(hpctf->loop, &poller, s_handle_backend, hpctf);

  // Socket to talk to clients
//  zloop_timer (hpctf->loop, 1000, 0, s_seq_cnt, hpctf);

  zloop_timer(hpctf->loop, 1000, 0, s_timer_publishstate_event, hpctf);
  zloop_timer(hpctf->loop, 1000, 0, s_timer_syncplid_event, hpctf);
  zloop_timer(hpctf->loop, 1000, 0, s_timer_syncfield_event, hpctf);

  // run reactor
  zloop_start(hpctf->loop);

  zloop_destroy(&hpctf->loop);
  printf("loop finished \n");
  return;
}


int main(int argc, char const *argv[])
{
  for (int i = -1; i < MAXPLAYER; ++i)//1787; ++i)
  {
    printcolor(i);
  }
  printcolor(0);
  printf("\n");

  usage(argc, argv);
  

  hpctf_game * hpctf = inithpctf(size);
  hpctf->verbose = verbose; 
  startzmqserver(hpctf);

  freehpctf(hpctf);

  printf("%s\n", "Server shut down");
  exit(0);
}

