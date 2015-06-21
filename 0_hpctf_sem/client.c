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
  printf("\t-ms=5000\tUpdate in ms, default rand 0-999ms\n");
  printf("\t-s=1\tStrategy 1-6 will calc with mod 6\n");

  updms=(randof(1000))*1;
  pid = getpid();
  strat = pid;

  for (int i = 0; i < argc; ++i)
  {
    if(strncmp(argv[i], "-s=", 3) == 0)
    {  
      sscanf(argv[i], "-s=%d", &strat);
      continue;
    }
    if(strncmp(argv[i], "-ms=", 4) == 0)
    {
      sscanf(argv[i], "-ms=%d", &updms);
      continue;
    }
  }
}  

void handlecommand(game_settings * gs, cmd * cmdptr)
{
  cmd_dump(cmdptr);
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
  printf("leave handle commad\n");
}

#define REQUEST_TIMEOUT     1000
#define MAX_RETRIES         3       //  Before we abandon

static zmsg_t * 
s_try_request(zctx_t *ctx, char * endpoint, zmsg_t *request)
{

//  void * requester = zmq_socket(ctx, ZMQ_REQ);
//  zmq_connect(requester, "tcp://localhost:5555");
  void * client = zsocket_new(ctx, ZMQ_REQ);
  zsocket_connect(client, endpoint);

  //printf("%s\n", zstr_recv(client)); 
    zmsg_t *msg = zmsg_dup (request);
    zmsg_send (&msg, client);
    zmq_pollitem_t items [] = { { client, 0, ZMQ_POLLIN, 0 } };
    zmq_poll (items, 1, REQUEST_TIMEOUT * ZMQ_POLL_MSEC);
    zmsg_t *reply = NULL;
    if (items [0].revents & ZMQ_POLLIN)
      reply = zmsg_recv (client);
    //  Close socket in any case, we're done with it now
    zsocket_destroy (ctx, client);
    return reply;
}


cmd * sendCmd(game_settings * gs, char * scmd)
{ 
  char * buffer = malloc(sizeof(char)*256);//[256];
  snprintf(buffer, 256, "%s", scmd);
  printf("sending: %s\n", buffer);


    
  zmsg_t *request = zmsg_new ();
  zmsg_addstr (request, buffer);
  zmsg_t *reply = NULL;
  cmd * cmdrepl = NULL;
/*
    zstr_send(gs->requester, buffer);
    buffer = zstr_recv(gs->requester);

  if(buffer)
  {
    cmdrepl = parseandinitcommand(buffer); 
    handlecommand(gs, cmdrepl);
    free(buffer);
  }*/

  for (int retries = 0; retries < MAX_RETRIES; retries++) 
  {
    //zmsg_send (&request, gs->requester);
    //reply = zmsg_recv(gs->requester);
    reply  = s_try_request (gs->ctx, gs->endpoint, request);
    
    if(reply)
    {
      buffer = zframe_strdup(zmsg_last (reply));
      printf("buffer: %s\n", buffer);
      break;
    }
    printf ("W: no response from %s, retrying...\n", gs->endpoint);
  }
  if(reply)
  {
    cmdrepl = parseandinitcommand(buffer); 
    handlecommand(gs, cmdrepl);
    free(buffer);
  } 

  zmsg_destroy(&request);
  zmsg_destroy(&reply);
  //free(buffer);
  //cmd_dump(cmdrepl);
  return cmdrepl;  
////  printf("response: '%s'",zframe_strdup(zmsg_last (reply)); 
    
//  printf("buffer: '%s'\n", buffer);
//  printf("req\n");
//  zmsg_dump(reply);

//  printf("after zmsg_dump\n");
  ///*int sentbytes = */zmq_send(gs->requester, buffer, 256, 0);
////  zstr_send(gs->requester, buffer);
////  printf("receive\n");

//  int readbytes = zmq_recv(gs->requester, buffer, 256, 0);
////  buffer = zstr_recv(gs->requester);


//  int readbytes = 5; 
//  printf("Received bytes: %d msg '%s' \n", readbytes, &buffer[0]);
/*  if(buffer!=NULL)
  {
    cmd * cmdptr = parseandinitcommand(buffer); 
    handlecommand(gs, cmdptr);
    free(buffer);
    return cmdptr;
  } 
*/

}

int sendHello(game_settings * gs)
{
  int retres = FALSE;
  cmd * cmdptr = sendCmd(gs, "HELLO \n");

  if(cmdptr && cmdptr->command == SIZE)
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

  printf("taken res : %d\n", retres);
  return retres;
}

void strategie(game_settings * gs)
{
  int strategie = strat%6;
  int res = sendHello(gs);
  if(res)
    switch(strategie)
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
        printf("strat4\n");
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

  // Socket to talk to clients
  printf("%s\n", "Connecting to hello world server ....");
  zctx_t * context = zctx_new();// zmq_ctx_new();
//  void * context = zmq_ctx_new();
//  void * requester = zmq_socket(context, ZMQ_REQ);
//  zmq_connect(requester, "tcp://localhost:5555");

  game_settings * gs = malloc(sizeof(game_settings));
//  gs->requester = requester;
  gs->endpoint = "tcp://localhost:5555";
  gs->updms = updms;
  gs->ctx = context; 

  strategie(gs);

  printf("%s\n", "exit client");

// zmq_close(requester);
  zctx_destroy(&context);
//  zmq_ctx_destroy(context);

  free(gs);

  return 0;
}

int main(int argc, char const *argv[])
{
  srandom ((unsigned) time (NULL));
  usage(argc, argv);

  printf("%s\n", "starting zmqclient");
  int res = startzmqclient();

  exit(res);
}

