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
#include <zmq.h>
#include <assert.h>

#include <gamehelper.h>


void usage(const char *argv)
{
	printf("USAGE:\n\n%s fieldsizeGreater3\n", argv);
  exit(1);
} 

void handlecommand(hpctf_game * hpctfptr, cmd * cmdptr, void * responder)
{
  char buf[256];
  int n;
  game_settings gs = getgamesettings(hpctfptr);

  int commandsucceed = verifycommand(cmdptr, &gs);
  if(commandsucceed == TRUE)
  {
    printf("%s\n", "handlecommand, verification sucess");
    if(hpctfptr->gamestate == FINISHED 
    && cmdptr->command != HELLO)
    {
      if((n = sprintf(buf, 
                      "END %s\n", 
//                      hpctfptr->winner, 
                      hpctfptr->winnername)) > 0)
      {
        zmq_send(responder, buf, 256, 0);
        logoff(hpctfptr);
        //zmq_send(responder, "START\n",256,0); 
      }
    }

    int res; 
    switch(cmdptr->command)
    {
      //case UNKNOWN:
      //  break;
      case HELLO:
        switch (logon(hpctfptr))
        {
          case 0:
            if((n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n)) > 0)
              zmq_send(responder, buf, 256, 0);
            break;
          case 1:
            // send async start
            if((n = sprintf(buf, "SIZE %d\n", hpctfptr->fs->n)) > 0)
            {
              zmq_send(responder, buf, 256, 0);
              //zmq_send(responder, "START\n",256,0); 
            }
            break;
          case -1:
          default:
            zmq_send(responder, "NACK\n",256,0);
            break;
        }
        break;
      case TAKE:
        printf("%s\n", "command take");
        if((res = capturetheflag(hpctfptr, 
                                 cmdptr->x, 
                                 cmdptr->y, 
                                 cmdptr->playername)) >= 0)
        {
          if(res == TRUE)
            zmq_send(responder, "TAKEN\n",256,0);
          // finished 
          if(res == FALSE)
            zmq_send(responder, "INUSE\n",256,0);
        }
        else
        {
          // Disconnect, unknown player, slots full
          zmq_send(responder, "NACK\n",256,0);
        }
        break;
      case STATUS:
        if((n = sprintf(buf, "%d\n", (hpctfptr->fs->field[cmdptr->y][cmdptr->x]).flag)) > 0)
        {
          zmq_send(responder, buf, 256, 0);
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
    // drop command, validation failed
    printf("%s\n", "handlecommand, verification failed");
  }


  printfield(hpctfptr->fs);
  printplayer(hpctfptr); 

}

void startzmqserver(hpctf_game * hpctf)
{
  // Socket to talk to clients
  void * context = zmq_ctx_new();
  void * responder = zmq_socket(context, ZMQ_REP);
  int rc = zmq_bind(responder, "tcp://*:5555");
  assert(rc == 0);

  while(1){
    char buffer[256];
    int readbytes = zmq_recv(responder, buffer, 256, 0);
//    if (readbytes <= -1)
//    {
//      printf("%s\n", "readbytes <= -1, check errno");
//      handle_error(readbytes, "zmq_recv <= -1", PROCESS_EXIT);
//      //continue;
//    }

    buffer[readbytes] = '\0';

    printf("Received bytes: %d msg %s \n", readbytes, buffer);
    cmd * cmdptr = parseandinitcommand(buffer);
    handlecommand(hpctf, cmdptr, responder);
    printf("%s\n", "after hanlde command");
    free(cmdptr);

 
    usleep(1*1000*100);
  }
}

int main(int argc, char const *argv[])
{
	int n;
	if (argc < 2 || argc != 2 || (n = atoi(argv[1])) < 4)
    usage(argv[0]);

  printf("n: %d\n", n);
  printf("res of sizeof(fldstruct) %d\n",sizeof(fldstruct)); 
 
  int a = -1;
  a = atoi("f");
  printf("a: %d\n", a);

//  cmd * cmdptr = parseandinitcommit("a b c d\targ5\narg6");
  //fldstruct fs; 

  hpctf_game * p_hpctf = inithpctf(n);
  printfield(p_hpctf->fs);

  logon(p_hpctf); // 5
  logon(p_hpctf); // 4
  logon(p_hpctf); // 3
  logon(p_hpctf); // 2
  logon(p_hpctf); // 1

  startzmqserver(p_hpctf);
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
  freehpctf(p_hpctf);

  printf("%s\n", "222222s");

  exit(0);
}

