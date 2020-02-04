
#include <unistd.h>

#include <apue.h>
#include <itskylib.h>
#include <czmq.h>
#include <assert.h>
#include <fieldhelper.h>
#include <kvmaphelper.h>

#include <kvsimple.h>

#define MAXPLAYER 1878 //1878 // -> somecolor, added new fieldprint. 1878 different stante could be printed so that the new max player cnt


int verbose = FALSE;
int updms = 5000;
int rawfld = FALSE;
int colfld = TRUE;

void usage(int argc, char const *argv[])
{
  printf("USAGE:\n\n%s\n", argv[0]);
  printf("\t-v\tVerbose\n");
  printf("\t-ms=5000\tUpdate in ms, default 5000\n");
  printf("\t-raw\tPrint field Raw with x y coords player, if not set print graphical\n");
  printf("\t-col\tPrint field Colored, if this is set explicity, raw and col are allowed(-raw -col)\n");
 
  for (int i = 1; i < argc; ++i)
  {
    if(strcmp(argv[i], "-v") == 0)
    {  
      verbose = TRUE; 
    }
    else if(strcmp(argv[i], "-raw") == 0)
    {  
      rawfld = TRUE; 
    }
    else if(strcmp(argv[i], "-col") == 0)
    {  
      colfld = TRUE; 
    }
    else if(strncmp(argv[i], "-ms=", 4) == 0)
    {
      sscanf(argv[i], "-ms=%d", &updms);
    }
  }
} 


static void 
updsubsriber_actor(zsock_t *pipe, void *args)

{
  zhash_t * kvmap = (zhash_t *)args;
  zsock_t *updsub = zsock_new_sub("tcp://localhost:5556", "");

  int64_t sequence = 0;

  while (!zsys_interrupted) {
      sequence++;
      kvmsg_t *kvmsg = kvmsg_recv (updsub);
      if (!kvmsg)
      {
        errno = zmq_errno(); 
        if (errno == EAGAIN) 
        { printf("I: EAGAIN! continue\n");
          continue; } 
        if (errno == ETERM) 
        { printf ("I: Terminated!\n"); 
          break; 
        } 
        printf ("E: (%d) %s\n", errno, strerror(errno)); 
        break;          //  Interrupted
      }
 
      if (verbose)
      {
        kvmsg_dump(kvmsg);
      }
      
      kvmsg_store (&kvmsg, kvmap);
  }

  zsock_destroy (&updsub);
  zsys_interrupted = 1; 

  char buf[100];
  snprintf(buf, 100, "updsubsriber_actor, Interrupted\n%" PRId64 "messages in\n", sequence);
  puts(buf);
}

int main(int argc, char const *argv[])
{
  usage(argc, argv);

  zhash_t *kvmap = zhash_new ();

  zactor_t *actor = zactor_new (updsubsriber_actor, kvmap);
  assert(actor);

  int size = kvmap_getSize(kvmap);
  fldstruct * fs = initfield(size);

  while(!zsys_interrupted)
  {
    size = kvmap_getSize(kvmap);
    if(fs->n != size)
    {
      freefield(fs); 
      fs = initfield(size);
    }

    kvmap_printGameSettings(kvmap);
    if(((rawfld == colfld) == FALSE) || colfld)
    {
      printfield(fs);
      printplayer(kvmap, fs);  
    }
    if(rawfld)
      fld_dump(kvmap, fs);

    usleep(updms*1000);//0*((pid%5)+1));
  }

  if(verbose)
  {
    printf("zctx_interrupted %d\n", zctx_interrupted);
    printf("zsys_interrupted %d\n", zsys_interrupted);
  }
    
  freefield(fs);
  zhash_destroy (&kvmap);
  zactor_destroy (&actor);
  return 0;
}

