/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-04-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <apue.h>
#include <itskylib.h> 

#define WHITE " \t\n" // white spafe for tokenizing arguments
#define MAXARGC 50 // max number of arguments in buf 
 

void handlecommand(char * cmdstr)
{
    char *cmdstrdup = strdup(cmdstr);
    if(cmdstrdup == NULL)
      //insuficient memory, do some errorhandling. 
      return; 
    char *saveptr;
    char *ptr;
    char *argv[MAXARGC]; 
    int argc;

    if((ptr = strtok_r(cmdstrdup, WHITE, &saveptr)) == NULL)
    {
        printf("%s\n", "no args given");
        return;
    }

    argv[argc = 0] = cmdstrdup;
    while(ptr != NULL) {
        ptr = strtok_r(NULL, WHITE, &saveptr);

        if(++argc >= MAXARGC-1) // -1 for room for NULL at the end
            break;

        argv[argc] = ptr;
    }

    // handle command before free
    handle(argc, argv);

    // free cmdstrdup, cuz strdup does malloc inside
    free(cmdstrdup);
}

void handle(int argc, char const *argv[])
{
    // do some handle code, in this example, just print the arguments
    for(int i = 0; i < argc; i++)
        printf("argv[%d]='%s'\n", i, argv[i]);
}//p658