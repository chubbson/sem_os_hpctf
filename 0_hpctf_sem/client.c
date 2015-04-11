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

void usage(const char *argv)
{
	printf("USAGE:\n\n%s fieldsizeGreater3\n", argv);
  exit(1);
} 

int startzmqclient()
{
  // Socket to talk to clients
  printf("%s\n", "Connecting to hello world server ...");
  void * context = zmq_ctx_new();
  void * requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");

  int request_nbr;
  for(request_nbr = 0; request_nbr != 10; request_nbr++)
  {
    char buffer [256];
    printf("Sending Hello %d ...\n", request_nbr);
    int sentbytes = zmq_send(requester, "HELLO \n", 256, 0);
    int receivedbytes = zmq_recv(requester, buffer, 10, 0);
    printf("Received World %d recbytes %d %s", request_nbr, receivedbytes, buffer);
  }

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

