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
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * main.c
 * 
 * For the GenPas project
 *
 * $Id: main.c,v 1.16 2011/04/17 11:32:45 jabik Exp $
 */

#include "../config.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "genpass.h"


/* Define the maximum number of config files.
 * Not counting the one given from commandline.
 * The one from commandline is at position MAXFILES in the array.
 */
#define MAXFILES 2
char *configfile[MAXFILES+1];

#ifndef CONFDIR
#define CONFDIR "/usr/local"
#endif

// Are we verbose or not?
// Can be set to 1 from commandline with -v
int verbose=0;

void usage(int exitstatus) 
{
	printf("Usage: genpass [OPTION #]...");
	printf("\nGenerate a random password according to the given options.");
	printf("\n\n  -h, --help\tPrint this screen.\n");
	printf("  -l #\t\tNumber of lowercase characters.\n");
	printf("  -u #\t\tNumber of uppercase characters.\n");
	printf("  -d #\t\tNumber of digits.\n");
	printf("  -e #\t\tNumber of extra, non alpha-numerical characters.\n");
	printf("  -n #\t\tLength of the password.\n");
	printf("  -f configfile\tAlternative config file. Default is: %s/genpass.conf.\n",CONFDIR);
	printf("  -s #\t\tThe custom seed for random. Use it with care.\n");
	printf("  -v\t\tVerbose, thus showing the settings in use.\n");
	printf("  -V\t\tPrint version.\n");
	printf("Setting a value to -1 disables it.\n");
	printf("\nExamples:\n  genpass -n 6 -l 2 -u 2 -d 2\tCreates a password with 6 characters, of wich 2 lowercase, 2 uppercase and 2 digits.\n");
	printf("  genpass -n 10 -l 2 -u 2 -d 2 -e 2\tCreates a password with 10 characters, and at least 2 of every type of character and fills in the rest.\n");
	exit(exitstatus);
}

void version() 
{
	printf("Genpass %s\nBy Jabik and Sjoerd Job Postmus\n\n",VERSION);
	printf("This program comes with ABSOLUTELY NO WARRANTY.\n"); 
	printf("This is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions.\n");
	exit(0);

}

void help()
{
	printf("Genpass %s\nBy Jabik and Sjoerd Job Postmus\n\n",VERSION);
	usage(0);
}

void foutje() 
{
	fprintf(stderr,"Human error in arguments.\n\n");
	usage(1);
}

void printsettings(struct settings *waarden) 
{
	if (waarden->seed != 0)
		printf("genpass -n %d -u %d -l %d -d %d -e %d -s %d\n",waarden->length,waarden->uppercase,waarden->lowercase,waarden->digits,waarden->extras,waarden->seed);
	else
		printf("genpass -n %d -u %d -l %d -d %d -e %d\n",waarden->length,waarden->uppercase,waarden->lowercase,waarden->digits,waarden->extras);
		
}

void printsettingserr(struct settings *waarden) 
{
	fprintf(stderr,"genpass -n %d -u %d -l %d -d %d -e %d\n", waarden->length,waarden->uppercase,waarden->lowercase,waarden->digits,waarden->extras);
}

int iscijfer(char *argument) 
{
	int i=0;
	int waar=1;
	while ((i < strlen(argument)) && (waar)) {
		if ((isdigit(argument[i])) || (argument[i] == '-') || (argument[i] == '+')) {
			waar = 1;
			i++;
		} else {
			waar = 0;
		}
	}
	return(waar);
}

int readInteger(char *value)
{
	if ( iscijfer(value) ) {
		return atoi(value);
	}
	foutje();
}

/* 
 * Let's read some settings from the config file.
 */
struct settings *readConfFile(char *conffile) 
{
	char *data,*key;
	FILE *conf;
	size_t len = 0;
	ssize_t read;
	int pos;
	int l,u,d,e,n=-2;
	struct settings *parser = malloc(sizeof(struct settings));

	conf=fopen(conffile,"r");
	if (! conf) {
		fprintf(stderr, "Unable to open %s\n", conffile);
		return(NULL);
	} else {
		while ((read = getline(&data, &len, conf)) != -1) {
			/* Strip the conffile from the newlines and the ';' */
			data = strsep(&data, "\n;");
			key = strsep(&data," =");	
			if (!strcmp(key,"DIGIT"))
				d=readInteger(data);
			if (!strcmp(key,"LOWER"))
				l=readInteger(data);
			if (!strcmp(key,"UPPER"))
				u=readInteger(data);
			if (!strcmp(key,"EXTRA"))
				e=readInteger(data);
			if (!strcmp(key,"LENGTH")) 
				n=readInteger(data);
			/* Use this seed option in the config file with care */
			if (!strcmp(key,"SEED")) {
				if (verbose) printf("seed is: ");
				parser->seed=readInteger(data);
				if (verbose) printf("%d\n",parser->seed);
			}
		}
		fclose(conf);
		parser->lowercase=l;
		parser->uppercase=u;
		parser->digits=d;
		parser->extras=e;
		parser->length=n;
		return(parser);
	}
}

