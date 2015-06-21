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

int handlecommand(char * buf, hpctf_game * hpctfptr, cmd * cmdptr, int64_t * seq)
{
//  char buf[256];
  int n = 0;
  game_settings gs = getgamesettings(hpctfptr);
  cmd_dump(cmdptr);

  int commandsucceed = verifycommand(cmdptr, &gs);
  printf("verify command: %d\n", commandsucceed);
  if(commandsucceed == TRUE)
  {
//debug    printf("%s\n", "handlecommand, verification sucess");
    if(hpctfptr->gamestate == FINISHED 
    && cmdptr->command != HELLO)
    { 

      if((n = sprintf(buf, 
                      "END %s\n", 
//                      hpctfptr->winner, 
                      hpctfptr->winnername)) > 0)
      {

//        zmq_send(hpctfptr->frontend, buf, 256, 0);
        logoff(hpctfptr);
        return n; 
        //zmq_send(frontend, "START\n",256,0); 
      }
    }



    int res; 
    switch(cmdptr->command)
    {
      //case UNKNOWN:
      //  break;
//      case SIZE: 
//        if((n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n)) > 0)
//              zmq_send(hpctfptr->frontend, buf, 256, 0);
      case HELLO:
        switch (logon(hpctfptr))
        {
          case 0:
            n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n);
//              zmq_send(hpctfptr->frontend, buf, 256, 0);
            break;
          case 1:
            // send async start
            n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n);
   //         {
   //           zmq_send(hpctfptr->frontend, buf, 256, 0);
   //           //zmq_send(frontend, "START\n",256,0); 
   //         }
            break;
          case -1:
          default:
            n = sprintf(buf, "%s", "NACK\n");
//            zmq_send(hpctfptr->frontend, "NACK\n",256,0);
            break;
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
        else
        {
          // Disconnect, unknown player, slots full
          n = sprintf(buf, "NACK\n");
        }
        break;
      case STATUS:
        n = sprintf(buf, "%d\n", (hpctfptr->fs->field[cmdptr->y][cmdptr->x]).flag);
        //if((n = sprintf(buf, "%d\n", (hpctfptr->fs->field[cmdptr->y][cmdptr->x]).flag)) > 0)
        //{
        //  zmq_send(hpctfptr->frontend, buf, 256, 0);
        //}
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
    n = sprintf(buf, "%s", "NACK\n");
  } 

  return n; 
}


static void updgamesettings_task(void *args, zctx_t *ctx, void *pipe)
{
  hpctf_game * hpctf = (hpctf_game *)args;
  //if(!zctx_interrupted)   
  //  zloop_start (hpctf->loop);
  int64_t sequence = 0; 

  while(!zctx_interrupted){
    printf("seq: %" PRId64 "\n", sequence++);

    char buffer[256];
    int rc = zmq_recv(hpctf->frontend, buffer, 256, 0);
    if (rc == -1) 
    {
      errno = zmq_errno(); 
      if (errno == EAGAIN) 
      { continue; } 
      if (errno == ETERM) 
      { printf ("I: Terminated!\n"); 
        break; 
      } 
      printf ("E: (%d) %s\n", errno, strerror(errno)); 
      break; 
    }

    buffer[rc] = '\0';

    printf("Received bytes: %d msg %s", rc, buffer);
    cmd * cmdptr = parseandinitcommand(buffer);
    char scmd256[256];
    int n = handlecommand(scmd256, hpctf, cmdptr, &sequence);
    printf("%s\n", "after hanlde command");
    if (n <= 0)
    {
      puts("handlecommand failed");
    }
    zmq_send(hpctf->frontend, scmd256,256,0); 
    free(cmdptr);
  }
  if (zctx_interrupted)//s_interrupted == 1)
  {
    printf("INTERRUPT RECEIVED, killing server");
  }


  printf("leave task");

}



