/*
 * arguments.c
 *
 * This file is part of genpass.
 *
 * genpass is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * genpass is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with genpass.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 * $URL$
 */
#define _WITH_GETLINE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "arguments.h"
#include "genpass.h"

void foutje(void) {
    fprintf(stderr, "Arguments are inconsistent.\n");
}

void printsettings(struct arguments *a) 
{
    printf("genpass -c %d -n %d -u %d -l %d -d %d -e %d",
	   a->count,
	   a->length, a->upper, a->lower,
	   a->digits, a->extras);
    if (a->seed != 0)
	printf(" -s %d", a->seed);
    printf("\n");
}

void printsettingserr(struct arguments *a)
{
	fprintf(stderr,"genpass -c %d -n %d -u %d -l %d -d %d -e %d\n",
		a->count,
		a->length,
		a->upper,
		a->lower,
		a->digits,
		a->extras);
}

static void print_help(void)
{
    printf("Genpass - A random password generator.\n\n");
    printf("\t-c #\t\tThe number of passwords to generate\n"
	   "\t-n #\t\tThe length of the password\n"
	   "\t-u #\t\tThe minimum of uppercase characters\n"
	   "\t-l #\t\tThe minimum of lowercase characters\n"
	   "\t-d #\t\tThe minimum of digits\n"
	   "\t-e #\t\tThe minimum of extra characters\n"
	   "\t-s #\t\tThe seed for random functions (only for debugging)\n"
	   "\t-f FILE\t\tSupply a config file\n"
	   "\t-h\t\tDisplay this message\n");
    exit(0);
}

int parse_arg(int argc, char **argv)
{
    extern struct arguments *arguments;
    char c;
    while ((c = getopt(argc, argv, "vhc:n:u:l:d:e:s:f:")) != -1) {
	switch(c) {
	case 'v':
	    arguments->verbose++;
	    break;
	case 'c':
	    arguments->count = atoi(optarg);
	    break;
	case 'n':
	    arguments->length = atoi(optarg);
	    break;
	case 'u':
	    arguments->length = atoi(optarg);
	    break;
	case 'l':
	    arguments->lower = atoi(optarg);
	    break;
	case 'd':
	    arguments->digits = atoi(optarg);
	    break;
	case 'e':
	    arguments->extras = atoi(optarg);
	    break;
	case 's':
	    arguments->seed = atoi(optarg);
	    break;
	case 'f':
	    configfile[MAXFILES] = malloc(strlen(optarg));
	    strncpy(configfile[MAXFILES], optarg, strlen(optarg));
	    break;
	case 'h':
	    print_help();
	    return 0;
	default:
	    return 1;
	}
    }
    return 0;
}

/* 
 * Let's read some settings from the config file.
 */
int readConfFile(char *conffile, struct arguments *parser)
{
    char *data;
    char *key;
    FILE *conf;
    size_t size;
    int c, n, u, l, d, e;
    n = u = l = d = e = -2;
    c = 1;

    data = malloc(MAXLINE + 1);
    size = MAXLINE - 1;
    conf = fopen(conffile, "r");
    if (!conf) {
        fprintf(stderr, "Unable to open %s\n", conffile);
	return(-1);
    } else {
	while ((getline(&data, &size, conf)) != -1) {
	    if (data == NULL)
		break;
	    /* Strip the conffile from the newlines
	       and the ';' */
	    data = strsep(&data, "\n;");
	    key = strsep(&data,"=");
	    if (!strcmp(key, "COUNT"))
		c=atoi(data);
	    if (!strcmp(key,"DIGIT"))
		d=atoi(data);
	    if (!strcmp(key,"LOWER"))
		l=atoi(data);
	    if (!strcmp(key,"UPPER"))
		u=atoi(data);
	    if (!strcmp(key,"EXTRA"))
		e=atoi(data);
	    if (!strcmp(key,"LENGTH"))
		n=atoi(data);
	    /* Use this seed option in the config
	       file with care */
	    if (!strcmp(key,"SEED")) {
		parser->seed=atoi(data);
		verbose("seed = %d\n",parser->seed);
	    }
	}
	fclose(conf);
	parser->count=c;
	parser->lower=l;
	parser->upper=u;
	parser->digits=d;
	parser->extras=e;
	parser->length=n;
	return(0);
    }
}

void set_default(struct arguments *arg)
{
    arg->count = 1;
    arg->length = 8;
    arg->upper = 1;
    arg->lower = 1;
    arg->digits = 1;
    arg->extras = 1;
    arg->seed = 0;
}

void set_empty(struct arguments *arg)
{
    arg->count = 1;
    arg->length = -2;
    arg->upper = -2;
    arg->lower = -2;
    arg->digits = -2;
    arg->extras = -2;
}
