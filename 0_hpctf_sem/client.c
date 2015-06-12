/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <unistd.h>

#include <apue.h>
#include <itskylib.h>
//#include <field.h>
//#include <game.h>
#include <czmq.h>
#include <assert.h>
#include <command.h>
#include <gamehelper.h>
#include <clistrategies.h>
#include <client.h>


int updms = 5000;
int pid = 0; 
int strat = 0; 

void usage(int argc, char const *argv[])
{
  printf("USAGE:\n\n%s\n", argv[0]);
  printf("\t-ms=5000\tUpdate in ms, default rand 1-1000ms\n");
  printf("\t-s=1\tStrategy 1-6 will calc with mod 6\n");

  updms=(randof(1000)+1)*1;
  pid = getpid();
  strat = pid;

  for (int i = 0; i < argc; ++i)
  {
    if(strncmp(argv[i], "-s=", 3) == 0)
    {  
      sscanf(argv[i], "-s=%d", &strat);
      break;
    }
    if(strncmp(argv[i], "-ms=", 4) == 0)
    {
      sscanf(argv[i], "-ms=%d", &updms);
      break;
    }
  }
}  

void handlecommand(game_settings * gs, cmd * cmdptr)
{
  if(verifycommand(cmdptr, gs) == TRUE)
  {
    switch(cmdptr->command)
    {
      case SIZE:
        // client set size
        gs->fieldsize = cmdptr->n; 
        break;
      case NACK:
        printf("%s\n", "NACK");
        //exit(0);
        // client logon failed
        break;
      case START:
        // start
        break;
      case TAKEN:
        printf("%s\n", "ctf succeed");
        break;
      case INUSE:
        printf("%s\n", "ctf failed");
        break;
      case PLAYER:
        //printf("%d\n", cmdptr->player);
        printf("%s\n", cmdptr->playername);
        break;
      case END:
        //printf("Game finished - WINNER: %d\n", cmdptr->player);
        printf("Game finished - WINNER: %s\n", cmdptr->playername);
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
  }

}

cmd * sendCmd(game_settings * gs, char * scmd)
{ 
  char buffer[256];
  snprintf(buffer, 256, "%s", scmd);
  printf("sending: %s\n", buffer);
  /*int sentbytes = */zmq_send(gs->requester, buffer, 256, 0);
  printf("receive\n");
  int readbytes = zmq_recv(gs->requester, buffer, 256, 0);
  printf("Received bytes: %d msg %s \n", readbytes, buffer);
  if(readbytes > 0)
  {
    cmd * cmdptr = parseandinitcommand(buffer); 
    handlecommand(gs, cmdptr);
    return cmdptr;
  } 
  return NULL;  
}

int sendHello(game_settings * gs)
{
  int retres = FALSE;
  cmd * cmdptr = sendCmd(gs, "HELLO \n");

  if(cmdptr->command == SIZE)
    retres = TRUE;

  if (cmdptr)
    free(cmdptr);

  return retres;
}

int sendTake(game_settings * gs, int x, int y, int pid)
{
  int retres = FALSE; 
  char buffer[256];
  sprintf(buffer, "TAKE %d %d %d \n", x, y, pid);//%(MAXPLAYER) + 1);

  cmd * cmdptr = sendCmd(gs, buffer);

  if(cmdptr && (cmdptr->command == TAKEN || cmdptr->command == INUSE))
    retres = TRUE;

  if(cmdptr)
    free(cmdptr);

  return retres;
}

void strategie(game_settings * gs)
{
  int res = sendHello(gs);
  if(res)
    switch(strat % 6)
    { 
      case 0:
        strategie1(pid, gs );
        break;
      case 1: 
        strategie2(pid, gs );
        break;
      case 2: 
        strategie3(pid, gs );
        break;
      case 3: 
        strategie4(pid, gs );
        break;
      case 4: 
        strategie5(pid, gs );
        break;
      case 5: 
      default:
        strategie6(pid, gs );
        break;
    }
}



int startzmqclient()
{
  game_settings * gs = malloc(sizeof(game_settings));

  // Socket to talk to clients
  printf("%s\n", "Connecting to hello world server ....");
  void * context = zmq_ctx_new();
  void * requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");

  gs->requester = requester;
  gs->updms = updms;

  strategie(gs);

  printf("%s\n", "exit client");

  zmq_close(requester);
  zmq_ctx_destroy(context);

  free(gs);

  return 0;
}

int main(int argc, char const *argv[])
{
  srandom ((unsigned) time (NULL));
	//if (argc < 1 )//|| argc != 2 || (n = atoi(argv[1])) < 4)
  usage(argc, argv);

  printf("%s\n", "starting zmqclient");
  int res = startzmqclient();

  exit(res);
}

