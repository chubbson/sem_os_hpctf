/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <apue.h>
#include <itskylib.h>
#include <field.h>
#include <game.h>

void usage(const char *argv)
{
	printf("USAGE:\n\n%s fieldsizeGreater3\n", argv);
  exit(1);
} 
 
/*
typedef struct {
	int n; // side length
  int** field; // field
  pthread_mutex_t mutfield[4][4];
} fldstruct;

*/
int isfinished(fldstruct *fs)
{
	int n = fs->n;
	int res = fs->field[0][0].value; 
	for (int y = 0; y < n; y++)
	{
		for (int x = 0; x < n; x++)
		{
			if(res != fs->field[y][x].value)
				return 0;
		}
	}
	return res;
}

int take(fldstruct *fs, int y, int x, int player)
{

	// todo: mod to phread_mutex_trylock
	// INUSE\n

	// lock field at x y
	pthread_mutex_lock(&fs->field[y][x].mutex);
	fs->field[y][x].value = player;
	// send taken to player
	// unlock 
	pthread_mutex_unlock(&fs->field[y][x].mutex);

	int res = isfinished(fs); 
	if(res > 0)
	{
		printf("END %d \n", res);
		return 0;
	} 

	return 1;
}

struct thread_info {     // Used as argument to thread_start() 
  pthread_t thread_id;   // ID returned by pthread_create() 
  int       thread_num;  // Application-defined thread # 
  int     	player;
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
	int resval = 0; 
	int n = tinfo->fs->n;

  for(int i = 0; i < 3; i++)
  {
  	for(int y = 0; y < n; y++)
	  {
	  	for(int x = 0; x < n; x++)
	 		{
	  		resval = take(tinfo->fs, x, y, tinfo->player);
	  		if (resval == 0)
	  			break;
	  		printfield(tinfo->fs);
	  		usleep(500000);
	  	}
	  	if (resval == 0)
	  		break;
	  }
  	if (resval == 0)
  		break;
	}

	return NULL;
}

void * strategy4(void * args)
{ 
	struct thread_info *tinfo = args;
	int resval = 0; 
	int n = tinfo->fs->n;

  for(int i = 0; i < 3; i++)
  {
  	for(int y = 0; y < n; y++)
	  {
	  	for(int x = 0; x < n; x++)
	 		{
	  		resval = take(tinfo->fs, y, x, tinfo->player);
	  		if (resval == 0)
	  			break;
	  		printfield(tinfo->fs);
	  		usleep(300000);
	  	}
	  	if (resval == 0)
	  		break;
	  }
	}

	return NULL;
}

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


  // join with each thread, and display its returned value */
  for (int i = 0; i < nmax; i++) {
    ptcres = pthread_join(tinfo[i].thread_id, &res);
    if (ptcres != 0)
        handle_error(ptcres, "pthread_join", PROCESS_EXIT);

    printf("Joined with thread %d; \n",
            tinfo[i].thread_num /*, (char *) res */);
    //free(res);      /* Free memory allocated by thread */
  }


  free(tinfo);
}

int main(int argc, char const *argv[])
{
	int n;
	if (argc < 2 || argc != 2 || (n = atoi(argv[1])) < 4)
    usage(argv[0]);

  printf("n: %d\n", n);

  printf("res of sizeof(fldstruct) %d\n",sizeof(fldstruct)); 
  //fldstruct fs; 

  hpctf_game * p_hpctf = inithpctf(n);
  printfield(p_hpctf->fs);

  someclients(p_hpctf->fs);

  freehpctf(p_hpctf);

/*
  hpctf_game * p_hpctf = malloc(sizeof(hpctf_game));
  inithpctf_(p_hpctf,7);
  printfield(p_hpctf->fs);
  freehpctf_(p_hpctf);
  free(p_hpctf);
  */
/*
  fldstruct * fs = malloc(sizeof(fldstruct));
  initfield(fs, 7);
  printfield(fs);
  freefield(fs);
  free(fs);
*/
//  initfield(&fs,n);


  printf("%s\n", "222222s");
//  printfield(&fs);
//  freefield(&fs);
/*
  hpctf_game hpctf;
  inithpctf(&hpctf, n);

//  someclients(hpctf.fs));

  printf("%d\n", (*hpctf.fs).n);
  //printfield(&(*hpctf.fs));
  printf("%d\n", (*hpctf.fs).n);
  printf("%d\n", hpctf.testvar);
  printf("%d\n", hpctf.testvar);
  
  printf("%d\n", (*hpctf.fs).n);

  printf("before freehpctf\n");
  freehpctf(&hpctf);
*/
  exit(0);
}

