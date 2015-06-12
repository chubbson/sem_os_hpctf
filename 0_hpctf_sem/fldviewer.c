
#include <unistd.h>

#include <apue.h>
#include <itskylib.h>
#include <czmq.h>
#include <assert.h>
#include <field.h>
#include <kvmaphelper.h>

#include <kvsimple.h>

#define MAXPLAYER 1878 //1878 // -> somecolor, added new fieldprint. 1878 different stante could be printed so that the new max player cnt

int verbose = FALSE; 
int updms = 5000;

static void updsubscriber_task(void *args, zctx_t *ctx, void *pipe)
{
    zhash_t *kvmap = (zhash_t *)args;
    void *updsub = zsocket_new (ctx, ZMQ_SUB);
    zsocket_set_subscribe (updsub, "");
    zsocket_connect (updsub, "tcp://localhost:5556");

    int64_t sequence = 0;

    while (!zctx_interrupted) {
        sequence++;
        kvmsg_t *kvmsg = kvmsg_recv (updsub);
        if (verbose)
          kvmsg_dump(kvmsg);
        if (!kvmsg)
            break;          //  Interrupted
        
        kvmsg_store (&kvmsg, kvmap);
    }

    printf (" Interrupted\n%d messages in\n", (int) sequence);
}

void usage(int argc, char const *argv[])
{
  printf("%s\n", "fldviewer");
  printf("\t-v\tVerbose\n");
  printf("\t-ms=5000\tUpdate in ms, default 5000\n");

  for (int i = 0; i < argc; ++i)
  {
    if(strcmp(argv[i], "-v") == 0)
    {  
      verbose = TRUE; 
      break;
    }
    if(strncmp(argv[i], "-ms=", 4) == 0)
    {
      sscanf(argv[i], "-ms=%d", &updms);
      break;
    }
  }
} 

int main(int argc, char const *argv[])
{
  usage(argc, argv);

  //  Prepare our context and updates socket
  zctx_t *ctx = zctx_new ();
  zhash_t *kvmap = zhash_new ();

  zthread_fork(ctx, updsubscriber_task, kvmap);

  int size = getSize(kvmap);
  fldstruct * fs = initfield(size);


  while(!zctx_interrupted)
  {
    size = getSize(kvmap);
    if(fs->n != size)
    {
      freefield(fs); 
      fs = initfield(size);
    }

    bool players[MAXPLAYER] = {FALSE};
    char * prntfld =  '\0';
    int res = (size*size*30 + size + 1) * sizeof(char);
    prntfld = malloc(res);
    prntfld[0] = '\0';
    char buf[15];
    buf[0] = '\0';

    for (int y = 0; y < size/* && !zctx_interrupted*/; ++y)
      for (int x = 0; x < size/* && !zctx_interrupted*/; ++x)
      {
        char * playername = getOwner(kvmap, x, y);
        int plid = getPlayerId(kvmap, playername);
        if(verbose)
          printf("%d:[%d][%d] - %d:\t%s\n", size, x, y, plid, playername);
        fs->field[y][x].flag = plid;

        if(plid > 0 && players[plid] == FALSE)
        { 
          players[plid] = TRUE;
          sprintcolfield(plid, prntfld);
          int n = sprintf(buf, ":%d=%s, ", plid, playername);
          buf[n] = '\0';
          strcat(prntfld, buf);
        }
      }
    printfield(fs);
    printGameSettings(kvmap);
    printf("%s\n", prntfld);
    free(prntfld);

    usleep(updms*1000);//0*((pid%5)+1));
  }
  freefield(fs);

  zhash_destroy (&kvmap);
  zctx_destroy (&ctx);
  return 0;
}