struct settings *readParameters(int arguc, char **arguv) 
{
	int i;
	int l,u,d,e,n;
	struct settings *parser = malloc(sizeof(struct settings));
	/*
	 * this next int is used to see how many mandatory chars are used.
	 * Substract that from the total lenght, and we know how many chars
	 * we still can use for the rest.
	 */
	int used=0;
	l=u=d=e=n=-2;
	for (i=1; i<arguc; i++) {
		if (!isalpha(arguv[i][0])) {
			switch (arguv[i][1]) {
				case '-':
					help();
					break;
				case 'h':
					help();
					break;
				case 'l':
					l = readInteger(arguv[i+1]);
					i++;
					break;
				case 'u':
					u = readInteger(arguv[i+1]);
					i++;
					break;
				case 'd':
					d = readInteger(arguv[i+1]);
					i++;
					break;
				case 'e':
					e = readInteger(arguv[i+1]);
					i++;
					break;
				case 'n':
					n = readInteger(arguv[i+1]);
					i++;
					break;
				case 'f':
					configfile[MAXFILES] = arguv[i+1];
					i++;
					break;
				case 's' :
					parser->seed = readInteger(arguv[i+1]);
					i++;
					break;
				case 'v':
					verbose=1;
					break;
				case 'V':
					version();
					break;
				default:
					help();
					break;
			}
		}
	}
	parser->digits=d;
	parser->lowercase=l;
	parser->uppercase=u;
	parser->extras=e;
	parser->length=n;
	return(parser);
}

int main(int argc, char *argv[]) 
{
	struct settings *waarden = malloc(sizeof(struct settings));
	struct settings *parser1 = malloc(sizeof(struct settings));
	struct settings *parser2 = malloc(sizeof(struct settings));

	/* for the stat part we need this one. We don't use it, but we need it. */
	struct stat *bluf= malloc(sizeof(struct stat));

	int seed,i;
	int ret;
	char *filename = "/genpass.conf";

	/* Fill the var configfile */
	configfile[0]=malloc(strlen(CONFDIR)+strlen(filename));
	strcat(configfile[0],CONFDIR);
	strcat(configfile[0],filename);
	configfile[1]=strcat(getenv("HOME"),"/.genpass");
	
	/* Set the default values. */
	waarden->length = 8;
	waarden->uppercase = 1;
	waarden->lowercase = 1;
	waarden->digits = 1;
	waarden->extras = 1;
	waarden->seed = 0;

	/* Get some values from commandline. */
	parser1=readParameters(argc,argv);

	/* 
	 * Now we will read the configfiles.
	 * Possibilities are stored in char *configfile[MAXFILES].
	 *
	 * First we will stat() the files.
	 * Then read the configfile.
	 * Then we have to process the settings for each file.
	 * As you can see the last one overrides the previous ones,
	 * with the one from commandline as supreme ruler.
	 */
	for (i=0;i<=MAXFILES;i++) {
		if (verbose) printf("Trying configfile: %s ...", configfile[i]);
		ret = stat(configfile[i],bluf);
		if (ret != -1) {
			if (verbose) printf("ok\n");
			parser2=readConfFile(configfile[i]);
			if (parser2 == NULL)
				break;
			waarden = processSettings(parser2,waarden);
			if (verbose) printsettings(parser2);
		} else {
			if (verbose) printf("no\n");
		}
	}

	/* override configfile with commandline */
	waarden = processSettings(parser1,waarden);
	
	/* 
	 * Check if there is an override in the seed.
	 * Overriding the seed should only be done to debug or test. 
	 */
	if (waarden->seed == 0) {
		/* 
		 * We don't want to seed with time entirely, but we have to do it for now.
		 * We are going to seed twice. Once with time, once with the output from
		 * getSeedFromDev
		 */
		srandom(time(NULL));
		seed = abs(getSeedFromDev(random() % 1024));
		srandom(seed);
	} else {
		seed = waarden->seed;
		srandom(seed);
	}

	if (!validsettings(*waarden)) {
		fprintf(stderr,"Error: The sum of the tokens is larger than the desired lenght of the password.\nPlease fix this problem.\n");
		printsettingserr(waarden);
		exit(1);
	} else {
		if (verbose) {
			printsettings(waarden);
			printf("Random seed is: %d\n", seed);
			printf("\n");
		}
		printf("%s\n", genpass(*waarden));
	}
	return 0;
}