//static void updgamesettings_task(void *args, zctx_t *ctx, void *pipe)
/*
static int s_handlerupdgamesettings (zloop_t *loop, int timer_id, void *arg)
{
  hpctf_game * hpctf = (hpctf_game *)arg;
  //if(!zctx_interrupted)   
  //  zloop_start (hpctf->loop);
  int64_t sequence = 0; 

  while(!zctx_interrupted){
    printf("seq: %" PRId64 "\n", sequence++);

    char buffer[256];
    int rc = zmq_recv(hpctf->frontend, buffer, 256, 0);
    if (rc == -1) 
    {
      errno = zmq_errno(); 
      if (errno == EAGAIN) 
      { continue; } 
      if (errno == ETERM) 
      { printf ("I: Terminated!\n"); 
        break; 
      } 
      printf ("E: (%d) %s\n", errno, strerror(errno)); 
      break; 
    }

    buffer[rc] = '\0';

    printf("Received bytes: %d msg %s", rc, buffer);
    cmd * cmdptr = parseandinitcommand(buffer);
    handlecommand(hpctf, cmdptr, &sequence);
    printf("%s\n", "after hanlde command");
    free(cmdptr);
  }
  if (zctx_interrupted)//s_interrupted == 1)
  {
    printf("INTERRUPT RECEIVED, killing server");
  }

  printf("leave task");

  return 0;
}
*/

static int s_timer_syncplid_event (zloop_t *loop, int timer_id, void *arg)
{
  puts("s_timer_syncplid_event");
  //sync players
  bool fldbool[MAXPLAYER] = {FALSE};
  hpctf_game * hpctf = (hpctf_game *)arg;
  if(hpctf && !zctx_interrupted)
  {
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
  puts("s_timer_publishstate_event");
  hpctf_game * hpctf = (hpctf_game *)arg;
  if(hpctf && !zctx_interrupted)
  {
    kvmap_setState(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->gamestate);
    kvmap_setSize(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->fs->n);
    if(hpctf->verbose)
      printf("kvmsg state %d fldlen=%d\n", hpctf->gamestate, hpctf->fs->n);
  }
  return 0;
}

/*
static int
s_seq_cnt (zloop_t *loop, int timer_id, void *args)
{
    printf("%s\n", "in s_seq_cnt");
    hpctf_game * hpctf = (hpctf_game *) args;
    printf("chk1\n");
    hpctf->seq++; 

    printf("chk1\n");
    printf("sseqcnt %" PRId64 "\n", hpctf->seq);
    return 0;
}
*/

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
      { continue; } 
      if (errno == ETERM) 
      { 
        printf ("I: Terminated!\n"); 
        break; 
      } 
      printf ("E: (%d) %s\n", errno, strerror(errno)); 
      break; 
    }

    frame = zmsg_last (msg);
//    if(verbose)
//      zframe_print (frame, "Worker");

    char * sval = zframe_strdup(frame);
    if(sval)
    {
      cmd * cmdptr = parseandinitcommand(sval);
      char scmd256[256];

      int n = handlecommand(scmd256, hpctf, cmdptr, &sequence);
      
      printf("scmd256: '%s'\n", scmd256); 
      if (n <= 0)
      {
        zframe_reset(frame, "NACK\n", 5);
      }
      else
      {
        zframe_reset(frame, scmd256, 256);
      }
      free(cmdptr);
    }
    else
    {
      zframe_reset(frame, "NACK\n", 5);
    }

//    if(verbose)
//      zframe_print (zmsg_last (msg), "Worker: Send:");

    zmsg_send (&msg, worker);
    free(sval);
  }
  zctx_destroy (&ctx);
  return NULL;
}

