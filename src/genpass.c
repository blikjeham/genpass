/* 
 *    genpass - a random password generator
 *    Copyright (C) 2010 Jabik Postmus and Sjoerd Job Postmus
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id: genpass.c,v 1.5 2010/09/30 17:29:18 jabik Exp $
 */

#include <stdarg.h>
#include "genpass.h"
#include "arguments.h"

char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
char digits[] = "0123456789";
char extras[] = "~`!@#$%^&*(){}[]',.\"<>/=\\?+|;:-_";

void verbose(char *fmt, ...)
{
	extern struct arguments *arguments;
	if (arguments->verbose) {
		va_list va;
		va_start(va, fmt);
		vfprintf(stdout, fmt, va);
		va_end(va);
	}
}

void printWaarden(struct settings *waarden)
{
	printf("%d,%d,%d,%d,%d\n",waarden->lowercase, waarden->uppercase, waarden->digits, waarden->extras, waarden->length);
}

char *stripchar(char *string,char endchar)
{
	int i = 0;
	while(string[i]!='\0') {
		if(string[i]==endchar)
			string[i]='\0';
		i++;
	}
	return string;
}

// This function reads a random amount of chars from
// /dev/random.
// It will return this value, and it will be used as
// the seed for the rest of the random's.
int getSeedFromDev(int randomchars) {
	// Open the /dev/urandom file for seeding.
	FILE *randomfile = fopen ("/dev/urandom", "r");
	int randombits = 0;
	int i;
	for (i=0;i<randomchars;i++) {
		randombits += getc(randomfile);
	}
	fclose(randomfile);
	return(randombits);
}

// Auxilery function for validsettings
int max(int fst, int snd)
{
	return fst > snd ? fst : snd;
}

int randTo(int n)
{
	if ( n == 0 )
		return 0;
	else
		return random() % n;
}

struct arguments *processSettings(struct arguments *parser,
				  struct arguments *waarden)
{
	int used=0;
	int l,u,d,e,n = -2;

	l=parser->lower; u=parser->upper;
	d=parser->digits; e=parser->extras;
	n=parser->length;
	if ((l != -2) || (u != -2) || (d != -2) ||
	    (e != -2) || (n != -2)) {
		if (l != -2) {
			waarden->lower=l;
			used = used+l;
		}
		if (u != -2) {
			waarden->upper=u;
			used = used+u;
		}
		if (d != -2) {
			waarden->digits=d;
			used = used+d;
		}
		if (e != -2) {
			waarden->extras=e;
			used = used+e;
		}
		if (n != -2) {
			waarden->length=n;
		}
		if ((waarden->length - used) < 0) {
			// This is an error situation.
			// How should we handle this?
			foutje();
		}
	}
	// Since we don't really do something with the seed,
	// except seed random, we can safely parse it directly.
	// But we have to check if it's null or not.
	// Else the configfile doesn't work for the seed.
	if (parser->seed != 0)
		waarden->seed = parser->seed;
	return(waarden);

}

// Check if there is enough room to store the items.
int validsettings(struct arguments *setts)
{
	int needed = 0;
	int positive = 0;
	if ( setts->upper >= 0 )
	{
		needed += setts->upper;
		positive++;
	}
	if ( setts->lower >= 0 )
	{
		needed += setts->lower;
		positive++;
	}
	if ( setts->digits >= 0 )
	{
		needed += setts->digits;
		positive++;
	}
	if ( setts->extras >= 0 )
	{
		needed += setts->extras;
		positive++;
	}
	return ( needed <= setts->length && positive > 0 );
}

char randomfromlist(char *list, int length)
{
	return list[randTo(length)];
}

void putcharinpwd(char* password, char c, int slot)
{
	while ( password[0] != 0 )
	{
		if ( password[0] == ' ' )
		{
			slot--;

			if ( slot < 0 )
			{
				password[0] = c;
				return;
			}
		}
		password++;
	}
}

int fillRandom(char* dst, char* src, int free, int amount)
{
	int length = strlen(src);
	while ( amount > 0 && free > 0 )
	{
		putcharinpwd(dst, randomfromlist(src, length),
			     randTo(free--));
		amount--;
	}
	return free;
}

char* genpass(struct arguments *setts)
{
	char *password = malloc(setts->length + 1);
	memset(password, ' ', setts->length);
	password[setts->length] = 0;

	int free = setts->length;

	free = fillRandom(password, uppercase, free, setts->upper);
	free = fillRandom(password, lowercase, free, setts->lower);
	free = fillRandom(password, digits,    free, setts->digits);
	free = fillRandom(password, extras,    free, setts->extras);

	// We now know that all values (setts.(...)) are either -1 or 0.
	char *randomList = malloc(
		(setts->upper+1)*strlen(uppercase) +
		(setts->lower+1)*strlen(lowercase) +
		(setts->digits+1)*strlen(digits   ) +
		(setts->extras+1)*strlen(extras   ) + 1);
	randomList[0] = '\0';

	if ( setts->upper >= 0 ) strcat(randomList, uppercase);
	if ( setts->lower >= 0 ) strcat(randomList, lowercase);
	if ( setts->digits >= 0 ) strcat(randomList, digits   );
	if ( setts->extras >= 0 ) strcat(randomList, extras   );

	free = fillRandom(password, randomList, free, free);

	return password;
}
