/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <apue.h>
#include <itskylib.h>
//#include <field.h>
//#include <game.h>
#include <zmq.h>
#include <assert.h>
#include <command.h>
#include <gamehelper.h>

void usage(const char *argv)
{
	printf("USAGE:\n\n%s fieldsizeGreater3\n", argv);
  exit(1);
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

int startzmqclient()
{
  char buffer[256];
  game_settings gs;

  // Socket to talk to clients
  printf("%s\n", "Connecting to hello world server ....");
  void * context = zmq_ctx_new();
  void * requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");

  int sentbytes = zmq_send(requester, "HELLO \n", 256, 0);
  int readbytes;
  if((readbytes = zmq_recv(requester, buffer, 256, 0)) > 0)
  {
    printf("Received bytes: %d msg %s \n", readbytes, buffer);
    cmd * cmdptr = parseandinitcommand(buffer);
    handlecommand(&gs, cmdptr);
    int docontinue = TRUE; 
    if(cmdptr->command == SIZE)
    {
      //int cnt = 0;
      while(docontinue){
        for (int y = 0; y < gs.fieldsize && docontinue; y++)
          for (int x = 0; x < gs.fieldsize && docontinue; x++)
          {
            pid_t pid = getpid();
            sprintf(buffer, "TAKE %d %d %d", y, x, pid);//%(MAXPLAYER) + 1);
            printf("sending: %s\n", buffer);
            sentbytes = zmq_send(requester, buffer, 256, 0);
            readbytes = zmq_recv(requester, buffer, 256, 0);
            printf("Received bytes: %d msg %s \n", readbytes, buffer);
            if(readbytes > 0)
            {
              free(cmdptr);
              cmdptr = parseandinitcommand(buffer); 
              handlecommand(&gs, cmdptr);
            }
            if(cmdptr->command == NACK || cmdptr->command == END)
              docontinue = FALSE;

            usleep(1*1000*100*((pid%MAXPLAYER)+1));
          }
      }
    }

    free(cmdptr);


  } 

  printf("Received bytes: %d msg %s \n", readbytes, buffer);
  printf("%s\n", "exit client");
/*

  int receivedbytes = zmq_recv(requester, buffer, 10, 0);

  int request_nbr;

  while(1){
    char buffer[256];
    int readbytes = zmq_recv(responder, buffer, 256, 0);
    if (readbytes <= -1)
    {
      printf("%s\n", "readbytes <= -1, check errno");
      handle_error(readbytes, "zmq_recv <= -1", PROCESS_EXIT);
      //continue;
    }

    buffer[readbytes] = '\0';

    printf("Received bytes: %d msg %s", readbytes, buffer);
    cmd * cmdptr = parseandinitcommand(buffer);
    handlecommand(hpctf_game * hpctfptr, cmd * cmdptr, responder);
 
    sleep(1);
  }



  for(request_nbr = 0; request_nbr != 10; request_nbr++)
  {
    char buffer [256];
    printf("Sending Hello %d ...\n", request_nbr);
    int sentbytes = zmq_send(requester, "HELLO \n", 256, 0);
    int receivedbytes = zmq_recv(requester, buffer, 10, 0);
    printf("Received World %d recbytes %d %s", request_nbr, receivedbytes, buffer);
  }
*/
  zmq_close(requester);
  zmq_ctx_destroy(context);

  return 0;
}

int main(int argc, char const *argv[])
{
	int n;
	if (argc < 2 || argc != 2 || (n = atoi(argv[1])) < 4)
    usage(argv[0]);

  printf("n: %d\n", n);
  //fldstruct fs; 

  printf("%s\n", "starting zmqclient");
  int res = startzmqclient();

  exit(res);
}

