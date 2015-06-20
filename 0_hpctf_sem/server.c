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

void handlecommand(hpctf_game * hpctfptr, cmd * cmdptr, int64_t * seq)
{
  char buf[256];
  int n;
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
        zmq_send(hpctfptr->responder, buf, 256, 0);
        logoff(hpctfptr);
        //zmq_send(responder, "START\n",256,0); 
      }
    }



    int res; 
    switch(cmdptr->command)
    {
      //case UNKNOWN:
      //  break;
      case SIZE: 
        if((n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n)) > 0)
              zmq_send(hpctfptr->responder, buf, 256, 0);
      case HELLO:
        switch (logon(hpctfptr))
        {
          case 0:
            if((n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n)) > 0)
              zmq_send(hpctfptr->responder, buf, 256, 0);
            break;
          case 1:
            // send async start
            if((n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n)) > 0)
            {
              zmq_send(hpctfptr->responder, buf, 256, 0);
              //zmq_send(responder, "START\n",256,0); 
            }
            break;
          case -1:
          default:
            zmq_send(hpctfptr->responder, "NACK\n",256,0);
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
          if(res == TRUE)
          {
            zmq_send(hpctfptr->responder, "TAKEN\n",256,0);
          }
          // finished 
          if(res == FALSE)
            zmq_send(hpctfptr->responder, "INUSE\n",256,0);
        }
        else
        {
          // Disconnect, unknown player, slots full
          zmq_send(hpctfptr->responder, "NACK\n",256,0);
        }
        break;
      case STATUS:
        if((n = sprintf(buf, "%d\n", (hpctfptr->fs->field[cmdptr->y][cmdptr->x]).flag)) > 0)
        {
          zmq_send(hpctfptr->responder, buf, 256, 0);
        }
        break;
      case UNKNOWN:
      default:
        // drop command 
        break;
    }    
  }
  else
  {
    printf("%s\n", "handlecommand, verification failed -> sending NACK");
    // Disconnect, unknown player, slots full
    zmq_send(hpctfptr->responder, "NACK\n",256,0);

    // drop command, validation failed

  } 
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
    int rc = zmq_recv(hpctf->responder, buffer, 256, 0);
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
    int rc = zmq_recv(hpctf->responder, buffer, 256, 0);
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
          setPlayerid(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->plidx[plid], plid);
          //printf("%d - %s \n", plid, hpctf->plidx[plid]);
          fldbool[plid] = TRUE;
        }   
      } 
  }

  return 0; 
}

static int s_timer_publishstate_event (zloop_t *loop, int timer_id, void *arg)
{
  printf("s_timer_publishstate_event\n");
  hpctf_game * hpctf = (hpctf_game *)arg;
  if(hpctf && !zctx_interrupted)
  {
    setState(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->gamestate);
    setSize(hpctf->kvmap, hpctf->seq++, hpctf->fldpublisher, hpctf->fs->n);
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

void startzmqserver(hpctf_game * hpctf)
{
  #ifndef WIN32
    s_catch_signals ();
  #else
    signal (SIGINT, s_signal_handler);
    signal (SIGTERM, s_signal_handler);
  #endif


  int rc1 = zmq_bind(hpctf->responder, "tcp://*:5555");
  assert(rc1 == 0);

  int rc2 = zmq_bind(hpctf->fldpublisher, "tcp://*:5556");
  assert(rc2 == 0);
  zclock_sleep(200);

  // Socket to talk to clients
//  zloop_timer (hpctf->loop, 1000, 0, s_seq_cnt, hpctf);

  zloop_timer(hpctf->loop, 1000, 0, s_timer_publishstate_event, hpctf);
  zloop_timer(hpctf->loop, 1000, 0, s_timer_syncplid_event, hpctf);

//  zloop_timer(hpctf->loop, 1000, 1, s_handlerupdgamesettings, hpctf);
  zthread_fork(hpctf->ctx, updgamesettings_task, hpctf);

  // run reactor
  zloop_start(hpctf->loop);

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

int main(int argc, char const *argv[])
{
  for (int i = -1; i < 1878; ++i)//1787; ++i)
  {
    printcolor(i);
  }
  puts("");

  usage(argc, argv);
  

  hpctf_game * hpctf = inithpctf(size);
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

