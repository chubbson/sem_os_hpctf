
#include <unistd.h>
#include <apue.h>
#include <itskylib.h>
#include <field.h>
/*
struct thread_info {     // Used as argument to thread_start() 
  pthread_t thread_id;   // ID returned by pthread_create() 
  int       thread_num;  // Application-defined thread # 
  int       player;
  fldstruct *fs;         // Pointer to fldstruct var *  
}; //counterstruct shared;

void * strategy1(void * args)
{ 
  struct thread_info *tinfo = args;
  sleep(5);
  take(tinfo->fs, 4, 4, tinfo->player);

  return NULL;
}

void * strategy2(void * args)
{ 
  struct thread_info *tinfo = args;

  take(tinfo->fs, 1,1, 1);
  take(tinfo->fs, 1,2, 2);
  printfield(tinfo->fs);
  sleep(1);
  take(tinfo->fs, 1,3, 3);
  printfield(tinfo->fs);
  sleep(1);
  take(tinfo->fs, 2,2, 4);
  printfield(tinfo->fs);
  sleep(1);
  take(tinfo->fs, 3,2, 5);
  printfield(tinfo->fs);
  sleep(1);
  take(tinfo->fs, 3,3, 6);

  return NULL;
}

void * strategy3(void * args)
{ 
  struct thread_info *tinfo = args;
  int resval = -1; 
  int n = tinfo->fs->n;

  for(int i = 0; i < 30 && resval != 0; i++)
    for(int y = 0; y < n && resval != 0; y++)
      for(int x = 0; x < n && resval != 0; x++)
      {
        resval = take(tinfo->fs, x, y, tinfo->player);
        printfield(tinfo->fs);
        usleep(200000);
      }

  return NULL;
}

void * strategy4(void * args)
{ 
  struct thread_info *tinfo = args;
  int resval = -1; 
  int n = tinfo->fs->n;

  for(int i = 0; i < 30 && resval != 0; i++)
    for(int y = 0; y < n && resval != 0; y++)
      for(int x = 0; x < n && resval != 0; x++)
      {
        resval = take(tinfo->fs, y, x, tinfo->player);
        printfield(tinfo->fs);
        usleep(1750);
      }

  return NULL;
}

*/
/*

void someclients(fldstruct *fs)
{
  struct thread_info *tinfo;
  void  *res;  
  int nmax , ptcres;
  nmax = 4;

  // allocate memorey for pthread_creat() arguments
  tinfo = calloc(nmax, sizeof(struct thread_info));
  if (tinfo == NULL)
    err_sys("calloc");

  // Create thread per arg
  for(int i = 0; i < nmax; i++)
  {
    tinfo[i].thread_num = i; 
    tinfo[i].player = i+1;
    tinfo[i].fs = fs;

    // the pthread_create() call stores the thread id into corresponding element 
    // of tinfo[]
    switch(i % 4)
    {
      case 0:
        ptcres = pthread_create(&tinfo[i].thread_id, NULL, &strategy1, &tinfo[i]);
        break;
      case 1:
        ptcres = pthread_create(&tinfo[i].thread_id, NULL, &strategy2, &tinfo[i]);
        break;
      case 2:
        ptcres = pthread_create(&tinfo[i].thread_id, NULL, &strategy3, &tinfo[i]);
        break;
      case 3:
      default:
        ptcres = pthread_create(&tinfo[i].thread_id, NULL, &strategy4, &tinfo[i]);
      
        break;
    }

    if (ptcres != 0)
      handle_error(ptcres, "pthread_create", PROCESS_EXIT);

    printf("thread created: %d; \n",
        tinfo[i].thread_num);
  }


  // join with each thread, and display its returned value 
  for (int i = 0; i < nmax; i++) {
    ptcres = pthread_join(tinfo[i].thread_id, &res);
    if (ptcres != 0)
        handle_error(ptcres, "pthread_join", PROCESS_EXIT);

    printf("Joined with thread %d; \n",
            tinfo[i].thread_num); // , (char *) res );
    //free(res);      // Free memory allocated by thread 
  }


  free(tinfo);
}

*/