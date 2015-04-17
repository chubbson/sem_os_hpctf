/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-04-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <apue.h>
#include <itskylib.h> 
#include <gamehelper.h>
#include <command.h> 

#define WHITE " \t\n" // white spafe for tokenizing arguments
#define MAXARGC 50 // max number of arguments in buf 

cmd * initcommand(int argc, char *argv[])
{
  cmd * cmdptr = malloc(sizeof(cmd));
  if(strcmp(argv[0], "HELLO") == 0 && argc == 1)
  {
    cmdptr->command = HELLO;
  }
  else if(strcmp(argv[0], "SIZE") == 0 && argc == 2)
  {
    cmdptr->command = SIZE;
    int n = atoi(argv[1]);
    cmdptr->n = n;
  }
  else if(strcmp(argv[0], "NACK") == 0 && argc == 1)
  {
    cmdptr->command = NACK;
  }
  else if(strcmp(argv[0], "START") == 0 && argc == 1)
  {
    cmdptr->command = START;
  }
  else if(strcmp(argv[0], "TAKE") == 0 && argc == 4)
  {    
    cmdptr->command = TAKE;
    cmdptr->x = atoi(argv[1]);
    cmdptr->y = atoi(argv[2]);
    cmdptr->playername = argv[3];// = argv[3]; 
  }
  else if(strcmp(argv[0], "TAKEN") == 0 && argc == 1)
  {
    cmdptr->command = TAKEN;
  }
  else if(strcmp(argv[0], "INUSE") == 0 && argc == 1)
  {
    cmdptr->command = INUSE;
    // field in use, cannot be taken
  }
  else if(strcmp(argv[0], "STATUS") == 0 && argc == 3)
  {
    cmdptr->command = STATUS;
    cmdptr->x = atoi(argv[1]);
    cmdptr->y = atoi(argv[2]);
  }
  else if(strcmp(argv[0], "END") == 0 && argc == 2)
  {
    cmdptr->command = END;
    cmdptr->playername = argv[1]; // atoi(argv[1]);
  }
///  else if((cmdptr->player = atoi(argv[0])) > 0)
///  {
///    cmdptr->command = PLAYER;
///  }
  else 
  {
    cmdptr->command = UNKNOWN; 
  }

  return cmdptr;
}

int verifycommand(cmd * cmdptr, game_settings * settings)
{
  switch(cmdptr->command)
  {
    case HELLO:
    case SIZE:
    case NACK:
    case START:
    case TAKEN:
    case INUSE:
    case END:
      return TRUE;
      break;
    case TAKE:
      if( cmdptr->x >= 0 
       && cmdptr->y >= 0
       && cmdptr->x < settings->fieldsize
       && cmdptr->y < settings->fieldsize)
//       && cmdptr->player > 1 
//       && cmdptr->player <= settings->fullslots)
        return TRUE;       
      break;
    case STATUS:
      if( cmdptr->x >= 0 
       && cmdptr->y >= 0
       && cmdptr->x < settings->fieldsize
       && cmdptr->y < settings->fieldsize)
        return TRUE;   
      break;
    case PLAYER:
//      if( cmdptr->player > 1 
//       && cmdptr->player <= settings->fullslots)  
        return TRUE;  
      break;
    case UNKNOWN: 
    default:
      return FALSE;
      break;
  } 
}

cmd * parseandinitcommand(char * cmdstr)
{
  char *cmdstrdup = strdup(cmdstr);
  if(cmdstrdup == NULL)
  //insuficient memory, do some errorhandling. 
    return NULL; 
  char *saveptr;
  char *ptr;
  char *argv[MAXARGC]; 
  int argc; 

  if((ptr = strtok_r(cmdstrdup, WHITE, &saveptr)) == NULL)
  {
    printf("%s\n", "no args given");
    return NULL;
  }

  argv[argc = 0] = cmdstrdup;
  while(ptr != NULL) {
    ptr = strtok_r(NULL, WHITE, &saveptr);

    if(++argc >= MAXARGC-1) // -1 for room for NULL at the end
      break;

    argv[argc] = ptr;
  }

  // handle command before free
  cmd * cmdptr = initcommand(argc, argv);

  // free cmdstrdup, cuz strdup does malloc inside
  free(cmdstrdup);

  return cmdptr;
}

