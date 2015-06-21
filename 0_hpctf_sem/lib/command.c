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


// debugging and tracing
void cmd_dump(cmd * cmdptr)
{
  if (cmdptr) {
      if (!cmdptr) {
          fprintf (stderr, "NULL");
          return;
      }

      fprintf(stderr, "[cmd:%s]", cmd_stype(cmdptr));
      fprintf(stderr, "[n:%d]", cmdptr->n);
      fprintf(stderr, "[x:%d]", cmdptr->x);
      fprintf(stderr, "[y:%d]", cmdptr->y);
      fprintf(stderr, "[playername:%s]", cmdptr->playername);

/*
      fprintf (stderr, "[size:%zd] ", size);
      int char_nbr;
      for (char_nbr = 0; char_nbr < size; char_nbr++)
          fprintf (stderr, "%02X", body [char_nbr]);
*/
      fprintf (stderr, "\n");
  }
  else
      fprintf (stderr, "NULL message\n");
}

char * cmd_stype(cmd * cmdptr)
{
  if(cmdptr)
    switch(cmdptr->command)
    {
      case HELLO:
        return "HELLO"; 
        break;
      case SIZE:
        return "SIZE";
        break;
      case NACK:
        return "NACK";
        break;
      case START:
        return "START";
        break;
      case TAKEN:
        return "TAKEN";
        break;
      case INUSE:
        return "INUSE";
        break;
      case END:
        return "END";
        break;
      case TAKE:
        return "TAKE";    
        break;
      case STATUS:
        return "STATUS";  
        break;
      case PLAYER:
        return "PLAYER";
        break;
      case UNKNOWN: 
        return "UNKNOWN";
        break; 
      default:
        return "NOT DEFINED CMD ENUM";
        break;
    }
  return ""; 
}

cmd * initcommand(int argc, char *argv[])
{
  cmd * cmdptr = malloc(sizeof(cmd));
  cmdptr->n = 0;
  cmdptr->x = 0;
  cmdptr->y = 0; 
  cmdptr->playername = ""; 
  printf("argc: %d: %s\n", argc, argv[0]);
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
  else if(strcmp(argv[0], "END") == 0 && argc >= 2)
  {
    char tmppn[256]; 
    cmdptr->command = END;
    cmdptr->playername = argc > 2
                          ? sprintf(tmppn, "%s%s", argv[1], argv[2]) > 0
                              ? strdup(tmppn)
                              : argv[2]
                          : argv[1];
    

   // cmdptr->playername = argv[1]; // atoi(argv[1]);
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
  if(cmdptr)
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
  return FALSE;
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