static int s_timer_syncfield_event (zloop_t * loop, int timer_id, void *arg)
{
  puts("s_timer_syncfield_event");
  //sync players
  hpctf_game * hpctf = (hpctf_game*)arg;

  if(hpctf)
  {
    if(hpctf->verbose)
      printf("n: %d\n",hpctf->fs->n);
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
//  zloop_timer(hpctf->loop, 1000, 1, s_handlerupdgamesettings, hpctf);


//  zthread_fork(hpctf->ctx, updgamesettings_task, hpctf);

  // run reactor
  zloop_start(hpctf->loop);

  zloop_destroy(&hpctf->loop);
  printf("loop finished \n");
  return;
}



struct thread_info {    /* Used as argument to thread_start() */
  pthread_t thread_id;        /* ID returned by pthread_create() */
  int       thread_num;       /* Application-defined thread # */
  hpctf_game* hpctf;
}; //counterstruct shared;

/*
void * threadwork_zmqserver(void *args)
{
  struct thread_info *tinfo = args;
  
  startzmqserver(tinfo->hpctf);

  return NULL;
}
*/

/*
void * threadwork_printfld(void *args)
{
  struct thread_info *tinfo = args;
  
  while(1)
  {
    printfield(tinfo->hpctf->fs);
    printplayer(tinfo->hpctf);
    usleep(1*1000*100);
  }

  return NULL;
}
*/

int main(int argc, char const *argv[])
{
  for (int i = -1; i < 1878; ++i)//1787; ++i)
  {
    printcolor(i);
  }
  puts("");

  usage(argc, argv);
  

  hpctf_game * hpctf = inithpctf(size);
  hpctf->verbose = verbose; 
  startzmqserver(hpctf);

//  printf("%s\n", "before free");
//  sleep(2);
  freehpctf(hpctf);

  printf("%s\n", "222222s");


    //logon(p_hpctf); // 5
  //logon(p_hpctf); // 4
  //logon(p_hpctf); // 3
  //logon(p_hpctf); // 2
  //logon(p_hpctf); // 1
/*
  struct thread_info *tinfo;
  tinfo = calloc(2, sizeof(struct thread_info));
  if (tinfo == NULL)
    err_sys("calloc");
*/
  /*

  int ptcres =0;
  void * res;
  for(int i = 0; i < 2; i++)
  {
    tinfo[i].thread_num = i; 
    tinfo[i].hpctf = p_hpctf;


    switch(i)
    {
      case 0: 
        ptcres = pthread_create(&tinfo[i].thread_id, NULL, &threadwork_zmqserver, &tinfo[i]);
        break;
      case 1: 
        ptcres = pthread_create(&tinfo[i].thread_id, NULL, &threadwork_printfld, &tinfo[i]);
        break;
    }
    // the pthread_create() call stores the thread id into corresponding element 
    // of tinfo[]
    //if (ptcres != 0)
    //  handle_error_en(ptcres, "pthread_create");

    printf("thread created: %d; \n",
        tinfo[i].thread_num);
  } 

  */

  /*
  // join with each thread, and display its returned value 
  for (int i = 0; i < 2; i++) {
    ptcres = pthread_join(tinfo[i].thread_id, &res);
    //if (ptcres != 0)
    //    handle_error_en(ptcres, "pthread_join");

    printf("Joined with thread %d; \n",
            tinfo[i].thread_num);
    //free(res);      //Free memory allocated by thread 
  }

  free(tinfo);
  */
  /*
//  someclients(p_hpctf->fs);
  game_settings gs;
  //MAXPLAYER || 6
  //if(MAXPLAYER - logon(p_hpctf) >= 2)
  logon(p_hpctf); // 5
  logon(p_hpctf); // 4
  logon(p_hpctf); // 3
  gs = getgamesettings(p_hpctf); 
  printgamesettings(&gs);
  logon(p_hpctf); // 2
  gs = getgamesettings(p_hpctf); 
  printgamesettings(&gs);
  logon(p_hpctf); // 1
  logon(p_hpctf); // 0
//  logon(p_hpctf); // 0

  capturetheflag(p_hpctf, 4,2,3);
  */

  exit(0);
}

