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
#include <time.h>
#include "arguments.h"
#include "genpass.h"


/* Define the maximum number of config files.
 * Not counting the one given from commandline.
 * The one from commandline is at position MAXFILES in the array.
 */
#define MAXFILES 2
char *configfile[MAXFILES+1];

#ifndef CONFDIR
#define CONFDIR "/usr/local"
#endif /* CONFDIR */

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

void printsettings(struct arguments *waarden) 
{
	if (waarden->seed != 0)
		printf("genpass -n %d -u %d -l %d -d %d -e %d -s %d\n",
		       waarden->length,waarden->upper,waarden->lower,
		       waarden->digits,waarden->extras,waarden->seed);
	else
		printf("genpass -n %d -u %d -l %d -d %d -e %d\n",
		       waarden->length,waarden->upper,waarden->lower,
		       waarden->digits,waarden->extras);
		
}

void printsettingserr(struct arguments *waarden) 
{
	fprintf(stderr,"genpass -n %d -u %d -l %d -d %d -e %d\n",
		waarden->length,
		waarden->upper,
		waarden->lower,
		waarden->digits,
		waarden->extras);
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
	return(-1);
}

/* 
 * Let's read some settings from the config file.
 */
int readConfFile(char *conffile, struct arguments *parser) 
{
	char *data,*key;
	FILE *conf;
	size_t len = 0;
	ssize_t read;
	int n, u, l, d, e;
	n = u = l = d = e = -2;
	/* struct arguments *parser = malloc(sizeof(struct arguments)); */

	conf=fopen(conffile,"r");
	if (! conf) {
		fprintf(stderr, "Unable to open %s\n", conffile);
		return(-1);
	} else {
		while ((read = getline(&data, &len, conf)) != -1) {
			/* Strip the conffile from the newlines
			   and the ';' */
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
			/* Use this seed option in the config
			   file with care */
			if (!strcmp(key,"SEED")) {
				parser->seed=readInteger(data);
				verbose("seed = %d\n",parser->seed);
			}
		}
		fclose(conf);
		parser->lower=l;
		parser->upper=u;
		parser->digits=d;
		parser->extras=e;
		parser->length=n;
		return(0);
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
				/* case 'v': */
				/* 	verbose=1; */
				/* 	break; */
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
	/* struct settings *waarden = malloc(sizeof(struct settings)); */
	/* struct settings *parser1 = malloc(sizeof(struct settings)); */
	struct settings *parser2 = malloc(sizeof(struct settings));

	extern struct arguments *arguments;
	arguments = malloc(sizeof(struct arguments));
	memset(arguments, 0, sizeof(struct arguments));
	extern struct arguments *file_args;
	file_args = malloc(sizeof(struct arguments));
	memset(file_args, 0, sizeof(struct arguments));
	extern struct arguments *final;
	final = malloc(sizeof(struct arguments));
	memset(final, 0, sizeof(struct arguments));

	/* for the stat part we need this one. We don't use it, but we need it. */
	struct stat *bluf= malloc(sizeof(struct stat));

	int seed,i;
	int ret;
	char *filename = "/genpass.conf";

	/* Fill the var configfile */
	configfile[0]=malloc(strlen(CONFDIR)+strlen(filename));
	strncat(configfile[0], CONFDIR, strlen(CONFDIR));
	strncat(configfile[0], filename, strlen(filename));
	configfile[1]=strcat(getenv("HOME"),"/.genpass");
	
	/* Set the default values. */
	final->length = 8;
	final->upper = 1;
	final->lower = 1;
	final->digits = 1;
	final->extras = 1;
	final->seed = 0;
	/* All values for command line arguments
	 * need to be '-2' as default, except
	 * seed. That can remain 0.
	 */
	arguments->length = -2;
	arguments->upper = -2;
	arguments->lower = -2;
	arguments->digits = -2;
	arguments->extras = -2;

	/* Get some values from commandline. */
	/* parser1=readParameters(argc,argv); */
	argp_parse(&argp, argc, argv, 0, 0, arguments);

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
		if (configfile[i] != NULL) {
			verbose("Trying configfile: %s ...",
				configfile[i]);
			ret = stat(configfile[i],bluf);
			if (ret != -1) {
				verbose("ok\n");
				/* parser2=readConfFile(configfile[i]); */
				readConfFile(configfile[i], file_args);
				if (parser2 == NULL)
					break;
				final = processSettings(file_args, final);
				if (arguments->verbose)
					printsettings(file_args);
			} else {
				verbose("no\n");
			}
		}
	}

	/* override configfile with commandline */
	final = processSettings(arguments, final);
	
	/* 
	 * Check if there is an override in the seed.
	 * Overriding the seed should only be done to debug or test. 
	 */
	if (final->seed == 0) {
		/* 
		 * We don't want to seed with time entirely, but we have
		 * to do it for now.
		 * We are going to seed twice. Once with time, once with
		 * the output from
		 * getSeedFromDev
		 */
		srandom(time(NULL));
		seed = abs(getSeedFromDev(random() % 1024));
		srandom(seed);
	} else {
		seed = final->seed;
		srandom(seed);
	}

	if (!validsettings(final)) {
		fprintf(stderr,"Error: The sum of the tokens is larger than the desired lenght of the password.\nPlease fix this problem.\n");
		printsettingserr(final);
		exit(1);
	} else {
		if (arguments->verbose) {
			printsettings(final);
			printf("Random seed is: %d\n", seed);
			printf("\n");
		}
		printf("%s\n", genpass(final));
	}
	return 0;
}
