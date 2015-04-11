/* (C) Hauronen Patronens waste of time projects!
 * https://github.com/chubbson/zhaw_os_linux
 * Author: David Hauri - hauridav
 * Date: 2015-04-07
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
**/

#include <gamehelper.h>

void printgamestate(hpctf_game* hpctf)
{
	switch(hpctf->gamestate)
	{
		case RUNNING:
			printf("%s\n", "gamestate: RUNNING");
			break;
		case WAITING4PLAYERS:
			printf("%s\n", "gamestate: WAITING4PLAYER");
			break;
		case FINISHED:
			printf("%s\n", "gamestate: FINISHED");
			break;
		default: 
			printf("%s\n", "gamestate: 'unknown'");
			break;
	}
}