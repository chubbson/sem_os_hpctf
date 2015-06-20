/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-03-29
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <somecolor.h>
#include <field.h>

fldstruct * initfield(int n)
{
	fldstruct * p_fs = malloc(sizeof(fldstruct)); 
	p_fs->n = n;

	p_fs->field = malloc(n * sizeof(square*));
	for (int i = 0; i < n; i++)	
		p_fs->field[i] = malloc(n * sizeof(square));

	for(int y = 0; y < n; y++)
		for (int x = 0; x < n; x++)
			if ((p_fs->field[y][x].flag = 0) == 0
				  && pthread_mutex_init(&p_fs->field[y][x].mutex,NULL) != 0) 
		    printf("\n mutex #'%d'|'%d' init failed\n", y, x);

	return p_fs;
}

void freefield(fldstruct *fs)
{
	int n = fs->n;

 	for(int y = 0; y < n; y++)
 	{
 		for(int x = 0; x < n; x++)
 		{
 			if (pthread_mutex_destroy(&fs->field[y][x].mutex) != 0)
	    	printf("\n mutex #'%d'|'%d' in mutarr - clean failed\n", y, x);
	  }
	  free(fs->field[y]);
	}
	free(fs->field);
	free(fs);
}

int isfinished(fldstruct *fs)
{
	int n = fs->n;
	int res = fs->field[0][0].flag; 
	for (int y = 0; y < n; y++)
		for (int x = 0; x < n; x++)
			if(res != fs->field[y][x].flag)
				return 0;

	return res;
}

void printfield(fldstruct *fs)
{
	int n2 = fs->n + 2;
	char * prntfld;

	// alloc enough 
	int res = (n2*n2*15 + n2 + 1) * sizeof(char);
	if((prntfld = malloc(res)) != NULL)
	{
		prntfld[0] = '\0';
		// top border
		for (int i = 0; i < (fs->n+2); ++i)
		  sprintcolfield(-1, prntfld);
		strcat(prntfld, "\n");

		// print field line 
		for (int y = 0; y < fs->n; ++y)
		{
			// left border
			sprintcolfield(-1, prntfld);

			for(int x = 0; x < fs->n; ++x)
			{
				sprintcolfield(fs->field[y][x].flag, prntfld);
			}

			// right border
			sprintcolfield(-1, prntfld);
			strcat(prntfld, "\n");
		}

		// bottom border
		for (int i = 0; i < (fs->n+2); ++i)
		  sprintcolfield(-1, prntfld);
		strcat(prntfld, "\n");

		int res2 = strlen(prntfld);
		prntfld[res2] = '\0';

		printf("%s", prntfld);//%d\n%d\n", prntfld, res2, res);
		//write(STDOUT_FILENO, prntfld, res2);
		//fflush(STDOUT_FILENO);
		free(prntfld);
	}
	else
	{
		// malloc failed 
		// exit ? 
	}
	return; 
}
